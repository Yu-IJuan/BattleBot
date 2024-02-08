int servo = 10;

void setup() {
  pinMode(servo, OUTPUT);
}

void loop() {
  moving(0);
  delay(500);

  moving(90);
  delay(500);

  moving(180);
  delay(500);
}

void moving(int angle) {
  int pulseWidth = map(angle, 0, 180, 544, 2400);

  digitalWrite(servo, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(servo, LOW);
  delay(20);
}
