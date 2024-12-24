#include "OpenKNX/Facade.h"

#ifdef ARDUINO_ARCH_RP2040
    #include "LittleFS.h"
#endif

#if defined(ARDUINO_ARCH_RP2040) && defined(USE_TP_RX_QUEUE) && defined(USE_KNX_DMA_UART) && defined(USE_KNX_DMA_IRQ) && (MASK_VERSION == 0x07B0 || MASK_VERSION == 0x091A)
// Definiert eine Interrupt-Service-Routine (ISR) für den KNX-Datenempfang.
// Diese Funktion wird bei einem Empfangsereignis ausgelöst und verarbeitet die empfangenen Daten.
void __time_critical_func(processKnxRxISR)()
{
    // Löscht die Interrupts für "Receive Timeout" und "Receive" im UART.
    uart_get_hw(KNX_DMA_UART)->icr = UART_UARTICR_RTIC_BITS | UART_UARTICR_RXIC_BITS;
    #if MASK_VERSION == 0x07B0
    // Verarbeitet die empfangenen Daten mit der primären Datenverbindungsschicht (Data Link Layer).
    knx.bau().getDataLinkLayer()->processRxISR();
    #elif MASK_VERSION == 0x091A
    // Verarbeitet die empfangenen Daten mit einer sekundären Datenverbindungsschicht.
    knx.bau().getSecondaryDataLinkLayer()->processRxISR();
    #endif
}
#endif

namespace OpenKNX
{
    // Initialisiert die grundlegende Hardware und Plattform-spezifische Komponenten.
    void Hardware::init()
    {
        // Initialisiert den Flash-Speicher (derzeit auskommentiert, kann bei Bedarf aktiviert werden).
        // initFlash();

#ifdef ARDUINO_ARCH_RP2040
        // Aktiviert die ADC-Hardware und den internen Temperatursensor.
        adc_init();
        adc_set_temp_sensor_enabled(true);

        // Initialisiert das Dateisystem auf RP2040-Plattformen.
        initFilesystem();
#endif
    }

    // Konfiguriert die LEDs basierend auf der Plattform (GPIO- oder serielle LEDs).
    void Hardware::initLeds()
    {
#ifdef OPENKNX_SERIALLED_ENABLE
    #ifndef PROG_LED_COLOR
        #define PROG_LED_COLOR 63, 0, 0
    #endif
        // Initialisiert den LED-Manager für serielle LEDs.
        openknx.ledManager.init(OPENKNX_SERIALLED_PIN, 0, OPENKNX_SERIALLED_NUM);

        // Konfiguriert die Programmier-LED mit einer Standardfarbe.
        openknx.progLed.init(PROG_LED_PIN, &(openknx.ledManager), PROG_LED_COLOR);

    #ifdef INFO1_LED_PIN
        #ifndef INFO1_LED_COLOR
            #define INFO1_LED_COLOR 0, 63, 0
        #endif
        // Initialisiert die Info-LED 1.
        openknx.info1Led.init(INFO1_LED_PIN, &(openknx.ledManager), INFO1_LED_COLOR);
    #endif
    #ifdef INFO2_LED_PIN
        #ifndef INFO2_LED_COLOR
            #define INFO2_LED_COLOR 0, 63, 0
        #endif
        // Initialisiert die Info-LED 2.
        openknx.info2Led.init(INFO2_LED_PIN, &(openknx.ledManager), INFO2_LED_COLOR);
    #endif
    #ifdef INFO3_LED_PIN
        #ifndef INFO3_LED_COLOR
            #define INFO3_LED_COLOR 0, 63, 0
        #endif
        // Initialisiert die Info-LED 3.
        openknx.info3Led.init(INFO3_LED_PIN, &(openknx.ledManager), INFO3_LED_COLOR);
    #endif
#else
        // Initialisiert die LEDs als GPIO-basierte LEDs mit aktivem Pegel.
        openknx.progLed.init(PROG_LED_PIN, PROG_LED_PIN_ACTIVE_ON);
    #ifdef INFO1_LED_PIN
        openknx.info1Led.init(INFO1_LED_PIN, INFO1_LED_PIN_ACTIVE_ON);
    #endif
    #ifdef INFO2_LED_PIN
        openknx.info2Led.init(INFO2_LED_PIN, INFO2_LED_PIN_ACTIVE_ON);
    #endif
    #ifdef INFO3_LED_PIN
        openknx.info3Led.init(INFO3_LED_PIN, INFO3_LED_PIN_ACTIVE_ON);
    #endif
#endif
    }

    // Konfiguriert die Tasten und ihre Interrupts für Ereignisänderungen (z. B. gedrückt/losgelassen).
    void Hardware::initButtons()
    {
#ifdef PROG_BUTTON_PIN
        // Konfiguriert den Programmier-Button als Eingang mit Pull-up-Widerstand.
        pinMode(PROG_BUTTON_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(PROG_BUTTON_PIN),
            []() -> void { openknx.progButton.change(!digitalRead(PROG_BUTTON_PIN)); }, CHANGE);
#endif

#ifdef FUNC1_BUTTON_PIN
        // Konfiguriert den ersten Funktions-Button.
        pinMode(FUNC1_BUTTON_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(FUNC1_BUTTON_PIN),
            []() -> void { openknx.func1Button.change(!digitalRead(FUNC1_BUTTON_PIN)); }, CHANGE);
#endif

#ifdef FUNC2_BUTTON_PIN
        // Konfiguriert den zweiten Funktions-Button.
        pinMode(FUNC2_BUTTON_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(FUNC2_BUTTON_PIN),
            []() -> void { openknx.func2Button.change(!digitalRead(FUNC2_BUTTON_PIN)); }, CHANGE);
#endif

#ifdef FUNC3_BUTTON_PIN
        // Konfiguriert den dritten Funktions-Button.
        pinMode(FUNC3_BUTTON_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(FUNC3_BUTTON_PIN),
            []() -> void { openknx.func3Button.change(!digitalRead(FUNC3_BUTTON_PIN)); }, CHANGE);
#endif
    }

