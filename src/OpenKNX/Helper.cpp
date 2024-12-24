#include "Helper.h"
#include "OpenKNX/Facade.h"

#ifndef ARDUINO_ARCH_ESP32

    /*
     * Freier Speicher:
     * Diese Funktion wird auf Nicht-ESP32-Plattformen verwendet, um den verbleibenden RAM zu berechnen.
     */
    #ifdef __arm__
    // Definiert den Bruchpunkt (Heap-Start) für ARM-Architekturen.
    extern "C" char *sbrk(int incr);
    #else  // __ARM__
    // Definiert den Bruchpunkt für andere Architekturen.
    extern char *__brkval;
    #endif // __arm__

#endif // !ARDUINO_ARCH_ESP32

/*
 * Berechnet den freien Speicherplatz basierend auf der Plattform.
 */
int freeMemory()
{
#if defined(ARDUINO_ARCH_ESP32)
    // Verwendet die ESP32-spezifische Funktion, um den freien Heap-Speicher zu erhalten.
    return ESP.getFreeHeap();
#elif defined(ARDUINO_ARCH_RP2040)
    // Verwendet die RP2040-spezifische Funktion, um den freien Heap-Speicher zu berechnen.
    return rp2040.getFreeHeap();
#else
    char top;
    #ifdef __arm__
    // Berechnet den freien Speicher auf ARM-Architekturen durch Vergleich mit dem Bruchpunkt.
    return &top - reinterpret_cast<char *>(sbrk(0));
    #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    // Berechnet den freien Speicher auf Teensy-Plattformen oder anderen Arduino-Plattformen.
    return &top - __brkval;
    #else  // __arm__
    // Berechnet den freien Speicher, falls kein Bruchpunkt definiert ist.
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
    #endif // __arm__
#endif     // !ARDUINO_ARCH_ESP32
}

/*
 * Berechnet die Systemlaufzeit in Sekunden.
 * Diese Funktion behandelt Millisekunden-Überläufe durch Zählen der Rollovers.
 */
uint32_t uptime(bool result)
{
    static uint16_t uptimeRolloverCount = 0; // Zählt die Anzahl der Millisekunden-Überläufe.
    static uint32_t uptimeLastMillis = 0;   // Speichert den letzten Millisekundenwert.

    const uint32_t uptimeCurrentMillis = millis();
    if (uptimeCurrentMillis < uptimeLastMillis)
        uptimeRolloverCount++; // Erhöht den Zähler bei einem Überlauf.
    uptimeLastMillis = uptimeCurrentMillis;

    if (!result)
        return 0; // Gibt 0 zurück, wenn die Berechnung nicht gewünscht ist.
    return ((uint64_t)uptimeRolloverCount << 32 | uptimeCurrentMillis) / 1000UL; // Berechnet die Laufzeit in Sekunden.
}

#ifdef ARDUINO_ARCH_RP2040

/*
 * Löscht einen Bereich im Flash-Speicher.
 * Offset und Größe müssen Vielfache von 4096 sein, da Flash in Seiten organisiert ist.
 */
bool __no_inline_not_in_flash_func(__nukeFlash)(uint32_t offset, size_t size)
{
    if (offset % 4096 > 0 || size % 4096 > 0)
    {
        return false; // Abbruch, wenn die Anforderungen nicht erfüllt sind.
    }
    else
    {
        rp2040.idleOtherCore(); // Stoppt den anderen Kern, um Konflikte zu vermeiden.
        noInterrupts();         // Deaktiviert Interrupts während des Löschvorgangs.
        flash_range_erase(offset, size); // Löscht den Flash-Bereich.
        interrupts();          // Reaktiviert Interrupts.
        rp2040.resumeOtherCore(); // Startet den anderen Kern erneut.
        return true; // Erfolgsmeldung.
    }
}

#ifdef SERIAL_DEBUG
/*
 * Gibt die verbleibende Stapelgröße (Stack Size) für Debugging aus.
 * Unterstützt Dual-Core-Systeme, indem der aktive Kern angegeben wird.
 */
void printFreeStackSize()
{
    #ifdef OPENKNX_DUALCORE
    // Zeigt an, welcher Kern aktiv ist (0 oder 1).
    SERIAL_DEBUG.print(rp2040.cpuid() ? "_1> " : "0_> ");
    #endif
    // Gibt die freie Stapelgröße in Bytes aus.
    SERIAL_DEBUG.printf("Free stack size: %i bytes\r\n", rp2040.getFreeStack());
}
#endif
#endif
