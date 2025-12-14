// Same includes + setup as code 1

void loop() {
  display.clearDisplay();

  for (int angle = 0; angle <= 180; angle++) {
    myservo.write(angle);
    int d = getDistance();
    int x = map(angle, 0, 180, 0, 128);

    display.drawLine(64, 64, x, 64 - map(d, 0, 200, 0, 64), WHITE);
    display.display();
    delay(40);
    display.clearDisplay();
  }
}