    // Initialisiert die Interrupt-Service-Routinen für den KNX-Datenempfang (falls Plattform kompatibel).
    void Hardware::initKnxRxISR()
    {
#if defined(ARDUINO_ARCH_RP2040) && defined(USE_TP_RX_QUEUE) && defined(USE_KNX_DMA_UART) && defined(USE_KNX_DMA_IRQ) && (MASK_VERSION == 0x07B0 || MASK_VERSION == 0x091A)
        // Setzt die exklusive Handler-Funktion für UART-Interrupts.
        irq_set_exclusive_handler(KNX_DMA_UART_IRQ, processKnxRxISR);
        irq_set_enabled(KNX_DMA_UART_IRQ, true);
        uart_set_irq_enables(KNX_DMA_UART, true, false);
#endif
    }

    // Initialisiert den Flash-Speicher mit den spezifizierten Parametern.
    void Hardware::initFlash()
    {
        logDebug("Hardware<Flash>", "Initialize flash");
        logIndentUp();
#ifdef ARDUINO_ARCH_ESP32
        // Initialisierung des Flash-Speichers für ESP32.
        openknx.openknxFlash.init("openknx");
        openknx.knxFlash.init("knx");
#else
        // Initialisierung des Flash-Speichers für andere Plattformen mit Offset und Größe.
        openknx.openknxFlash.init("openknx", OPENKNX_FLASH_OFFSET, OPENKNX_FLASH_SIZE);
        openknx.knxFlash.init("knx", KNX_FLASH_OFFSET, KNX_FLASH_SIZE);
#endif

#ifdef KNX_FLASH_CALLBACK
        // Registriert Callback-Funktionen für KNX-spezifischen Flash-Zugriff.
        knx.platform().registerFlashCallbacks(
            []() -> uint32_t {
                return openknx.knxFlash.size();
            },
            []() -> uint8_t* {
                return openknx.knxFlash.flashAddress();
            },
            [](uint32_t relativeAddress, uint8_t* buffer, size_t len) -> uint32_t {
                return openknx.knxFlash.write(relativeAddress, buffer, len);
            },
            []() -> void {
                return openknx.knxFlash.commit();
            }
        );
#endif
        logIndentDown();
    }

#ifdef ARDUINO_ARCH_RP2040
    // Initialisiert das LittleFS-Dateisystem für RP2040.
    void Hardware::initFilesystem()
    {
        LittleFSConfig cfg;
        cfg.setAutoFormat(true); // Automatische Formatierung bei Fehlern aktivieren.
        LittleFS.setConfig(cfg);

        // Prüft, ob das Dateisystem erfolgreich gestartet werden konnte.
        if (!LittleFS.begin())
        {
            fatalError(FATAL_INIT_FILESYSTEM, "Unable to initialize filesystem");
        }
    }
#endif

    // Behandelt schwerwiegende Fehler, zeigt Fehlercodes an und blockiert das System.
    void Hardware::fatalError(uint8_t code, const char* message)
    {
        logError("FatalError", "Code: %d (%s)", code, message);
        logIndentUp();
#ifdef INFO1_LED_PIN
        // Aktiviert die Info-LED 1 im Fehlerzustand.
        openknx.info1Led.on();
#endif
        // Zeigt den Fehlercode über die Programmier-LED an.
        openknx.progLed.errorCode(code);

#if MASK_VERSION == 0x07B0
        // Stoppt die Datenverbindungsschicht bei bestimmten Hardware-Konfigurationen.
        TpUartDataLinkLayer* ddl = knx.bau().getDataLinkLayer();
        ddl->stop(true);
    #ifdef NCN5120
        ddl->powerControl(false);
    #endif
#endif
        logIndentDown();

        // Bleibt in einer Endlosschleife, um den Fehlerzustand aufrechtzuerhalten.
        while (true)
        {
#ifdef OPENKNX_WATCHDOG
            openknx.watchdog.deactivate(); // Deaktiviert den Watchdog, falls aktiviert.
#endif
            delay(2000); // Wiederholt die Fehlermeldung alle 2 Sekunden.
            logError("FatalError", "Code: %d (%s)", code, message);
        }
    }

    // Liest die CPU-Temperatur aus (nur auf RP2040-Plattformen verfügbar).
    float Hardware::cpuTemperature()
    {
#if defined(ARDUINO_ARCH_RP2040)
        adc_select_input(4); // Wählt den internen Temperatursensor als ADC-Eingang.
        const float conversionFactor = 3.3f / (1 << 12); // ADC-Wert in Spannung umrechnen.
        float adc = (float)adc_read() * conversionFactor;
        // Berechnet die Temperatur basierend auf den RP2040-Datenblattspezifikationen.
        return 27.0f - (adc - 0.706f) / 0.001721f;
#else
        return 0.0f; // Gibt 0 zurück, wenn die Funktion nicht unterstützt wird.
#endif
    }
} // namespace OpenKNX
