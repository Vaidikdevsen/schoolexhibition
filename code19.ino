#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ================= OLED CONFIG ================= */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ================= PIN CONFIG ================= */
#define SERVO_PIN 12
#define TRIG_PIN  10
#define ECHO_PIN  11

Servo sonarServo;

/* ================= VARIABLES ================= */
long echoDuration;
int distanceCM;

/* ================= SETUP ================= */
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  sonarServo.attach(SERVO_PIN);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (true); // Halt if OLED not detected
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 28);
  display.println("Ultrasonic Sonar");
  display.display();
  delay(2000);
}

/* ================= DISTANCE FUNCTION ================= */
int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  echoDuration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout

  if (echoDuration == 0) return -1;

  int dist = echoDuration * 0.034 / 2;
  if (dist > 400) return -1;

  return dist;
}

/* ================= DISPLAY FUNCTION ================= */
void showData(int angle, int distance) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("ANG:");
  display.print(angle);

  display.setCursor(0, 32);
  display.print("DIS:");
  if (distance == -1) {
    display.print("--");
  } else {
    display.print(distance);
    display.print("cm");
  }

  display.display();
}

/* ================= LOOP ================= */
void loop() {

  // Sweep from 0° to 180°
  for (int angle = 0; angle <= 180; angle++) {
    sonarServo.write(angle);
    delay(25); // Smooth movement & stable reading

    distanceCM = readDistance();
    showData(angle, distanceCM);
  }

  // Sweep back from 180° to 0°
  for (int angle = 180; angle >= 0; angle--) {
    sonarServo.write(angle);
    delay(25);

    distanceCM = readDistance();
    showData(angle, distanceCM);
  }
}
