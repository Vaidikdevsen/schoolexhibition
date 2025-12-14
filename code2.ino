#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C   // Common I2C address

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions
#define SERVO_PIN 12
#define TRIG_PIN 10
#define ECHO_PIN 11

Servo sonarServo;

long duration;
int distance;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  sonarServo.attach(SERVO_PIN);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true); // OLED not found
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 25);
  display.println("Ultrasonic Sonar");
  display.display();
  delay(2000);
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout safety
  int dist = duration * 0.034 / 2;

  if (dist <= 0 || dist > 400) return -1;
  return dist;
}

void loop() {

  // Sweep 0° to 180°
  for (int angle = 0; angle <= 180; angle++) {
    sonarServo.write(angle);
    delay(20);

    distance = getDistance();
    showData(angle, distance);
  }

  // Sweep 180° to 0°
  for (int angle = 180; angle >= 0; angle--) {
    sonarServo.write(angle);
    delay(20);

    distance = getDistance();
    showData(angle, distance);
  }
}

void showData(int angle, int distance) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("A:");
  display.print(angle);
  display.print((char)247); // degree symbol

  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print("D:");

  if (distance == -1) {
    display.print("--");
  } else {
    display.print(distance);
    display.print("cm");
  }

  display.display();
}