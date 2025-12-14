// Same includes as code 1

void loop() {
  for (int pos = 0; pos <= 180; pos++) {
    myservo.write(pos);
    int d = getDistance();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Angle: "); display.println(pos);
    display.print("Dist: "); display.print(d); display.println(" cm");
    display.display();
    delay(30);
  }

  for (int pos = 180; pos >= 0; pos--) {
    myservo.write(pos);
    int d = getDistance();

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Angle: "); display.println(pos);
    display.print("Dist: "); display.print(d); display.println(" cm");
    display.display();
    delay(30);
  }
}