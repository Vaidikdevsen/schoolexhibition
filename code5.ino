int maxDist = 0;

void loop() {
  for (int pos = 0; pos <= 180; pos++) {
    myservo.write(pos);
    int d = getDistance();
    if (d > maxDist) maxDist = d;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Angle: "); display.println(pos);
    display.print("Dist: "); display.println(d);
    display.print("Max: "); display.println(maxDist);
    display.display();
    delay(30);
  }
}