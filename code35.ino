#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

const int trigPin = 10;
const int echoPin = 11;
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  myServo.attach(12);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Radar Sweep Init");
  delay(2000);
  lcd.clear();
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 5) {
    myServo.write(angle);
    delay(100);

    distance = getDistance();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("[");

    int barLength = map(distance, 0, 200, 1, 14);
    for (int i = 0; i < barLength; i++) lcd.print("-");
    lcd.print("]");

    lcd.setCursor(0, 1);
    lcd.print("Ang:");
    lcd.print(angle);
    lcd.print((char)223);
    lcd.print(" D:");
    lcd.print(distance);
    lcd.print("cm");

    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print(" | Distance: ");
    Serial.println(distance);
  }
}

int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 0;
  return duration * 0.034 / 2;
}