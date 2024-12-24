#pragma once

#include <Arduino.h>
#include <cstdint>
#include <stdarg.h>
#include <stdio.h>
#include <string>

// Makros zur Überprüfung von Zeitintervallen in Millisekunden oder Mikrosekunden
#define delayCheckMillis(last, duration) (millis() - last >= duration)
#define delayCheckMicros(last, duration) (micros() - last >= duration)
#define delayCheck(last, duration) delayCheckMillis(last, duration)

// Initialisiert einen Timer, der mindestens 1 ms lang ist
#define delayTimerInit() (max(millis(), 1UL))

// Definiert einen speziellen Wert für ungültige Zahlen
#define NO_NUM -987654321.0F 

// Überprüft, ob ein Wert eine gültige Zahl ist
#define isNum(value) ((value + 10.0) > NO_NUM)

/*
 * Systemlaufzeit (Uptime):
 * Diese Funktion liefert die Systemlaufzeit in Sekunden.
 * Sie muss regelmäßig aufgerufen werden, um Überläufe der Millisekunden zu behandeln.
 */
uint32_t uptime(bool result = true);

/*
 * Freier Speicher:
 * Diese Funktion berechnet den verfügbaren RAM des Systems.
 */
int freeMemory();

#ifdef ARDUINO_ARCH_RP2040
    #include "hardware/flash.h"
    #include "hardware/sync.h"
    #include "pico/sync.h"

/*
 * Löscht einen Bereich im Flash-Speicher.
 * Offset und Größe müssen Vielfache von 4096 sein.
 */
bool __no_inline_not_in_flash_func(__nukeFlash)(uint32_t offset, size_t count);

#ifdef SERIAL_DEBUG
/*
 * Gibt die verbleibende Stapelgröße (Stack Size) für Debugging-Zwecke aus.
 */
void printFreeStackSize();
#endif
#endif