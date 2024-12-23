#ifdef ARDUINO_ARCH_ESP32
    #include "OpenKNX/Led/Serial.h"
    #include "OpenKNX/Facade.h"

    #include "esp_log.h"
    #include "esp_system.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/timers.h"

namespace OpenKNX
{
    namespace Led
    {
        void Serial::init(long num, SerialLedManager *manager, uint8_t r, uint8_t g, uint8_t b)
        {
            // no valid pin
            if (num < 0 || manager == nullptr)
                return;

            _pin = num;
            _manager = manager;
            setColor(r, g, b);
        }

        /*
         * write led state based on bool and _brightness
         */
        void Serial::writeLed(uint8_t brightness)
        {
            // no valid pin
            if (_pin < 0 || _manager == nullptr) return;

            if (_currentLedBrightness != brightness)
            {
                _manager->setLED(
                    _pin,
                    ((uint32_t)color[0] * brightness * _maxBrightness / 100 / 256),
                    ((uint32_t)color[1] * brightness * _maxBrightness / 100 / 256),
                    ((uint32_t)color[2] * brightness * _maxBrightness / 100 / 256));

                _currentLedBrightness = brightness;
            }
        }

    #define BITS_PER_LED_CMD 24

    #define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)


    static const rmt_symbol_word_t ws2812_zero = {
        .duration0 = (uint16_t)3, // T0H=0.3us
        .level0 = 1,
        .duration1 = (uint16_t)9, // T0L=0.9us
        .level1 = 0,
    };

    static const rmt_symbol_word_t ws2812_one = {
        .duration0 = (uint16_t)9, // T1L=0.9us
        .level0 = 1,
        .duration1 = (uint16_t)3, // T1H=0.3us
        .level1 = 0,
    };

    //reset defaults to 50uS
    static const rmt_symbol_word_t ws2812_reset = {
        .duration0 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
        .level0 = 1,
        .duration1 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
        .level1 = 0,
    };

        /*
         * Set the color of the RGB LED
         */
        void Serial::setColor(uint8_t r, uint8_t g, uint8_t b)
        {
            color[0] = r;
            color[1] = g;
            color[2] = b;
            _manager->setLED(_pin, (color[0] * (uint16_t)_currentLedBrightness) / 256, (color[1] * (uint16_t)_currentLedBrightness) / 256, (color[2] * (uint16_t)_currentLedBrightness) / 256);
        }

        void SerialLedManager::init(uint8_t ledPin, uint8_t rmtChannel, uint8_t ledCount)
        {
            _rmtChannel = rmtChannel;
            _ledCount = ledCount;
            rmt_tx_channel_config_t config = // RMT_DEFAULT_CONFIG_TX((gpio_num_t)ledPin, (rmt_channel_t)rmtChannel);
                                                    {
                                            .gpio_num = (gpio_num_t)ledPin,
                                            .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
                                            .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
                                            .mem_block_symbols = (ledCount * BITS_PER_LED_CMD) , // increase the block size can make the LED less flickering
                                            .trans_queue_depth = 0, // set the number of transactions that can be pending in the background
                                        };

            //_rmtItems = new rmt_item32_t[_ledCount * BITS_PER_LED_CMD + 1];
            _ledData = new uint32_t[_ledCount];

            // initalize with all LEDs off
            for (int i = 0; i < BITS_PER_LED_CMD * _ledCount; i++)
            {
                //_rmtItems[i].level0 = 1;
                //_rmtItems[i].duration0 = T0H;
                //_rmtItems[i].level1 = 0;
                //_rmtItems[i].duration1 = T1H;
            }

            // Initialize the RMT driver
            //if (rmt_config(&config) != ESP_OK)
            {
                logError("SerialLedManager", "Configuration of RMT driver failed");
                return;
            }
            //if (rmt_driver_install(config.channel, 0, 0) != ESP_OK)
            {
                logError("SerialLedManager", "Installation of RMT driver failed");
                return;
            }

            writeLeds();

            // Timer-Handle erstellen
            _timer = xTimerCreate(
                "SerialLedManager", // Name des Timers
                pdMS_TO_TICKS(10),  // Timer-Periode in Millisekunden (hier 1 Sekunde)
                pdTRUE,             // Auto-Reload (Wiederholung nach Ablauf)
                (void *)0,          // Timer-ID (kann für Identifikation verwendet werden)
                [](TimerHandle_t timer) {
                    openknx.progLed.loop();
    #ifdef INFO2_LED_PIN
                    openknx.info2Led.loop();
    #endif
    #ifdef INFO1_LED_PIN
                    openknx.info1Led.loop();
    #endif
    #ifdef INFO3_LED_PIN
                    openknx.info3Led.loop();
    #endif
    #ifdef OPENKNX_SERIALLED_ENABLE
                    openknx.ledManager.writeLeds();
    #endif
                } // Callback-Funktion, die beim Timeout aufgerufen wird
            );

            // Überprüfen, ob der Timer erfolgreich erstellt wurde
            if (_timer == NULL)
            {
                logError("SerialLedManager", "Timer creation failed");
                return;
            }

            // Timer starten
            if (xTimerStart(_timer, 0) != pdPASS)
            {
                logError("SerialLedManager", "Could not start Timer");
                return;
            }
        }

        void SerialLedManager::setLED(uint8_t ledAdr, uint8_t r, uint8_t g, uint8_t b)
        {
            uint32_t newrgb = (g << 16) | (r << 8) | b;
            if (_ledData[ledAdr] != newrgb)
            {
                _ledData[ledAdr] = newrgb;
                _dirty |= (1 << ledAdr);
            }
        }

        void SerialLedManager::fillRmt()
        {
            for (int j = 0; j < _ledCount; j++)
            {
                if (_dirty & (1 << j))
                {
                    uint32_t colorbits = _ledData[j];
                    for (int i = 0; i < BITS_PER_LED_CMD; i++)
                    {
                        if (colorbits & (1 << (23 - i)))
                        {
                            //_rmtItems[j * BITS_PER_LED_CMD + i].duration0 = T0L;
                            //_rmtItems[j * BITS_PER_LED_CMD + i].duration1 = T1L;
                        }
                        else
                        {
                            //_rmtItems[j * BITS_PER_LED_CMD + i].duration0 = T0H;
                            //_rmtItems[j * BITS_PER_LED_CMD + i].duration1 = T1H;
                        }
                    }
                }
            }
        }

        void SerialLedManager::writeLeds()
        {
            if (!_dirty)
                return;

            if (delayCheckMillis(_lastWritten, 5)) // prevent calling a new rmt transmission into an running on
            {
                _lastWritten = millis();
                // uint32_t t1 = micros();
                fillRmt();
                // uint32_t t2 = micros();
                //rmt_write_items((rmt_channel_t)_rmtChannel, _rmtItems, _ledCount * BITS_PER_LED_CMD, false);
                _dirty = 0;
                // uint32_t t3 = micros();

                //::Serial.print("fillRmt: ");
                //::Serial.print(t2-t1);
                //::Serial.print("us. rmt write: ");
                //::Serial.print(t3-t2);
                //::Serial.println("us");
            }
        }
    } // namespace Led
} // namespace OpenKNX

#endif