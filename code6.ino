void loop() {
  for (int pos = 0; pos <= 180; pos++) {
    myservo.write(pos);
    int d = getDistance();
    int bar = map(d, 0, 200, 0, 128);

    display.clearDisplay();
    display.fillRect(0, 30, bar, 10, WHITE);
    display.setCursor(0, 0);
    display.print("Angle: "); display.println(pos);
    display.print("Dist: "); display.print(d); display.println(" cm");
    display.display();

    delay(30);
  }
}