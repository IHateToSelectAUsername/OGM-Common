#include "OpenKNX/Common.h"
#include "OpenKNX/Facade.h"
#include "OpenKNX/Stat/RuntimeStat.h"

#if defined(OPENKNX_DUALCORE) && defined(ARDUINO_ARCH_ESP32)
extern void loop1();
extern void setup1();
#endif

namespace OpenKNX
{
    /*
     * Gibt das Log-Präfix für die Common-Klasse zurück.
     * Wird für Logging-Zwecke verwendet.
     * @return Log-Präfix "Common".
     */
    std::string Common::logPrefix()
    {
        return "Common";
    }

    /*
     * Initialisiert die Common-Klasse und bereitet die Hardware sowie KNX-spezifische Komponenten vor.
     * @param firmwareRevision Die Revision der Firmware.
     */
    void Common::init(uint8_t firmwareRevision)
    {
        // Initialisiert die serielle Debug-Schnittstelle für KNX.
        ArduinoPlatform::SerialDebug = new OpenKNX::Log::VirtualSerial("KNX");

        // Initialisiert den Timer-Interrupt und LEDs.
        openknx.timerInterrupt.init();
        openknx.hardware.initLeds();

#if defined(PROG_BUTTON_PIN) && PROG_BUTTON_PIN >= 0 && OPENKNX_RECOVERY_TIME > 0
        processRecovery();
#endif

        // Initialisiert die Hardware-Tasten.
        openknx.hardware.initButtons();

#ifdef OPENKNX_NO_BOOT_PULSATING
        openknx.progLed.on();
    #ifdef INFO1_LED_PIN
        openknx.info1Led.on();
    #endif
#else
        openknx.progLed.pulsing();
    #ifdef INFO1_LED_PIN
        openknx.info1Led.pulsing();
    #endif
#endif

        debugWait();

        if (openknx.watchdog.lastReset()) logErrorP("Restarted by watchdog");

        logInfoP("Init firmware");

#ifdef OPENKNX_DEBUG
        showDebugInfo();
#endif

        // Initialisiert den Flash-Speicher und KNX-spezifische Informationen.
        openknx.hardware.initFlash();
        openknx.info.serialNumber(knx.platform().uniqueSerialNumber());
        openknx.info.firmwareRevision(firmwareRevision);

        initKnx();

        openknx.hardware.init();
    }

#ifdef OPENKNX_DEBUG
    /*
     * Zeigt Debugging-Informationen an, wenn Debugging aktiviert ist.
     */
    void Common::showDebugInfo()
    {
        logDebugP("Debug logging is enabled!");
    #if defined(OPENKNX_TRACE1) || defined(OPENKNX_TRACE2) || defined(OPENKNX_TRACE3) || defined(OPENKNX_TRACE4) || defined(OPENKNX_TRACE5)
        logDebugP("Trace logging is enabled with:");
        logIndentUp();
        #ifdef OPENKNX_TRACE1
        logDebugP("Filter 1: %s", TRACE_STRINGIFY(OPENKNX_TRACE1));
        #endif
        #ifdef OPENKNX_TRACE2
        logDebugP("Filter 2: %s", TRACE_STRINGIFY(OPENKNX_TRACE2));
        #endif
        #ifdef OPENKNX_TRACE3
        logDebugP("Filter 3: %s", TRACE_STRINGIFY(OPENKNX_TRACE3));
        #endif
        #ifdef OPENKNX_TRACE4
        logDebugP("Filter 4: %s", TRACE_STRINGIFY(OPENKNX_TRACE4));
        #endif
        #ifdef OPENKNX_TRACE5
        logDebugP("Filter 5: %s", TRACE_STRINGIFY(OPENKNX_TRACE5));
        #endif
        logIndentDown();
    #endif
    }
#endif

#if defined(PROG_BUTTON_PIN) && PROG_BUTTON_PIN >= 0 && OPENKNX_RECOVERY_TIME > 0
    /*
     * Verarbeitet ein Recovery-Ereignis, das durch den Programmier-Button ausgelöst wird.
     */
    void Common::processRecovery()
    {
        bool erase = false;
        pinMode(PROG_BUTTON_PIN, INPUT_PULLUP);
        while (!digitalRead(PROG_BUTTON_PIN))
        {
            if (millis() >= OPENKNX_RECOVERY_TIME)
            {
                if (!erase)
                {
                    openknx.progLed.blinking(200);
                    erase = true;
                }
            }
        }

        if (erase)
        {
            openknx.hardware.initFlash();
            openknx.knxFlash.erase();
            restart();
        }

        openknx.progLed.off();
    }
#endif

    /*
     * Initialisiert den KNX-Stack und konfiguriert spezifische Eigenschaften.
     */
    void Common::initKnx()
    {
        logInfoP("Init knx stack");
        logIndentUp();

#if (defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_ESP32)) && defined(KNX_UART_RX_PIN) && defined(KNX_UART_TX_PIN)
        knx.platform().knxUartPins(KNX_UART_RX_PIN, KNX_UART_TX_PIN);
#endif

        openknx.progButton.onShortClick([] { knx.toggleProgMode(); });

        knx.ledPin(0);
        knx.setProgLedOnCallback([] { openknx.progLed.forceOn(true); });
        knx.setProgLedOffCallback([] { openknx.progLed.forceOn(false); });

        uint8_t hardwareType[LEN_HARDWARE_TYPE] = {0x00, 0x00, MAIN_OpenKnxId, MAIN_ApplicationNumber, MAIN_ApplicationVersion, 0x00};

        knx.bau().versionCheckCallback(versionCheck);
        knx.bau().deviceObject().hardwareType(hardwareType);
        knx.readMemory();
        knx.bau().deviceObject().hardwareType(hardwareType);
        knx.bau().deviceObject().version(openknx.info.firmwareVersion());

#ifdef MAIN_OrderNumber
        knx.orderNumber((const uint8_t*)MAIN_OrderNumber);
#endif
        logIndentDown();
    }

    /*
     * Startet das System neu, nachdem vorbereitende Aktionen abgeschlossen sind.
     */
    void Common::restart()
    {
        logInfoP("System will restart now");
        delay(10);
        openknx.watchdog.safeRestart();
        knx.platform().restart();
    }

#ifdef OPENKNX_RUNTIME_STAT
    /*
     * Zeigt Laufzeitstatistiken an, wenn aktiviert.
     */
    void Common::showRuntimeStat(const bool stat /*= true*/, const bool hist /*= false*/)
    {
        logInfoP("Runtime Statistics: (Uptime=%dms)", millis());
        logIndentUp();
        {
            Stat::RuntimeStat::showStatHeader();
            _runtimeLoop.showStat("___Loop", 0, stat, hist);
            _runtimeConsole.showStat("__Console", 0, stat, hist);
            _runtimeKnxStack.showStat("__KnxStack", 0, stat, hist);
            _runtimeModuleLoop.showStat("_All_Modules_Loop", 0, stat, hist);
        }
        logIndentDown();
    }
#endif

} // namespace OpenKNX
