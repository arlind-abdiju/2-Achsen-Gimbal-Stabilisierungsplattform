/*
 * 2-Achsen Gimbal Stabilisierungsplattform
 * =========================================
 * Autor:    Arlind Abdiju
 * Hardware: ESP32 WROOM DevKit V1, MPU6050, 2x Servo Motor
 * 
 * Beschreibung:
 * Selbst-stabilisierende Plattform mit PID-Regler.
 * Der MPU6050 misst die Neigung auf zwei Achsen und
 * korrigiert die Servo-Positionen in Echtzeit.
 * 
 * Vorbereitungsprojekt für den Self-Balancing Robot.
 */

#include <ESP32Servo.h>
#include <MPU6050.h>
#include <Wire.h>

// =============================================
// HARDWARE PINS
// =============================================
#define SERVO_X_PIN   32   // Servo 1 - X-Achse (Vor/Zurück)
#define SERVO_Y_PIN   33   // Servo 2 - Y-Achse (Links/Rechts)
#define SDA_PIN       21   // I2C Daten
#define SCL_PIN       22   // I2C Takt
#define BOOT_PIN       0   // BOOT Knopf zum Starten

// =============================================
// PID PARAMETER
// =============================================
// Kp: Proportionalanteil - Stärke der direkten Korrektur
// Ki: Integralanteil     - korrigiert langsame Drift über Zeit
// Kd: Differentialanteil - dämpft Überschwingen und Zittern
float Kp = 0.003;
float Ki = 0.0001;
float Kd = 0.002;

// Integral Begrenzung (verhindert Integral Windup)
const float INTEGRAL_MAX = 1000.0;

// =============================================
// OBJEKTE & VARIABLEN
// =============================================
Servo servo1;
Servo servo2;
MPU6050 mpu(0x68);

// Kalibrierter Nullpunkt
int null_x = 0;
int null_y = 0;

// PID Zustandsvariablen
float fehler_x = 0, fehler_y = 0;
float letzter_fehler_x = 0, letzter_fehler_y = 0;
float integral_x = 0, integral_y = 0;

// =============================================
// SETUP
// =============================================
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  mpu.initialize();

  // Servos initialisieren und in Mittelposition fahren
  servo1.attach(SERVO_X_PIN);
  servo2.attach(SERVO_Y_PIN);
  servo1.write(90);
  servo2.write(90);
  delay(2000);

  // Selbstkalibrierung - 100 Messungen Mittelwert
  Serial.println("Kalibrierung läuft - bitte nicht bewegen...");
  long sum_ax = 0, sum_ay = 0;
  int16_t ax, ay, az, gx, gy, gz;

  for (int i = 0; i < 100; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sum_ax += ax;
    sum_ay += ay;
    delay(5);
  }
  null_x = sum_ax / 100;
  null_y = sum_ay / 100;

  Serial.print("Nullpunkt X: "); Serial.println(null_x);
  Serial.print("Nullpunkt Y: "); Serial.println(null_y);
  Serial.println("Kalibrierung abgeschlossen!");

  // Warten auf BOOT Knopf
  pinMode(BOOT_PIN, INPUT_PULLUP);
  Serial.println("BOOT Knopf drücken zum Starten...");
  while (digitalRead(BOOT_PIN) == HIGH) {
    delay(100);
  }
  Serial.println("System gestartet!");
}

// =============================================
// HAUPTSCHLEIFE
// =============================================
void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Fehler berechnen (Abweichung vom Nullpunkt)
  fehler_x = ax - null_x;
  fehler_y = ay - null_y;

  // Integral mit Windup Schutz
  integral_x = constrain(integral_x + fehler_x, -INTEGRAL_MAX, INTEGRAL_MAX);
  integral_y = constrain(integral_y + fehler_y, -INTEGRAL_MAX, INTEGRAL_MAX);

  // Differential (Änderungsrate des Fehlers)
  float d_x = fehler_x - letzter_fehler_x;
  float d_y = fehler_y - letzter_fehler_y;

  // PID Ausgabe berechnen
  float ausgabe_x = Kp * fehler_x + Ki * integral_x + Kd * d_x;
  float ausgabe_y = Kp * fehler_y + Ki * integral_y + Kd * d_y;

  // Fehler für nächsten Durchlauf speichern
  letzter_fehler_x = fehler_x;
  letzter_fehler_y = fehler_y;

  // Servo Position berechnen und auf sicheren Bereich begrenzen
  // 90° = Mittelposition, ±30° maximaler Ausschlag
  int pos_x = constrain(90 - (int)ausgabe_x, 60, 120);
  int pos_y = constrain(90 - (int)ausgabe_y, 60, 120);

  servo1.write(pos_x);
  servo2.write(pos_y);

  // Debug Ausgabe
  Serial.print("FX:"); Serial.print(fehler_x);
  Serial.print(" FY:"); Serial.print(fehler_y);
  Serial.print(" PX:"); Serial.print(pos_x);
  Serial.print(" PY:"); Serial.println(pos_y);

  delay(20); // 50Hz Regelfrequenz
}
