// --- Library Includes ---
#include <Servo.h>

// --- Pin Definitions ---
// Ultrasonic Sensor
const int TRIG_PIN = 10;
const int ECHO_PIN = 11;

// Servo Motor
const int SERVO_PIN = 12;

// --- Global Objects ---
Servo myservo;

// --- Timing and Distance Variables ---
long duration;  // stores the duration of the ultrasonic pulse
int distance_cm; // stores the calculated distance in cm
int angle = 0;   // stores the current servo angle

// --- Setup Function ---
void setup() {
  // 1. Initialize Serial Communication (ESSENTIAL for this version)
  Serial.begin(9600);
  Serial.println("--- Sonar System Running (Serial Output) ---");
  
  // 2. Initialize Ultrasonic Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // 3. Attach Servo
  myservo.attach(SERVO_PIN);
  myservo.write(90); // Start at the center (90 degrees)
  
  delay(1000); 
}

// --- Main Loop Function ---
void loop() {
  // Sweep the Servo from 0 to 180 degrees
  for (angle = 0; angle <= 180; angle += 5) {
    myservo.write(angle);
    measure_and_print();
    delay(50); // Small delay for movement
  }

  // Sweep the Servo back from 180 to 0 degrees
  for (angle = 180; angle >= 0; angle -= 5) {
    myservo.write(angle);
    measure_and_print();
    delay(50); // Small delay for movement
  }
}

// --- Custom Function to Measure Distance ---
int measureDistance() {
  // Clear trigger, then send 10us pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read the echo pin
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate distance: Distance (cm) = (Duration * 0.0343) / 2
  distance_cm = duration * 0.0343 / 2;
  
  return distance_cm;
}

// --- Custom Function to Print Data to Serial Monitor ---
void measure_and_print() {
  distance_cm = measureDistance();
  
  // Print to Serial Monitor
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" deg | Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
}
