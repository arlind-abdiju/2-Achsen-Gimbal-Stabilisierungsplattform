# 🎯 2-Achsen Gimbal Stabilisierungsplattform

> **Vorbereitungsprojekt für den Self-Balancing Robot** – entwickelt um PID-Regelung, Sensorintegration und Embedded Systems Programmierung praktisch kennenzulernen, bevor ich mit dem Hauptprojekt gestartet habe.

---

## 📌 Kontext & Motivation

Als Maschinenbau-Absolvent mit Schwerpunkt Mechatronik hatte ich zwar solide theoretische Kenntnisse in Regelungstechnik – aber kaum praktische Erfahrung mit Embedded Systems oder Mikrocontrollern.

Mein eigentliches Ziel war der Aufbau eines **selbst-balancierenden Roboters** (PID-geregelt, Encoder-Feedback, WLAN-Dashboard). Da die Bauteile noch nicht geliefert wurden, nutzte ich die Zeit sinnvoll: Ich baute dieses Gimbal-Projekt als **strukturierte Einarbeitung** – von den absoluten Grundlagen bis zum ersten funktionierenden Regelkreis.

---

## 🎬 Demo Video

*(Video wird hier eingebettet)*

---

## 🗺️ Lernpfad – Von Null zum PID-Regler

Ich habe mich bewusst schrittweise vorgearbeitet, anstatt direkt ins Komplexe einzusteigen:

| Schritt | Was ich gemacht habe | Was ich gelernt habe |
|---|---|---|
| 1 | LED blinken, Taster | GPIO, digitalWrite, digitalRead |
| 2 | MPU6050 auslesen | I2C Protokoll, Sensorfusion |
| 3 | OLED Display | I2C mit mehreren Geräten parallel |
| 4 | Servo Ansteuerung | PWM Signale, ESP32Servo Bibliothek |
| 5 | Sensor + Servo kombinieren | Echtzeit Regelung, Latenz |
| 6 | PID Regler implementieren | P, I, D Anteile praktisch verstehen |
| 7 | Mechanischer Aufbau | Prototyping, Mechanik-Software Schnittstelle |
| 8 | PID Feintuning | Parametrierung, Stabilität vs. Reaktionszeit |

---

## 🔧 Hardware

| Bauteil | Beschreibung |
|---|---|
| ESP32 WROOM DevKit V1 | Mikrocontroller mit WLAN |
| MPU6050 | 6-Achsen Gyroskop & Beschleunigungssensor (I2C) |
| 2x Servo Motor | Für X und Y Achse |
| Pappe / Karton | Prototyp Chassis |
| Breadboard + Jumperkabel | Verkabelung |

---

## 🔌 Verkabelung

| ESP32 Pin | Bauteil | Funktion |
|---|---|---|
| D32 | Servo 1 | X-Achse (Vor/Zurück) |
| D33 | Servo 2 | Y-Achse (Links/Rechts) |
| D21 | MPU6050 SDA | I2C Datenleitung |
| D22 | MPU6050 SCL | I2C Taktleitung |
| 3V3 | MPU6050 VCC | Versorgungsspannung |
| GND | MPU6050 GND | Masse |

---

## 🧠 Regelungskonzept

### Selbstkalibrierung beim Start

Beim Einschalten nimmt das System 100 Messwerte auf und berechnet den Mittelwert als Nullpunkt. Dadurch werden Sensor-Offsets automatisch kompensiert – ohne manuelle Justierung.

```cpp
for(int i = 0; i < 100; i++) {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  sum_ax += ax;
  sum_ay += ay;
  delay(5);
}
null_ax = sum_ax / 100;
null_ay = sum_ay / 100;
```

### PID-Regler

Der Kern des Projekts – ein PID-Regler der den Neigungswinkel auswertet und die Servos in Echtzeit korrigiert:

```
Fehler = Ist-Winkel - Soll-Winkel (0°)

PID Ausgabe = Kp × Fehler                    (Proportional)
            + Ki × Integral(Fehler)           (Integral)
            + Kd × Ableitung(Fehler)          (Differential)

Servo Position = 90° - PID Ausgabe
```

### Zwei-Achsen Stabilisierung

```
MPU6050 misst Neigung
        │
        ├── X-Achse (Pitch) → PID 1 → Servo 1 korrigiert Vor/Zurück
        └── Y-Achse (Roll)  → PID 2 → Servo 2 korrigiert Links/Rechts
```

---

## ✅ Was gut funktioniert hat

- **Selbstkalibrierung** – funktionierte zuverlässig und brauchte keine manuelle Justierung
- **Grundlegende Stabilisierung** – die Plattform hat Neigungen erkannt und korrigiert
- **Praxistest mit Würfel** – ein auf die Plattform gelegter Würfel blieb bei ruhigen Bewegungen stabil
- **PID Grundprinzip** – der Regler hat funktioniert und auf Neigungen reagiert
- **I2C Bus** – MPU6050 und OLED Display liefen stabil parallel auf denselben Pins
- **Iterativer Entwicklungsprozess** – durch schrittweise Erweiterung konnte ich Probleme isoliert lösen

