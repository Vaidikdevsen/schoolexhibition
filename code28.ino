#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int trigPin = 10;
const int echoPin = 11;
const int buzzerPin = 9;
Servo myServo;

unsigned long lastUpdate = 0;
const long updateInterval = 50;
const int alertDistance = 50;
int closestDistance = 200;
int closestAngle = 0;
float tempC = 25.0; // Default temperature (adjust or add sensor)

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  myServo.attach(12);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Radar Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  static int angle = 15;
  static int step = 2;
  
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= updateInterval) {
    scan(angle);
    if (angle == 15 && step == 2) {
      Serial.print("Closest: Angle=");
      Serial.print(closestAngle);
      Serial.print(", Distance=");
      Serial.print(closestDistance);
      Serial.println(" cm");
      closestDistance = 200;
      closestAngle = 0;
    }
    angle += step;
    if (angle >= 165 || angle <= 15) step = -step;
    lastUpdate = currentMillis;
  }
}

void scan(int angle) {
  myServo.write(angle);
  int distance = getAverageDistance(3);
  
  if (distance > 0 && distance < closestDistance) {
    closestDistance = distance;
    closestAngle = angle;
  }
  
  lcd.setCursor(0, 0);
  lcd.print("Angle: ");
  lcd.print(angle);
  lcd.print((char)223);
  lcd.print("  ");
  
  lcd.setCursor(0, 1);
  if (distance == 0 || distance > 200) {
    lcd.print("No Object Found ");
    noTone(buzzerPin);
  } else {
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" cm   ");
    if (distance < alertDistance) {
      tone(buzzerPin, 1000, 100);
    }
  }
  
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" | Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
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
  float speedOfSound = 331.4 + 0.606 * tempC; // m/s
  return (duration * (speedOfSound / 10000)) / 2; // Convert to cm
}