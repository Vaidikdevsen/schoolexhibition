// -----------------------------------------------------------------------------
// Arduino Sonar Sweep Project
// Hardware: Arduino UNO, HC-SR04, Servo, I2C OLED (SSD1306, 128x64)
// Libraries: Adafruit_SSD1306, Adafruit_GFX, Servo
// -----------------------------------------------------------------------------

// Include necessary libraries
#include <Wire.h>               // Required for I2C communication (OLED)
#include <Adafruit_GFX.h>       // Core graphics library
#include <Adafruit_SSD1306.h>   // Library for the OLED display
#include <Servo.h>              // Library for controlling the servo motor

// --- Hardware Pin Definitions ---
const int TRIG_PIN = 10;        // HC-SR04 Trigger pin connected to Arduino Digital 10
const int ECHO_PIN = 11;        // HC-SR04 Echo pin connected to Arduino Digital 11
const int SERVO_PIN = 12;       // Servo motor signal pin connected to Arduino Digital 12

// --- OLED Display Parameters ---
#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define OLED_RESET -1           // Reset pin # (or -1 if sharing Arduino reset pin)
const byte I2C_ADDRESS = 0x3C;  // I2C address for 0.96" 128x64 OLED (common address)

// --- Constants ---
const float SPEED_OF_SOUND_CM_PER_US = 0.0343; // Speed of sound in air (cm/µs)
const long MAX_DISTANCE_CM = 400;             // Maximum distance to measure (for time-out)
const long MAX_ECHO_DURATION_US = MAX_DISTANCE_CM * 2 / SPEED_OF_SOUND_CM_PER_US; // Max duration for 400cm round trip

// --- Global Objects ---
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myServo;                  // Create a servo object to control the servo

// --- Variables ---
int currentAngle = 0;           // Current angle of the servo
int stepDirection = 1;          // Direction of the sweep (1 for forward, -1 for backward)

// -----------------------------------------------------------------------------
// Function to measure distance using the HC-SR04 sensor
// -----------------------------------------------------------------------------
float measureDistanceCM() {
  // Clear the trigger pin by setting it LOW for 2µs (ensures a clean pulse)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Trigger the sensor by setting the trigger pin HIGH for 10µs
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the duration (in µs) of the echo pulse (time-out after MAX_ECHO_DURATION_US)
  // pulseIn returns 0 if it times out
  long duration_us = pulseIn(ECHO_PIN, HIGH, MAX_ECHO_DURATION_US);

  // Check for time-out (out-of-range or error)
  if (duration_us == 0) {
    return -1.0; // Return -1.0 to signify out-of-range or error
  }

  // Calculate the distance in centimeters:
  // Distance = (Duration * Speed of Sound) / 2 (since it's a round trip)
  float distance_cm = duration_us * SPEED_OF_SOUND_CM_PER_US / 2.0;

  // Clamp the distance to a safe maximum for display/logic
  if (distance_cm > MAX_DISTANCE_CM) {
    return -1.0; // Treat distances greater than max as out-of-range
  }

  return distance_cm;
}

// -----------------------------------------------------------------------------
// Setup function: Runs once when the sketch starts
// -----------------------------------------------------------------------------
void setup() {
  // --- Initialize Serial Communication (for debugging, optional) ---
  // Serial.begin(9600);
  // Serial.println("Sonar Sweep Starting...");

  // --- Initialize Sensor Pins ---
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // --- Attach Servo ---
  myServo.attach(SERVO_PIN);
  // Set initial position
  myServo.write(0);
  delay(100); // Give servo time to move to initial position

  // --- Initialize OLED Display ---
  // Check if the display object was created successfully
  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    // Serial.println(F("SSD1306 allocation failed"));
    while (true); // Loop forever if initialization fails
  }

  // Clear the buffer
  display.clearDisplay();

  // Display a startup message
  display.setTextSize(2);        // Large text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 25);
  display.println("SONAR");
  display.println("SWEEP");
  display.display();
  delay(2000); // Wait for 2 seconds
}

// -----------------------------------------------------------------------------
// Loop function: Runs repeatedly after setup
// -----------------------------------------------------------------------------
void loop() {
  // --- 1. Servo Movement ---
  currentAngle += stepDirection;

  // Check for sweep limits and reverse direction if a limit is reached
  if (currentAngle >= 180) {
    currentAngle = 180;
    stepDirection = -1; // Change to backward
  } else if (currentAngle <= 0) {
    currentAngle = 0;
    stepDirection = 1;  // Change to forward
  }

  // Move the servo to the new angle
  myServo.write(currentAngle);

  // --- 2. Distance Measurement ---
  float distance_cm = measureDistanceCM();

  // --- 3. Display Update ---
  display.clearDisplay(); // Clear the display buffer

  // Display Title
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sonar Sweep Data");
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  // Display Angle
  display.setTextSize(2);
  display.setCursor(0, 15);
  display.print("Angle:");
  display.setCursor(80, 15);
  display.print(currentAngle);
  display.print((char)248); // Degree symbol

  // Display Distance
  display.setCursor(0, 40);
  display.print("Dist:");

  if (distance_cm > 0.0) {
    // Distance is valid (within range)
    display.setCursor(80, 40);
    // Print distance with one decimal place
    display.print(distance_cm, 1);
    display.print("cm");
  } else {
    // Distance is out-of-range (<= 0.0)
    display.setTextSize(2); // Keep large text for 'OUT'
    display.setCursor(70, 40);
    display.print("OUT");
  }

  // Push the buffer content to the actual display
  display.display();

  // --- 4. Timing Control ---
  // A small delay to allow the servo to move and stabilize, and to control the sweep speed.
  // This is crucial for avoiding jitter and giving time for the next reliable ultrasonic reading.
  // 15ms is a good compromise for smooth servo movement and a decent scan rate.
  delay(15);
}