---

## ⚠️ Was nicht optimal war & was ich daraus gelernt habe

### PID Abstimmung
**Problem:** Die Balance zwischen Reaktionsgeschwindigkeit und Stabilität war schwer zu finden. Zu niedriger Kp → Plattform korrigiert zu träge. Zu hoher Kp → System schwingt unkontrolliert.

**Erkenntnis:** PID-Parametrierung ist ein iterativer Prozess der Erfahrung und systematisches Vorgehen erfordert. In der Industrie werden dafür Methoden wie Ziegler-Nichols genutzt. Für das Hauptprojekt (Self-Balancing Robot) werde ich ein **Echtzeit-Dashboard** implementieren um Parameter live anzupassen.

---

### Mechanischer Aufbau
**Problem:** Pappe als Material war zu flexibel – die Servos lagen nicht immer perfekt parallel zum Boden. Verbindungen zwischen Pappe und Servo-Aufsatz hielten nicht optimal mit Kleber, teilweise mit leichtem Winkelversatz.

**Erkenntnis:** Mechanische Präzision beeinflusst die Regelgüte erheblich – ein Prinzip das ich aus der Konstruktionslehre kannte, hier aber erstmals direkt erlebt habe. Für das Hauptprojekt werde ich ein stabileres Chassis aus Holz oder Acrylglas verwenden.

---

### Servo Leistung
**Problem:** Die verwendeten Hobby-Servos waren für schnelle Korrekturen teilweise zu schwach und zu langsam. Bei abrupten Neigungen kamen sie nicht hinterher.

**Erkenntnis:** Aktuatorauswahl ist entscheidend für die Regelgüte – beim Self-Balancing Robot verwende ich daher DC-Getriebemotoren mit Hall-Encodern für präzises Geschwindigkeitsfeedback.

---

### Sensorrauschen
**Problem:** Die rohen MPU6050 Werte rauschen und führten zu leichtem Zittern der Servos auch in Ruhelage.

**Erkenntnis:** Für das Hauptprojekt implementiere ich einen **Kalman Filter** zur Sensorfusion aus Gyroskop und Beschleunigungssensor – Industriestandard für präzise Winkelschätzung.

---

## 🎯 Projektabschluss & Bewertung

Dieses Projekt hatte von Anfang an **nicht das Ziel ein perfektes Endprodukt** zu liefern. Es war eine gezielte Vorbereitung um:

1. Den ESP32 und seine Peripherie praktisch kennenzulernen
2. PID-Regelung vom Papier in Code zu übersetzen
3. Die Herausforderungen an der Schnittstelle Mechanik-Software zu erleben
4. Eine solide Basis für das Hauptprojekt aufzubauen

Das Ergebnis: Ein funktionierender Prototyp der das Grundprinzip demonstriert – mit klar identifizierten Verbesserungspunkten die direkt in das Hauptprojekt eingeflossen sind.

---

## 🔭 Weiterentwicklung im Hauptprojekt

Alle Erkenntnisse aus diesem Projekt fließen direkt in den **Self-Balancing Robot** ein:

| Problem hier | Lösung im Hauptprojekt |
|---|---|
| PID schwer abstimmbar | Live PID-Tuning über WLAN Dashboard |
| Sensorrauschen | Kalman Filter Implementierung |
| Kein Geschwindigkeitsfeedback | Hall-Encoder an DC Motoren |
| Pappe zu instabil | Stabiles Holz/Acryl Chassis |
| Manuelle Beobachtung | Echtzeit Telemetrie im Browser |


---

## 📚 Gesamte Lernbilanz

Durch dieses Projekt habe ich folgende praktische Kompetenzen aufgebaut:

**Embedded Systems:**
- ESP32 GPIO, PWM, I2C in der Praxis
- Sensor-Kalibrierung und Rauschunterdrückung
- Echtzeit-Programmierung ohne delays

**Regelungstechnik:**
- PID-Regler Implementierung auf Mikrocontroller
- Praktisches Verständnis von Kp, Ki, Kd
- Stabilität vs. Reaktionsgeschwindigkeit abwägen

**Hardware & Prototyping:**
- Löten von Stiftleisten und Sensoren
- Breadboard Verkabelung und Debugging
- Einfluss der Mechanik auf die Regelgüte

---

## 👤 Autor

**Arlind Abdiju**
Maschinenbau M.Sc. | Schwerpunkt Mechatronik

[![LinkedIn](https://img.shields.io/badge/LinkedIn-blue)](https://www.linkedin.com/in/arlindabdiju/)

