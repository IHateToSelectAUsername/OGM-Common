#pragma once

// Verhindert, dass die Datei mehrmals innerhalb des Projekts eingebunden wird.
#ifndef SMALL_GROUPOBJECT
#error OGM-Common needs build-flag "-D SMALL_GROUPOBJECT"
#endif

// Prüft, ob die Architektur unterstützt wird.
// Unterstützt werden: SAMD (z. B. SAMD21), RP2040 (Raspberry Pi Pico), ESP32.
#if !defined(ARDUINO_ARCH_SAMD) && !defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_ARCH_ESP32)
#error Your architecture is not supported by OpenKNX
#endif

// Für alle Architekturen außer ESP32 müssen spezifische Flash-Parameter definiert sein.
#ifndef ARDUINO_ARCH_ESP32
    #ifndef OPENKNX_FLASH_OFFSET
        #error "OPENKNX_FLASH_OFFSET is not defined"
    #endif

    #ifndef OPENKNX_FLASH_SIZE
        #error "OPENKNX_FLASH_SIZE is not defined"
    #endif

    #ifndef KNX_FLASH_OFFSET
        #error "KNX_FLASH_OFFSET is not defined"
    #endif

    #ifndef KNX_FLASH_SIZE
        #error "KNX_FLASH_SIZE is not defined"
    #endif
#endif

// Zusätzliche Anforderungen für RP2040-Architekturen:
// Die Flash-Größe muss bestimmte Anforderungen erfüllen, um korrekt genutzt werden zu können.
#ifdef ARDUINO_ARCH_RP2040
    #if OPENKNX_FLASH_SIZE % 2
        #error "OPENKNX_FLASH_SIZE cannot be divided by 2"
    #endif

    #if OPENKNX_FLASH_SIZE % 4096
        #error "OPENKNX_FLASH_SIZE must be multiple of 4096"
    #endif
#endif

// Bindet die wichtigsten Module des OpenKNX-Frameworks ein:
#include "OpenKNX/Channel.h"  // Verwaltung der KNX-Kommunikationskanäle
#include "OpenKNX/Common.h"   // Allgemeine Definitionen und Hilfsfunktionen
#include "OpenKNX/Facade.h"   // Schnittstelle zur einfachen Nutzung der KNX-Funktionalität
#include "OpenKNX/Hardware.h" // Hardware-spezifische Funktionen und Definitionen
#include "OpenKNX/Helper.h"   // Sammlung von Hilfsfunktionen für verschiedene Aufgaben
#include "OpenKNX/Module.h"   // Definition und Verwaltung von KNX-Modulen
