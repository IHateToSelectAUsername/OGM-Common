#pragma once

// Enthält die Definition der Hardware-Klasse für die OpenKNX-Plattform.
// Diese Klasse kümmert sich um die Initialisierung und Verwaltung der Hardwarekomponenten,
// wie LEDs, Tasten, Flash-Speicher und andere Module.

#include "OpenKNX/Button.h"
#ifdef OPENKNX_SERIALLED_ENABLE
    #include "OpenKNX/Led/Serial.h" // Serielle LEDs
#else
    #include "OpenKNX/Led/GPIO.h"   // GPIO-basierte LEDs
#endif
#include <Arduino.h>

#ifdef ARDUINO_ARCH_RP2040
    #include <hardware.h>
    #include <hardware/adc.h>
    #include <pico/stdlib.h>
#endif

// Definition von Fehlercodes für schwerwiegende Fehler
#define FATAL_FLASH_PARAMETERS 2        // Fehlerhafte Flash-Parameter
#define FATAL_I2C_BUSY 3                // I2C-Bus während des Starts belegt
#define FATAL_LOG_WRONG_CHANNEL_COUNT 4 // Zu viele Kanäle in der KNX-Konfiguration
#define FATAL_SENS_UNKNOWN 5            // Unbekannter oder nicht unterstützter Sensor
#define FATAL_SCHEDULE_MAX_CALLBACKS 6  // Zu viele Callback-Funktionen im Scheduler
#define FATAL_NETWORK 7                 // Netzwerkfehler
#define FATAL_INIT_FILESYSTEM 10        // Fehler bei der Initialisierung des Dateisystems
#define FATAL_SYSTEM 20                 // Systemfehler (z. B. Pufferüberlauf)

// Flags für Hardware-Funktionen des Boards
#define BOARD_HW_EEPROM 0x01
#define BOARD_HW_LED 0x02
#define BOARD_HW_ONEWIRE 0x04
#define BOARD_HW_NCN5130 0x08

namespace OpenKNX
{
    // Die Hardware-Klasse stellt Methoden für die Hardware-Initialisierung und Fehlerbehandlung bereit.
    class Hardware
    {
      private:
        uint8_t features = 0; // Speichert verfügbare Hardware-Funktionen

      public:
        // Initialisiert die Hardware-Komponenten.
        void init();

        // Behandelt schwerwiegende Fehler durch Anzeigen von Fehlercodes und Meldungen.
        void fatalError(uint8_t code, const char* message = 0);

        // Gibt die Temperatur der CPU zurück (falls verfügbar).
        float cpuTemperature();

#ifdef ARDUINO_ARCH_RP2040
        // Initialisiert das Dateisystem (nur für RP2040).
        void initFilesystem();
#endif
        // Initialisiert den Flash-Speicher.
        void initFlash();

        // Initialisiert LEDs, je nach Plattform und Konfiguration.
        void initLeds();

        // Initialisiert die Tasten und konfiguriert Interrupts für Statusänderungen.
        void initButtons();

        // Initialisiert Interrupt-Service-Routinen für KNX-Datenempfang.
        void initKnxRxISR();
    };
} // namespace OpenKNX
