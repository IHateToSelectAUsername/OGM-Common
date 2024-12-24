#pragma once

// Die zentrale Header-Datei für die OpenKNX-Fassade.
// Sie bietet eine Abstraktion, um verschiedene KNX-bezogene Funktionen einfach zu nutzen.

#include "Helper.h"
#include "OpenKNX/Common.h"       // Allgemeine Funktionen und Daten
#include "OpenKNX/Console.h"      // Serielle Konsole
#include "OpenKNX/Flash/Default.h" // Flash-Speicherverwaltung
#include "OpenKNX/Hardware.h"     // Hardware-spezifische Funktionen
#include "OpenKNX/Information.h"  // Informationen zum Systemstatus
#include "OpenKNX/Log/Logger.h"   // Logging-Funktionalitäten
#include "OpenKNX/Module.h"       // Modulverwaltung
#include "OpenKNX/Watchdog.h"     // Watchdog-Funktionalität
#ifdef OPENKNX_RUNTIME_STAT
    #include "OpenKNX/Stat/RuntimeStat.h" // Laufzeitstatistiken (optional)
#endif
#include "OpenKNX/TimerInterrupt.h" // Timer-Interrupt-Verwaltung
#include "OpenKNX/defines.h"       // Globale Definitionen

namespace OpenKNX
{
    // Datenstruktur zur Verwaltung von KNX-Modulen.
    struct Modules
    {
        uint8_t count = 0; // Anzahl der registrierten Module
        uint8_t ids[OPENKNX_MAX_MODULES]; // IDs der Module
        Module* list[OPENKNX_MAX_MODULES]; // Zeiger auf Modulinstanzen
#ifdef OPENKNX_RUNTIME_STAT
        Stat::RuntimeStat runtime[OPENKNX_MAX_MODULES]; // Laufzeitstatistiken
    #ifdef OPENKNX_DUALCORE
        Stat::RuntimeStat runtime1[OPENKNX_MAX_MODULES]; // Laufzeitstatistiken für zweiten Kern
    #endif
#endif
    };

    // Die Facade-Klasse dient als zentrale Schnittstelle für das OpenKNX-Framework.
    class Facade
    {
      public:
        Common common;                // Allgemeine Funktionen
        Flash::Default flash;         // Verwaltung des Flash-Speichers
        Information info;             // Systeminformationen
        Console console;              // Serielle Konsole
        Log::Logger logger;           // Logger-Komponente
        TimerInterrupt timerInterrupt; // Timer-Interrupt-Management
        Hardware hardware;            // Hardware-spezifische Funktionen
        Watchdog watchdog;            // Watchdog-Funktionalität

        // Programmierungs- und Funktions-LEDs
        Button progButton = Button("Prog"); // Standard-Programmiertaste
#ifdef FUNC1_BUTTON_PIN
        Button func1Button = Button("Func1"); // Zusätzliche Funktionstaste 1
#endif
#ifdef FUNC2_BUTTON_PIN
        Button func2Button = Button("Func2"); // Zusätzliche Funktionstaste 2
#endif
#ifdef FUNC3_BUTTON_PIN
        Button func3Button = Button("Func3"); // Zusätzliche Funktionstaste 3
#endif

#ifdef OPENKNX_SERIALLED_ENABLE
        Led::SerialLedManager ledManager; // Manager für serielle LEDs
    #define LEDCLASS Led::Serial
#else
    #define LEDCLASS Led::GPIO
#endif

        LEDCLASS progLed; // Programmier-LED
#ifdef INFO1_LED_PIN
        LEDCLASS info1Led; // Info-LED 1
        LEDCLASS& infoLed = info1Led; // Alias für Info-LED
#endif
#ifdef INFO2_LED_PIN
        LEDCLASS info2Led; // Info-LED 2
#endif
#ifdef INFO3_LED_PIN
        LEDCLASS info3Led; // Info-LED 3
#endif

        Modules modules; // Struktur zur Verwaltung der Module
        Flash::Driver openknxFlash; // Flash-Speicher für OpenKNX-Daten
        Flash::Driver knxFlash;    // Flash-Speicher für KNX-Daten

        // Initialisiert die Fassade mit einer bestimmten Firmware-Version.
        void init(uint8_t firmwareRevision);

        // Hauptschleife zur Verarbeitung von Aufgaben und Ereignissen.
        void loop();

        // Setup-Funktion zur Initialisierung der KNX-Funktionalitäten.
        void setup();

        // Gibt an, ob Dual-Core-Unterstützung aktiv ist.
        bool usesDualCore();

#ifdef OPENKNX_DUALCORE
        // Methoden für den zweiten Kern (bei Dual-Core-Unterstützung).
        void loop1();
        void setup1();
#endif

        // Fügt ein Modul mit einer bestimmten ID zur Fassade hinzu.
        void addModule(uint8_t id, Module& module);

        // Gibt ein Modul anhand seiner ID zurück.
        Module* getModule(uint8_t id);

        // Gibt einen Zeiger auf die Modulstruktur zurück.
        Modules* getModules();

        // Prüft, ob die Startverzögerung abgeschlossen ist.
        bool afterStartupDelay();

        // Gibt an, ob freie Zeit in der Hauptschleife verfügbar ist.
        bool freeLoopTime();

        // Iteriert durch freie Zeit in der Hauptschleife (für Aufgaben).
        bool freeLoopIterate(uint8_t size, uint8_t& position, uint8_t& processed);

        // Startet das System neu.
        void restart();
    };
} // namespace OpenKNX

// Globale Instanz der Fassade, die im gesamten Projekt verwendet wird.
extern OpenKNX::Facade openknx;
