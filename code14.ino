#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // Adjust to 64 if your OLED is 128x32
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C  // Common address, change to 0x3D if needed

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TRIG_PIN 10
#define ECHO_PIN 11
#define SERVO_PIN 12

Servo servo;

const int maxDist = 200;  // Maximum distance to consider (in cm)
int angle = 0;
int stepAngle = 15;       // Sweep in larger steps for faster scanning
int dir = 1;

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  servo.attach(SERVO_PIN);
  servo.write(0);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("OLED initialization failed!"));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Sonar Ready"));
  display.display();
  delay(2000);
}

void loop() {
  // Simple text-based backup display
  int distance = measureDistance();
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.print(F("Ang:"));
  display.print(angle);
  display.print(F("°"));
  
  display.setCursor(10, 40);
  display.print(F("Dist:"));
  if (distance > 0 && distance < maxDist) {
    display.print(distance);
    display.print(F("cm"));
  } else {
    display.print(F("--"));
  }
  display.display();
  
  // Sweep servo
  angle += dir * stepAngle;
  if (angle >= 180 || angle <= 0) {
    dir = -dir;
    angle = (angle < 0) ? 0 : 180;
  }
  servo.write(angle);
  
  delay(300);  // Adjust for smoother or faster scanning
}

int measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout for ~500cm
  if (duration == 0) return -1;  // No echo
  
  int dist = duration * 0.034 / 2;
  return dist;
}