void loop() {
  for (int pos = 0; pos <= 180; pos++) {
    myservo.write(pos);
    int d = getDistance();

    Serial.print(pos);
    Serial.print(",");
    Serial.println(d);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("A: "); display.print(pos);
    display.print(" D: "); display.print(d);
    display.display();
    delay(20);
  }
}