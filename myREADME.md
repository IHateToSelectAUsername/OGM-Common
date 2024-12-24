# OpenKNX Framework: Aufbau, Struktur und Funktionsweise

Das OpenKNX-Framework ist ein vielseitiges System, das die Entwicklung und Integration von KNX-Anwendungen auf Mikrocontrollern wie ESP32, RP2040 und SAMD vereinfacht. Es bietet eine modulare Architektur, um verschiedene Komponenten und Funktionen zu verwalten, von der Hardware-Initialisierung bis zur KNX-Kommunikation.

## **1. Architektur des Frameworks**

Das OpenKNX-Framework basiert auf einer klar strukturierten Architektur, die aus folgenden Hauptkomponenten besteht:

### **1.1 Fassade (Facade)**
Die `Facade`-Klasse bildet das zentrale Bindeglied zwischen den verschiedenen Subsystemen. Sie kapselt Funktionen zur Initialisierung, Modulverwaltung und Hardwaresteuerung. Über die globale Instanz `openknx` wird die Fassade in allen Teilen des Frameworks verwendet.

### **1.2 Basisklassen**
Die Basisklassen stellen allgemeine Schnittstellen für die Erweiterung durch spezifische Module bereit:
- **`Base`**: Abstrakte Klasse, die grundlegende Methoden wie `setup()`, `loop()` und `processInputKo()` definiert.
- **`Module`**: Verwaltet funktionale Erweiterungen, die als Module hinzugefügt werden können.
- **`Channel`**: Stellt eine spezifische Implementierung eines Kanals bereit, um KNX-Daten oder andere Konfigurationen zu verwalten.

### **1.3 Common-Komponente**
Die `Common`-Klasse übernimmt zentrale Aufgaben wie:
- Initialisierung von Hardware und KNX-Stack.
- Ereignisverarbeitung (z. B. Heartbeat, Save-Pin).
- Hauptschleifensteuerung (`loop()`) und Speichermanagement.

### **1.4 Hardware-Abstraktion**
Die `Hardware`-Klasse kapselt plattformspezifische Details wie:
- LEDs und Tasten.
- Flash-Speicher.
- Prozessor-spezifische Funktionen (z. B. CPU-Temperaturmessung).

### **1.5 Logging und Debugging**
Das Framework bietet eine flexible Logging-Infrastruktur mit Klassen wie `Logger` und `VirtualSerial`. Debugging-Informationen können dynamisch aktiviert werden.

### **1.6 Watchdog und Sicherheitsfunktionen**
Die `Watchdog`-Klasse stellt Mechanismen bereit, um das System bei Fehlfunktionen sicher neu zu starten.

---

## **2. Funktionsweise des Frameworks**

### **2.1 Initialisierung**
Die Initialisierung erfolgt durch die Methode `openknx.init()`. Diese führt folgende Schritte aus:
1. **Hardware-Setup**: LEDs, Tasten und Flash-Speicher werden konfiguriert.
2. **KNX-Stack**: Der KNX-Stack wird über `initKnx()` vorbereitet, und spezifische Eigenschaften wie Programmiermodus-Callbacks werden registriert.
3. **Debugging**: Optional wird die Debug-Schnittstelle initialisiert.

### **2.2 Hauptschleife**
Die Methode `openknx.loop()` wird kontinuierlich aufgerufen und umfasst:
- Verarbeitung von KNX-Nachrichten.
- Aufruf der Modul-Logik (`processModulesLoop()`).
- Behandlung von Ereignissen (z. B. Save-Pin, Heartbeat).

### **2.3 Module und Kanäle**
Module erweitern die Funktionalität des Frameworks. Sie werden mit `openknx.addModule()` registriert und können:
- Eigene Setup- und Loop-Logik implementieren.
- Gruppenobjekte (`GroupObject`) verarbeiten.

### **2.4 Dual-Core-Unterstützung**
Auf Plattformen wie ESP32 wird der zweite Kern durch die Methoden `setup1()` und `loop1()` unterstützt. Diese Methoden ermöglichen die Parallelverarbeitung von Aufgaben.

---

## **3. Struktur der Codebasis**

### **3.1 Hauptdateien und ihre Rollen**
- **`Facade.h/.cpp`**: Zentrale Schnittstelle des Frameworks.
- **`Common.h/.cpp`**: Enthält die Hauptlogik und Ereignisverarbeitung.
- **`Hardware.h/.cpp`**: Abstrahiert hardware-spezifische Funktionen.
- **`Base.h/.cpp`**: Definiert grundlegende Schnittstellen für Module.
- **`Module.h/.cpp`**: Implementiert die Verwaltung und Ausführung von Modulen.
- **`Channel.h/.cpp`**: Handhabt spezifische KNX-Kanäle.

### **3.2 Makros und Konfigurationsdateien**
Die Datei `defines.h` enthält wichtige Konfigurationsmakros wie:
- `OPENKNX_DUALCORE` für Dual-Core-Unterstützung.
- `KNX_UART_RX_PIN` und `KNX_UART_TX_PIN` für KNX-spezifische UART-Konfigurationen.

### **3.3 Debugging und Logging**
- **`Logger`**: Flexibles Logging-System.
- **`VirtualSerial`**: Simuliert serielle Ausgaben für Debugging-Zwecke.

---

## **4. Besonderheiten des Frameworks**

### **4.1 Modularität**
Das Framework ist hochgradig modular aufgebaut, was die Erweiterung durch benutzerdefinierte Module und Kanäle erleichtert. Die Basisklassen bieten dafür eine einheitliche Schnittstelle.

### **4.2 Plattformunabhängigkeit**
Dank der Hardware-Abstraktionsschicht unterstützt das Framework mehrere Mikrocontroller-Architekturen, darunter ESP32, RP2040 und SAMD.

### **4.3 KNX-Integration**
Das Framework ist eng in den KNX-Standard integriert und unterstützt:
- KNX-Datenkommunikation über UART.
- Konfiguration und Gruppenobjekte.
- Programmiertaste und -LED für KNX-Geräte.

### **4.4 Sicherheit**
Mechanismen wie der Watchdog und der Save-Pin erhöhen die Systemstabilität und ermöglichen Wiederherstellung nach Fehlern.

---

## **5. Zusammenfassung**

Das OpenKNX-Framework ist ein leistungsstarkes und flexibles Werkzeug für die Entwicklung von KNX-Anwendungen. Es kombiniert eine modulare Architektur, umfassende Hardware-Unterstützung und robuste KNX-Integration. Durch die klare Struktur und die Unterstützung moderner Mikrocontroller-Architekturen ist es sowohl für Einsteiger als auch für erfahrene Entwickler geeignet.

