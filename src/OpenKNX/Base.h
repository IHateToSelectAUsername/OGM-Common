#pragma once

#include "OpenKNX/Log/Logger.h"
#include "OpenKNX/defines.h"
#include <knx.h>

namespace OpenKNX
{
    /*
     * Abstrakte Basisklasse für Module und Kanäle im OpenKNX-Framework.
     * Diese Klasse definiert grundlegende Schnittstellen und Methoden,
     * die von abgeleiteten Klassen implementiert werden.
     */
    class Base
    {
      protected:
        /*
         * Wrapper für das Logging. Der Name des Moduls wird als Präfix verwendet.
         * @param output Ausgabestring im printf-Format.
         */
        void log(const char *output, ...);

        /*
         * Wrapper für das Logging von Hexadezimaldaten. Der Modulname wird als Präfix verwendet.
         * @param data Zeiger auf die zu loggenden Daten.
         * @param size Größe der Daten in Bytes.
         */
        void logHex(const uint8_t *data, size_t size);

        /*
         * Gibt einen Zeiger auf das Log-Präfix zurück (z. B. Modulname).
         * Der Speicher muss nach der Nutzung freigegeben werden (delete[]).
         * @return Log-Präfix als Zeichenkette.
         */
        virtual const std::string logPrefix();

      public:
        /*
         * Gibt den Namen des Moduls zurück, der für Logging verwendet wird.
         * @return Name des Moduls.
         */
        virtual const std::string name() = 0;

        /*
         * Wird nach der Initialisierung des KNX-Stacks aufgerufen, aber vor der setup()-Methode.
         * Diese Methode wird unabhängig vom Zustand der KNX-Konfiguration ausgeführt.
         */
        virtual void init();

        /*
         * Setup-Methode, die nach der Initialisierung aller Module aufgerufen wird.
         * Diese Methode eignet sich z. B. für die Initialisierung von Interrupts auf Kern 0.
         * @param configured Gibt an, ob KNX konfiguriert ist.
         */
        virtual void setup(bool configured);

        /*
         * Setup-Methode ohne Parameter, wird bei Bedarf von setup(bool) aufgerufen.
         */
        virtual void setup();

        /*
         * Hauptlogik des Moduls, wird im Hauptzyklus (Loop) ausgeführt.
         * @param configured Gibt an, ob KNX konfiguriert ist.
         */
        virtual void loop(bool configured);

        /*
         * Hauptlogik ohne Parameter, wird von loop(bool) aufgerufen.
         */
        virtual void loop();

#ifdef OPENKNX_DUALCORE
        /*
         * Setup-Methode für den zweiten Kern (Kern 1).
         * Diese Methode eignet sich z. B. für die Initialisierung von Interrupts.
         * @param configured Gibt an, ob KNX konfiguriert ist.
         */
        virtual void setup1(bool configured);

        /*
         * Setup-Methode ohne Parameter für den zweiten Kern.
         */
        virtual void setup1();

        /*
         * Hauptlogik des Moduls für den zweiten Kern (Kern 1).
         * @param configured Gibt an, ob KNX konfiguriert ist.
         */
        virtual void loop1(bool configured);

        /*
         * Hauptlogik ohne Parameter für den zweiten Kern.
         */
        virtual void loop1();
#endif

#if (MASK_VERSION & 0x0900) != 0x0900 // Gilt nicht für Koppler ohne GroupObjects
        /*
         * Verarbeitet Eingaben oder Änderungen eines Gruppenobjekts (GroupObject).
         * @param ko Referenz auf das Gruppenobjekt.
         */
        virtual void processInputKo(GroupObject &ko);
#endif

        /*
         * Verarbeitet eingehende Befehle zu Funktionseigenschaften.
         * @param objectIndex Index des Objekts.
         * @param propertyId ID der Eigenschaft.
         * @param length Länge der Eigenschaftsdaten.
         * @param data Zeiger auf die Eingangsdaten.
         * @param resultData Zeiger auf die Ergebnisdaten.
         * @param resultLength Referenz auf die Länge der Ergebnisdaten.
         * @return true, wenn die Eigenschaft verarbeitet wurde; andernfalls false.
         */
        virtual bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength);

        /*
         * Verarbeitet Statusanfragen zu Funktionseigenschaften.
         * @param objectIndex Index des Objekts.
         * @param propertyId ID der Eigenschaft.
         * @param length Länge der Eigenschaftsdaten.
         * @param data Zeiger auf die Eingangsdaten.
         * @param resultData Zeiger auf die Ergebnisdaten.
         * @param resultLength Referenz auf die Länge der Ergebnisdaten.
         * @return true, wenn die Statusanfrage verarbeitet wurde; andernfalls false.
         */
        virtual bool processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
    };
} // namespace OpenKNX