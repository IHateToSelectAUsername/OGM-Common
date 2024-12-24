#pragma once

#include "OpenKNX/Log/Logger.h"
#include "OpenKNX/Log/VirtualSerial.h"
#ifdef OPENKNX_RUNTIME_STAT
    #include "OpenKNX/Stat/RuntimeStat.h"
#endif
#include "OpenKNX/defines.h"
#include "knx.h"

namespace OpenKNX
{
    /*
     * Die Common-Klasse bietet zentrale Funktionen und Logik für das OpenKNX-Framework.
     * Sie verwaltet die Initialisierung, den Hauptzyklus, Module und Hardwareereignisse.
     */
    class Common
    {
      private:
#if OPENKNX_LOOPTIME_WARNING > 1
        uint32_t _lastLooptimeWarning = 0; // Zeitstempel der letzten Loop-Warnung.
        bool _skipLooptimeWarning = false; // Gibt an, ob Loop-Warnungen übersprungen werden sollen.
#endif
        uint8_t _currentModule = 0; // Index des aktuell ausgeführten Moduls.
        uint32_t _loopMicros = 0; // Zeitstempel für den Beginn des aktuellen Loops.
        volatile bool _setup0Ready = false; // Gibt an, ob das Haupt-Setup abgeschlossen ist.
#ifdef OPENKNX_DUALCORE
        volatile bool _setup1Ready = false; // Gibt an, ob das Setup für den zweiten Kern abgeschlossen ist.
#endif

        uint32_t _savedPinProcessed = 0; // Zeitstempel für die letzte Bearbeitung des Save-Pins.
        bool _savePinTriggered = false; // Gibt an, ob der Save-Pin ausgelöst wurde.
        volatile int32_t _freeMemoryMin = 0x7FFFFFFF; // Minimaler freier Speicher während der Laufzeit.
#ifdef ARDUINO_ARCH_ESP32
        volatile int32_t _freeStackMin = 0; // Minimaler freier Stapelspeicher (ESP32).
    #ifdef OPENKNX_DUALCORE
        volatile int32_t _freeStackMin1 = 0; // Minimaler freier Stapelspeicher für den zweiten Kern (ESP32).
    #endif
#endif
#ifdef ARDUINO_ARCH_RP2040
        volatile int32_t _freeStackMin = 0x2000; // Minimaler freier Stapelspeicher (RP2040).
    #ifdef OPENKNX_DUALCORE
        volatile int32_t _freeStackMin1 = 0x2000; // Minimaler freier Stapelspeicher für den zweiten Kern (RP2040).
    #endif
#endif

        /*
         * Initialisiert den KNX-Stack.
         */
        void initKnx();

        /*
         * Verarbeitet den Hauptzyklus der Module.
         */
        void processModulesLoop();

        /*
         * Registriert Callbacks für KNX-Ereignisse und Funktionseigenschaften.
         */
        void registerCallbacks();

        /*
         * Verarbeitet die Wiederherstellung nach einem Save-Pin-Ereignis.
         */
        void processRestoreSavePin();

        /*
         * Initialisiert den Timer für die Speicherüberwachung.
         */
        void initMemoryTimerInterrupt();

        /*
         * Wartet auf Debugging-Eingaben, wenn aktiviert.
         */
        void debugWait();

#ifdef OPENKNX_DEBUG
        /*
         * Zeigt Debugging-Informationen an.
         */
        void showDebugInfo();
#endif

#if defined(PROG_BUTTON_PIN) && PROG_BUTTON_PIN >= 0 && OPENKNX_RECOVERY_TIME > 0
        /*
         * Verarbeitet das Recovery-Ereignis beim Drücken des Programmier-Buttons.
         */
        void processRecovery();
#endif

#ifdef BASE_HeartbeatDelayBase
        uint32_t _heartbeatDelay = 0; // Verzögerung für den Heartbeat.
        /*
         * Verarbeitet das Heartbeat-Ereignis.
         */
        void processHeartbeat();
#endif
#ifdef BASE_PeriodicSave
        /*
         * Verarbeitet das periodische Speichern von Daten.
         */
        void processPeriodicSave();
#endif

        /*
         * Verarbeitet Befehle zu Funktionseigenschaften.
         */
        bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t* data, uint8_t* resultData, uint8_t& resultLength);

        /*
         * Verarbeitet Statusanfragen zu Funktionseigenschaften.
         */
        bool processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t* data, uint8_t* resultData, uint8_t& resultLength);

