void loop() {
  for (int angle = 0; angle <= 180; angle++) {
    myservo.write(angle);

    int d = getDistance();
    int x = map(angle, 0, 180, 0, 128);
    int y = 64 - map(d, 0, 200, 0, 64);

    display.clearDisplay();
    display.drawCircle(64, 64, 60, WHITE);
    display.drawPixel(x, y, WHITE);

    display.setCursor(0, 0);
    display.print("A:"); display.print(angle);
    display.print(" D:"); display.print(d);
    display.display();

    delay(35);
  }
}