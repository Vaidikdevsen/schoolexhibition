#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

const int trigPin = 10;
const int echoPin = 11;
long duration;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  myServo.attach(12);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Median Filter");
  delay(2000);
  lcd.clear();
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 5) {
    myServo.write(angle);
    delay(100);

    int d = medianDistance();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Angle: ");
    lcd.print(angle);
    lcd.print((char)223);

    lcd.setCursor(0, 1);
    lcd.print("Dist: ");
    lcd.print(d);
    lcd.print(" cm");

    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print(" | Distance: ");
    Serial.println(d);
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

int medianDistance() {
  int a = getDistance();
  delay(20);
  int b = getDistance();
  delay(20);
  int c = getDistance();

  if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
  else if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
  else return c;
}