#ifdef OPENKNX_RUNTIME_STAT
        Stat::RuntimeStat _runtimeLoop; // Statistiken für den Hauptzyklus.
        Stat::RuntimeStat _runtimeConsole; // Statistiken für die Konsole.
        Stat::RuntimeStat _runtimeKnxStack; // Statistiken für den KNX-Stack.
        Stat::RuntimeStat _runtimeModuleLoop; // Statistiken für die Module.
#endif

#ifdef BASE_StartupDelayBase
        uint32_t _startupDelay = 0; // Verzögerung für den Startup.
        bool _firstStartup = true; // Gibt an, ob es sich um den ersten Startup handelt.
#endif
        bool _afterStartupDelay = false; // Gibt an, ob die Startup-Verzögerung abgeschlossen ist.

#ifdef BASE_KoManualSave
        /*
         * Verarbeitet manuelle Speicherbefehle.
         */
        void processSaveKo(GroupObject& ko);
#endif

      public:
        uint8_t extendedHeartbeatValue = 1; // Erweiterter Heartbeat-Wert.

        /*
         * Überprüft die Version der KNX-Hardware.
         * @param manufacturerId Hersteller-ID.
         * @param hardwareType Typ der Hardware.
         * @param firmwareVersion Firmware-Version.
         * @return Ergebnis der Versionsüberprüfung.
         */
        static VersionCheckResult versionCheck(uint16_t manufacturerId, uint8_t* hardwareType, uint16_t firmwareVersion);

        /*
         * Initialisiert die Common-Klasse.
         * @param firmwareRevision Firmware-Revision.
         */
        void init(uint8_t firmwareRevision);

        /*
         * Löst ein Save-Pin-Ereignis aus.
         */
        void triggerSavePin();

        /*
         * Setup-Methode für den Hauptkern.
         */
        void setup();

        /*
         * Hauptzyklus der Anwendung.
         */
        void loop();

#ifdef OPENKNX_DUALCORE
        /*
         * Setup-Methode für den zweiten Kern.
         */
        void setup1();

        /*
         * Hauptzyklus für den zweiten Kern.
         */
        void loop1();
#endif

        /*
         * Gibt an, ob die Startup-Verzögerung abgeschlossen ist.
         * @return true, wenn die Verzögerung abgeschlossen ist.
         */
        bool afterStartupDelay();

        /*
         * Verarbeitet die Startup-Verzögerung.
         */
        void processAfterStartupDelay();

        /*
         * Überspringt Loop-Warnungen.
         */
        void skipLooptimeWarning();

        /*
         * Startet das System neu.
         */
        void restart();

        /*
         * Sammelt Speicherstatistiken.
         */
        void collectHeapStats();

        /*
         * Sammelt Stapelstatistiken.
         */
        void collectStackStats();

        /*
         * Gibt den minimalen freien Speicher während der Laufzeit zurück.
         */
        uint freeMemoryMin();

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_ESP32)
        /*
         * Gibt den minimalen freien Stapelspeicher zurück.
         */
        int freeStackMin();
    #ifdef OPENKNX_DUALCORE
        /*
         * Gibt den minimalen freien Stapelspeicher des zweiten Kerns zurück.
         */
        int freeStackMin1();
    #endif
#endif

        /*
         * Gibt an, ob im Hauptzyklus noch freie Zeit vorhanden ist.
         * @return true, wenn freie Zeit vorhanden ist.
         */
        bool freeLoopTime();

        /*
         * Iteriert durch Module im Hauptzyklus, solange freie Zeit vorhanden ist.
         */
        bool freeLoopIterate(uint8_t size, uint8_t& position, uint8_t& processed);

        /*
         * Verarbeitet ein Save-Pin-Ereignis.
         */
        void processSavePin();

        /*
         * Verarbeitet Ereignisse vor einem Neustart.
         */
        void processBeforeRestart();

        /*
         * Verarbeitet Ereignisse vor dem Entladen von Tabellen.
         */
        void processBeforeTablesUnload();

#if (MASK_VERSION & 0x0900) != 0x0900 // Gilt nicht für Koppler ohne GroupObjects
        /*
         * Verarbeitet Eingaben von Gruppenobjekten.
         * @param ko Referenz auf das Gruppenobjekt.
         */
        void processInputKo(GroupObject& ko);
#endif

        /*
         * Gibt das Log-Präfix der Klasse zurück.
         * @return Log-Präfix als Zeichenkette.
         */
        std::string logPrefix();

#ifdef OPENKNX_RUNTIME_STAT
        /*
         * Zeigt Laufzeitstatistiken an.
         */
        void showRuntimeStat(const bool stat = true, const bool hist = false);
#endif
    };
} // namespace OpenKNX