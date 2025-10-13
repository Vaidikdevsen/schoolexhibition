#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
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

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true); // halt if OLED not found
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED Radar Ready");
  display.display();
  delay(2000);
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 5) {
    myServo.write(angle);
    delay(100);
    distance = getDistance();

    display.clearDisplay();
    display.setCursor(0, 0);
    if (angle<10){
     display.print("Angle:  ");
     display.print(angle);
    }else if(angle<99){
      display.print("Angle: ");
      display.print(angle);
    }else {
      display.print("Angle:");
      display.print(angle);
    }
    
    display.print((char)247); // degree symbol

    display.setCursor(0, 16);
    display.print("Distance: ");
    if (distance<10){
      display.print("  ");
    }else if (distance<100) {
      display.print(" ");
    }
    display.print(distance);
    display.println(" cm");

    display.display();

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