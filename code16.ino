#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int SERVO_PIN = 12;
const int TRIG_PIN = 10;
const int ECHO_PIN = 11;

Servo myServo;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Sonar");
  display.display();
  delay(2000);
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void loop() {
  for(int angle = 0; angle <= 180; angle += 10) {
    myServo.write(angle);
    delay(300);
    long dist = getDistance();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Angle: ");
    display.println(angle);
    display.print("Dist: ");
    display.print(dist);
    display.println(" cm");
    display.display();
  }
  
  for(int angle = 180; angle >= 0; angle -= 10) {
    myServo.write(angle);
    delay(300);
    long dist = getDistance();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Angle: ");
    display.println(angle);
    display.print("Dist: ");
    display.print(dist);
    display.println(" cm");
    display.display();
  }
}