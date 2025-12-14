#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels (adjust to 32 if it's smaller)
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_I2C_ADDRESS 0x3C  // Common I2C address for SSD1306

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TRIG_PIN 10
#define ECHO_PIN 11
#define SERVO_PIN 12

Servo myServo;

int angle = 0;
int direction = 1;  // 1 for increasing, -1 for decreasing

void setup() {
  // Initialize serial for debugging (optional)
  Serial.begin(9600);
  
  // Initialize ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Attach servo
  myServo.attach(SERVO_PIN);
  myServo.write(0);  // Start at 0 degrees
  
  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Ultrasonic Sonar"));
  display.display();
  delay(2000);  // Show startup message for 2 seconds
}

void loop() {
  // Measure distance
  int distance = getDistance();
  
  // Display angle and distance
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Angle: "));
  display.print(angle);
  display.println(F(" deg"));
  display.print(F("Distance: "));
  display.print(distance);
  display.println(F(" cm"));
  display.display();
  
  // Update servo position
  angle += direction * 5;  // Move in steps of 5 degrees
  if (angle >= 180) {
    direction = -1;  // Reverse direction
    angle = 180;
  } else if (angle <= 0) {
    direction = 1;   // Reverse direction
    angle = 0;
  }
  myServo.write(angle);
  
  delay(200);  // Delay for smooth movement and display update
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  
  // Handle invalid readings
  if (distance > 400 || distance < 2) {  // Typical range for HC-SR04 is 2-400 cm
    return -1;  // Invalid
  }
  return distance;
}