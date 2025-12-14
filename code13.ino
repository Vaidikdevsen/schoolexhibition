#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SERVO_PIN 12
#define TRIG_PIN 10
#define ECHO_PIN 11

Servo servo;

long echoTime;
int distanceCM;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  servo.attach(SERVO_PIN);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (1); // OLED fail
  }

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(25, 28);
  oled.println("SONAR READY");
  oled.display();
  delay(2000);
}

int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  echoTime = pulseIn(ECHO_PIN, HIGH, 35000);
  if (echoTime == 0) return -1;

  return echoTime * 0.034 / 2;
}

void loop() {

  for (int angle = 15; angle <= 165; angle++) {
    servo.write(angle);
    delay(30);

    distanceCM = readDistance();
    displayData(angle, distanceCM);
  }

  for (int angle = 165; angle >= 15; angle--) {
    servo.write(angle);
    delay(30);

    distanceCM = readDistance();
    displayData(angle, distanceCM);
  }
}

void displayData(int angle, int dist) {
  oled.clearDisplay();

  oled.setTextSize(2);
  oled.setCursor(0, 0);
  oled.print("ANG:");
  oled.print(angle);

  oled.setTextSize(2);
  oled.setCursor(0, 32);
  oled.print("DIS:");

  if (dist < 0 || dist > 400) {
    oled.print("--");
  } else {
    oled.print(dist);
    oled.print("cm");
  }

  oled.display();
}
