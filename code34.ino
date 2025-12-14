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
  lcd.print("360 Radar Start");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Rotate clockwise
  myServo.write(0);  // full speed CW
  for (int i = 0; i < 12; i++) {
    delay(500);
    distance = getDistance();
    showLCD("CW", distance);
  }

  myServo.write(90); // stop
  delay(1000);

  // Rotate counterclockwise
  myServo.write(180); // full speed CCW
  for (int i = 0; i < 12; i++) {
    delay(500);
    distance = getDistance();
    showLCD("CCW", distance);
  }

  myServo.write(90); // stop
  delay(1000);
}

int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 0;
  return duration * 0.034 / 2;
}

void showLCD(String dir, int dist) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dir: " + dir);

  lcd.setCursor(0, 1);
  if (dist == 0 || dist > 200) {
    lcd.print("No Obj");
  } else {
    lcd.print("Dist: ");
    lcd.print(dist);
    lcd.print("cm");
  }

  Serial.print("Direction: ");
  Serial.print(dir);
  Serial.print(" | Distance: ");
  Serial.println(dist);
}