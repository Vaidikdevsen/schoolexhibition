#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD I2C address — change 0x27 to 0x3F if needed
LiquidCrystal_I2C lcd(0x27, 16, 2);

// HC-SR04 ultrasonic sensor pins
const int trigPin = 10;
const int echoPin = 11;

// Servo object
Servo myServo;

long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  myServo.attach(12);

  lcd.init();         // Initialize LCD
  lcd.backlight();    // Turn on backlight
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Radar Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  for (int angle = 15; angle <= 165; angle++) {
    myServo.write(angle);
    delay(50);

    distance = getDistance();

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Angle: ");
    lcd.print(angle);
    lcd.print((char)223); // degree symbol

    lcd.setCursor(0, 1);
    if (distance == 0 || distance > 200) {
      lcd.print("No Object Found");
    } else {
      lcd.print("Distance: ");
      lcd.print(distance);
      lcd.print(" cm");
    }

    // Also print to serial (optional)
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print(" | Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(300);
  }
}

// Ultrasonic distance function
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  if (duration == 0) return 0;
  return duration * 0.034 / 2;
}