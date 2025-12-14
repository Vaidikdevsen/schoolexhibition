#include <Servo.h>

const int trigPin = 10;
const int echoPin = 11;
Servo myServo;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(12);
}

void loop() {
  // Rotate Servo to 90 degrees
  myServo.write(90); 
  delay(500);

  // Measure Distance
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034) / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(500);
}