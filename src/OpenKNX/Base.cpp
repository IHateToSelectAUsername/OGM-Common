#include "OpenKNX/Base.h"
#include "OpenKNX/Facade.h"

namespace OpenKNX
{
    /*
     * Gibt das Log-Präfix zurück, welches standardmäßig der Modulname ist.
     * Diese Methode kann von abgeleiteten Klassen überschrieben werden.
     * @return Der Modulname als Log-Präfix.
     */
    const std::string Base::logPrefix()
    {
        return name();
    }

    /*
     * Standard-Initialisierungsmethode.
     * Diese Methode hat keine Funktionalität und wird von abgeleiteten Klassen überschrieben.
     */
    void Base::init() {}

    /*
     * Setup-Methode mit Konfigurationsstatus.
     * Wenn das Modul konfiguriert ist, wird die Setup-Methode ohne Parameter aufgerufen.
     * @param configured Gibt an, ob das Modul konfiguriert ist.
     */
    void Base::setup(bool configured)
    {
        if (configured) setup();
    }

    /*
     * Standard-Setup-Methode ohne Parameter.
     * Diese Methode hat keine Funktionalität und wird von abgeleiteten Klassen überschrieben.
     */
    void Base::setup() {}

    /*
     * Hauptlogik des Moduls mit Konfigurationsstatus.
     * Wenn das Modul konfiguriert ist, wird die Loop-Methode ohne Parameter aufgerufen.
     * @param configured Gibt an, ob das Modul konfiguriert ist.
     */
    void Base::loop(bool configured)
    {
        if (configured) loop();
    }

    /*
     * Standard-Hauptlogik ohne Parameter.
     * Diese Methode hat keine Funktionalität und wird von abgeleiteten Klassen überschrieben.
     */
    void Base::loop() {}

#ifdef OPENKNX_DUALCORE
    /*
     * Setup-Methode für den zweiten Kern mit Konfigurationsstatus.
     * Wenn das Modul konfiguriert ist, wird die Setup1-Methode ohne Parameter aufgerufen.
     * @param configured Gibt an, ob das Modul konfiguriert ist.
     */
    void Base::setup1(bool configured)
    {
        if (configured) setup1();
    }

    /*
     * Standard-Setup-Methode für den zweiten Kern.
     * Diese Methode hat keine Funktionalität und wird von abgeleiteten Klassen überschrieben.
     */
    void Base::setup1() {}

    /*
     * Hauptlogik des Moduls für den zweiten Kern mit Konfigurationsstatus.
     * Wenn das Modul konfiguriert ist, wird die Loop1-Methode ohne Parameter aufgerufen.
     * @param configured Gibt an, ob das Modul konfiguriert ist.
     */
    void Base::loop1(bool configured)
    {
        if (configured) loop1();
    }

    /*
     * Standard-Hauptlogik für den zweiten Kern.
     * Diese Methode hat keine Funktionalität und wird von abgeleiteten Klassen überschrieben.
     */
    void Base::loop1() {}

#endif

#if (MASK_VERSION & 0x0900) != 0x0900 // Gilt nicht für Koppler ohne GroupObjects
    /*
     * Verarbeitet Eingaben oder Änderungen eines Gruppenobjekts.
     * Diese Methode hat keine Funktionalität und wird von abgeleiteten Klassen überschrieben.
     * @param ko Referenz auf das Gruppenobjekt.
     */
    void Base::processInputKo(GroupObject &ko)
    {
    }
#endif

    /*
     * Verarbeitet eingehende Befehle zu Funktionseigenschaften.
     * Diese Standardimplementierung gibt immer "false" zurück.
     * @param objectIndex Index des Objekts.
     * @param propertyId ID der Eigenschaft.
     * @param length Länge der Eigenschaftsdaten.
     * @param data Zeiger auf die Eingangsdaten.
     * @param resultData Zeiger auf die Ergebnisdaten.
     * @param resultLength Referenz auf die Länge der Ergebnisdaten.
     * @return Immer "false".
     */
    bool Base::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
    {
        return false;
    }

    /*
     * Verarbeitet Statusanfragen zu Funktionseigenschaften.
     * Diese Standardimplementierung gibt immer "false" zurück.
     * @param objectIndex Index des Objekts.
     * @param propertyId ID der Eigenschaft.
     * @param length Länge der Eigenschaftsdaten.
     * @param data Zeiger auf die Eingangsdaten.
     * @param resultData Zeiger auf die Ergebnisdaten.
     * @param resultLength Referenz auf die Länge der Ergebnisdaten.
     * @return Immer "false".
     */
    bool Base::processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
    {
        return false;
    }
} // namespace OpenKNX