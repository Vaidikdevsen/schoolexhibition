// --- Library Includes ---
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin Definitions ---
// Ultrasonic Sensor
const int TRIG_PIN = 10;
const int ECHO_PIN = 11;

// Servo Motor
const int SERVO_PIN = 12;

// OLED Display (I2C)
// The SDA and SCL pins are typically fixed on Arduino Uno:
// SDA -> A4
// SCL -> A5
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)

// --- Global Objects ---
Servo myservo;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Timing and Distance Variables ---
long duration;  // stores the duration of the ultrasonic pulse
int distance_cm; // stores the calculated distance in cm
int angle = 0;   // stores the current servo angle

// --- Setup Function ---
void setup() {
  // 1. Initialize Serial Communication (for debugging/monitoring)
  Serial.begin(9600);
  
  // 2. Initialize Ultrasonic Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // 3. Attach Servo
  myservo.attach(SERVO_PIN);
  myservo.write(90); // Start at the center (90 degrees)
  
  // 4. Initialize OLED Display
  Wire.begin(); // Start I2C communication
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C is common for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Display initial message
  display.display(); // Clear buffer and display splash screen
  delay(1000);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Sonar Initialized!");
  display.display();
  delay(500);
}

// --- Main Loop Function ---
void loop() {
  // 1. Sweep the Servo from 0 to 180 degrees
  for (angle = 0; angle <= 180; angle += 5) {
    myservo.write(angle);
    measure_and_display();
    delay(50); // Small delay for servo movement and stable reading
  }

  // 2. Sweep the Servo back from 180 to 0 degrees
  for (angle = 180; angle >= 0; angle -= 5) {
    myservo.write(angle);
    measure_and_display();
    delay(50); // Small delay for servo movement and stable reading
  }
}

// --- Custom Function to Measure Distance ---
int measureDistance() {
  // Clear the trigger pin by setting it LOW for 2 microseconds
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Trigger the sensor by setting the trigPin HIGH for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read the echo pin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  // Speed of sound is approx 343 m/s or 0.0343 cm/us.
  // The sound travels to the object and back, so divide by 2.
  // Distance (cm) = (Duration * Speed of Sound) / 2
  // Distance (cm) = Duration * 0.0343 / 2
  distance_cm = duration * 0.0343 / 2;
  
  return distance_cm;
}

// --- Custom Function to Display Data ---
void measure_and_display() {
  distance_cm = measureDistance();
  
  // Clear the display buffer
  display.clearDisplay();
  
  // --- Display Angle ---
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Angle: ");
  display.print(angle);
  display.println((char)248); // Print degree symbol
  
  // --- Display Distance ---
  display.setTextSize(2);
  display.setCursor(0, 32);
  display.print("Dist: ");
  
  // Handle case where object is too far (e.g., beyond 400cm or a very low reading)
  if (distance_cm > 400 || distance_cm <= 0) { 
    display.print("---");
  } else {
    display.print(distance_cm);
  }
  display.print(" cm");

  // Push the buffer to the actual display
  display.display();

  // Send to Serial Monitor for debugging
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" deg | Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
}
