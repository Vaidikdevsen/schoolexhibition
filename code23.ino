#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change to 0x3F if needed
const int trigPin = 10;
const int echoPin = 11;
Servo myServo;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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
  // Sweep from 15 to 165 and back
  for (int angle = 15; angle <= 165; angle += 2) {
    scan(angle);
  }
  for (int angle = 165; angle >= 15; angle -= 2) {
    scan(angle);
  }
}

void scan(int angle) {
  myServo.write(angle);
  delay(30); // Reduced delay for smoother movement
  int distance = getDistance();
  
  // Update LCD without clearing to avoid flicker
  lcd.setCursor(0, 0);
  lcd.print("Angle: ");
  lcd.print(angle);
  lcd.print((char)223);
  lcd.print("  "); // Clear trailing characters
  
  lcd.setCursor(0, 1);
  if (distance == 0 || distance > 200) {
    lcd.print("No Object Found ");
  } else {
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" cm   ");
  }
  
  // Serial output
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" | Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 50000); // 50ms timeout (~8.5m range)
  if (duration == 0) return 0;
  return (duration * 0.0343) / 2; // Speed of sound adjusted
}