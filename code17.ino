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
int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT;
int maxRadius = SCREEN_HEIGHT - 10;

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  myServo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  for(int angle = 0; angle <= 180; angle += 5) {
    myServo.write(angle);
    delay(30);
    long dist = getDistance();
    if(dist > 200) dist = 200;
    
    display.clearDisplay();
    // Draw arcs
    display.drawCircle(centerX, centerY, maxRadius/3, SSD1306_WHITE);
    display.drawCircle(centerX, centerY, maxRadius*2/3, SSD1306_WHITE);
    display.drawCircle(centerX, centerY, maxRadius, SSD1306_WHITE);
    
    // Sweep line
    float rad = radians(angle - 90);
    int x2 = centerX + maxRadius * cos(rad);
    int y2 = centerY + maxRadius * sin(rad);
    display.drawLine(centerX, centerY, x2, y2, SSD1306_WHITE);
    
    // Object dot
    if(dist < 200) {
      int objRadius = map(dist, 0, 200, 0, maxRadius);
      int objX = centerX + objRadius * cos(rad);
      int objY = centerY + objRadius * sin(rad);
      display.fillCircle(objX, objY, 3, SSD1306_WHITE);
    }
    
    display.display();
  }
  
  // Reverse sweep
  for(int angle = 180; angle >= 0; angle -= 5) {
    // similar code as above...
    myServo.write(angle);
    delay(30);
    long dist = getDistance();
    if(dist > 200) dist = 200;
    // redraw with new angle...
    // (repeat drawing code for brevity)
  }
}