int trig = 12, echo = 13, Servomot = 10, Mot_A1 = 9, Mot_A2 = 6, Mot_B1 = 5, Mot_B2 = 3;
const int Neo = 7;
unsigned long currenttime, distance, duration;
int LED_COUNT = 4, BRIGHTNESS = 255;
Adafruit_NeoPixel strip(LED_COUNT, Neo, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(Servomot, OUTPUT);
  pinMode(Mot_A1, OUTPUT);
  pinMode(Mot_A2, OUTPUT);
  pinMode(Mot_B1, OUTPUT);
  pinMode(Mot_B2, OUTPUT);
}

void loop() {
  if (distance < 10) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);

    moving(0);
    ultrasonic();
    delay(1000);

    moving(90);
    ultrasonic();
    delay(1000);

    moving(180);
    ultrasonic();
    delay(1000);
  }
  else if (distance > 10) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 255);
    analogWrite(Mot_B2, 0);
  }
  if (millis() - 500 >= currenttime) {
    ultrasonic();
    currenttime = millis();
  }
  delay(50);
}
void ultrasonic() {
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
}

void moving(int angle) {
  int pulseWidth = map(angle, 0, 180, 544, 2400);

  digitalWrite(Servomot, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(Servomot, LOW);
  delay(20);
}

void Yellow() {
  strip.clear();
  for (int i = 0; i < Neo; i++) {
    strip.setPixelColor(i, strip.Color(140, 255, 0));
    Serial.println(i);
    Serial.println("Yellow");
    strip.show();
  }
