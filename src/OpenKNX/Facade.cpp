#include "OpenKNX/Facade.h"
#include "ModuleVersionCheck.h"

// Definiert eine optionale Funktion "setup1" für den zweiten Kern,
// die schwach eingebunden ist und nur genutzt wird, wenn sie woanders implementiert wurde.
extern void setup1() __attribute__((weak));

namespace OpenKNX
{

    // Initialisiert die Fassade mit der angegebenen Firmware-Revision.
    // Delegiert die Initialisierung an das "common"-Objekt, das grundlegende Funktionen bereitstellt.
    void Facade::init(uint8_t firmwareRevision)
    {
        common.init(firmwareRevision);
    }

    // Führt Setup-Operationen aus, indem es die entsprechende Methode im "common"-Objekt aufruft.
    void Facade::setup()
    {
        common.setup();
    }

    // Wird in der Hauptschleife aufgerufen, um zyklische Aufgaben zu erledigen.
    // Diese Methode delegiert die Aufgaben ebenfalls an "common".
    void Facade::loop()
    {
        common.loop();
    }

    // Startet das System neu, indem die entsprechende Methode im "common"-Objekt aufgerufen wird.
    void Facade::restart()
    {
        common.restart();
    }

    // Gibt zurück, ob das System mit Dual-Core-Unterstützung arbeitet.
    // Prüft, ob die Funktion "setup1" definiert wurde.
    bool Facade::usesDualCore()
    {
#ifdef OPENKNX_DUALCORE
        if (::setup1)
            return true;
#endif
        return false;
    }

#ifdef OPENKNX_DUALCORE
    // Zusätzliche Schleife für den zweiten Kern bei Dual-Core-Unterstützung.
    void Facade::loop1()
    {
        common.loop1();
    }

    // Zusätzliche Setup-Methode für den zweiten Kern.
    void Facade::setup1()
    {
        common.setup1();
    }
#endif

    // Prüft, ob die Startverzögerung abgeschlossen ist.
    bool Facade::afterStartupDelay()
    {
        return common.afterStartupDelay();
    }

    // Gibt an, ob freie Zeit in der Hauptschleife verfügbar ist.
    bool Facade::freeLoopTime()
    {
        return common.freeLoopTime();
    }

    // Iteriert durch freie Zeit in der Hauptschleife und bearbeitet Aufgaben stückweise.
    bool Facade::freeLoopIterate(uint8_t size, uint8_t &position, uint8_t &processed)
    {
        return common.freeLoopIterate(size, position, processed);
    }

    // Fügt ein neues Modul mit der angegebenen ID zur Liste der Module hinzu.
    void Facade::addModule(uint8_t id, Module &module)
    {
        modules.count++;
        modules.list[modules.count - 1] = &module;
        modules.ids[modules.count - 1] = id;
#ifdef OPENKNX_RUNTIME_STAT
        modules.runtime[modules.count - 1] = Stat::RuntimeStat();
#endif
    }

    // Gibt die Liste aller registrierten Module zurück.
    Modules *Facade::getModules()
    {
        return &modules;
    }

    // Sucht ein Modul anhand seiner ID und gibt es zurück (oder nullptr, wenn es nicht existiert).
    Module *Facade::getModule(uint8_t id)
    {
        for (uint8_t i = 0; i < modules.count; i++)
        {
            if (modules.ids[i] == id)
                return modules.list[i];
        }

        return nullptr;
    }
} // namespace OpenKNX

// Definiert die globale Instanz der Fassade, die im gesamten Projekt verwendet wird.
OpenKNX::Facade openknx;
