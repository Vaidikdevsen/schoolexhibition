#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int trigPin = 10;
const int echoPin = 11;
const int buzzerPin = 9;
const int buttonPin = 7;
const int ledPin = 8;
Servo myServo;

unsigned long lastUpdate = 0;
const long updateInterval = 50;
const int alertDistance = 50;
float tempC = 25.0;
bool wideSweep = true;
bool lowPower = false;

struct Object {
  int angle;
  int distance;
};
Object objects[5];
int objectCount = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  myServo.attach(12);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Radar Starting...");
  
  // Read last closest object from EEPROM
  int savedAngle = EEPROM.read(0);
  int savedDistance = EEPROM.read(1);
  if (savedDistance != 255) { // Valid data
    lcd.setCursor(0, 1);
    lcd.print("Last: ");
    lcd.print(savedDistance);
    lcd.print("cm@");
    lcd.print(savedAngle);
    lcd.print((char)223);
  }
  delay(2000);
  lcd.clear();
  Serial.println("Angle,Distance");
}

void loop() {
  static int angle = 15;
  static int step = 2;
  static int minAngle = 15;
  static int maxAngle = 165;
  
  if (digitalRead(buttonPin) == LOW) {
    wideSweep = !wideSweep;
    lowPower = wideSweep ? false : true; // Narrow sweep = low power
    minAngle = wideSweep ? 15 : 60;
    maxAngle = wideSweep ? 165 : 120;
    angle = minAngle;
    delay(200);
  }
  
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= updateInterval) {
    scan(angle);
    if (angle == minAngle && step == 2) {
      printObjects();
      if (objectCount > 0) {
        EEPROM.write(0, objects[0].angle); // Save closest
        EEPROM.write(1, min(objects[0].distance, 255));
      }
      objectCount = 0;
    }
    angle += step;
    if (angle >= maxAngle || angle <= minAngle) step = -step;
    lastUpdate = currentMillis;
  }
}

void scan(int angle) {
  myServo.write(angle);
  int distance = lowPower ? getDistance() : getAverageDistance(3);
  
  if (distance > 0 && distance <= 200 && objectCount < 5) {
    objects[objectCount] = {angle, distance};
    objectCount++;
  }
  
  lcd.setCursor(0, 0);
  lcd.print(wideSweep ? "Wide " : "Narrow");
  lcd.print(lowPower ? "LP" : "  ");
  lcd.print("A:");
  lcd.print(angle);
  lcd.print((char)223);
  
  lcd.setCursor(0, 1);
  if (distance == 0 || distance > 200) {
    lcd.print("No Object Found ");
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  } else {
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" cm   ");
    if (distance < alertDistance) {
      tone(buzzerPin, 1000, 100);
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
  
  Serial.print(angle);
  Serial.print(",");
  Serial.println(distance);
}

void printObjects() {
  Serial.println("Detected Objects:");
  for (int i = 0; i < objectCount; i++) {
    Serial.print("Object ");
    Serial.print(i + 1);
    Serial.print(": Angle=");
    Serial.print(objects[i].angle);
    Serial.print(", Distance=");
    Serial.print(objects[i].distance);
    Serial.println(" cm");
  }
}

int getAverageDistance(int samples) {
  long total = 0;
  int validReadings = 0;
  for (int i = 0; i < samples; i++) {
    int dist = getDistance();
    if (dist != 0) {
      total += dist;
      validReadings++;
    }
    delay(5);
  }
  return validReadings > 0 ? total / validReadings : 0;
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 50000);
  if (duration == 0) return 0;
  float speedOfSound = 331.4 + 0.606 * tempC;
  return (duration * (speedOfSound / 10000)) / 2;
}