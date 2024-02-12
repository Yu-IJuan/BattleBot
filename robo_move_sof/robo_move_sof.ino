#include <Adafruit_NeoPixel.h>

int trig = 12, echo = 13, Servomot = 10, Mot_A1 = 9, Mot_A2 = 6, Mot_B1 = 5, Mot_B2 = 3;
const int Neo = 7;
unsigned long currenttime, duration, sensetime;
int distance, distance1, distance2, distance3;
int LED_COUNT = 4, BRIGHTNESS = 255;
bool state = false;
Adafruit_NeoPixel strip(LED_COUNT, Neo, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(Servomot, OUTPUT);
  pinMode(Mot_A1, OUTPUT);
  pinMode(Mot_A2, OUTPUT);
  pinMode(Mot_B1, OUTPUT);
  pinMode(Mot_B2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (distance < 20) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);
    Serial.println("Stop");
    Red();
  }
  else {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 255);
    analogWrite(Mot_B2, 0);
    Serial.println("Go");
    moving(90);
    Serial.print("DistanceF: ");
    Serial.println(distance);
    Yellow();
  }
  delay(50);
  if (!state) {
    if (millis() - 500 >= currenttime) {
      currenttime = millis;
    }
  }
}

int ultrasonic() {
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  //  Serial.print("Distance:");
  //  Serial.println(distance);
  //  return distance;
}

int moving(int angle) {
  int pulseWidth = map(angle, 0, 180, 490, 2500);

  digitalWrite(Servomot, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(Servomot, LOW);
  delay(125);
  ultrasonic();
  //  return distance;
  delay(200);
}

void Red() {
  //  strip.clear();
  //  for (int i = 0; i < LED_COUNT; i++) {
  //    strip.setPixelColor(i, strip.Color(0, 255, 0));
  //    strip.show();
  //    Serial.println("R");
  //    Serial.println(i);
  //  }
  state = true;
  //  int distance1 = moving(0);
  //  int distance2 = moving(90), distance3 = moving(180);

  while (state) {
    Serial.println("Right");
    moving(0);
    distance1 = distance;

    Serial.println("Front");
    moving(90);
    distance2 = distance;

    Serial.println("Left");
    moving(180);
    distance3 = distance;

    Serial.println("Dir detect complete");

    state = false;

    Serial.print("DistanceR: ");
    Serial.println( distance1);
    Serial.print("DistanceF: ");
    Serial.println( distance2);
    Serial.print("DistanceL: ");
    Serial.println( distance3);

    moving(90);
    delay(2000);
  }
  if (distance1 > 10 && distance2 > 10 && distance3 > 10) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 255);
    delay(1000);
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 250);
    analogWrite(Mot_B2, 0);
    delay(1000);
  }
  else if (distance3 > 10 && distance2 > 10) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 255);
    delay(1000);
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 250);
    analogWrite(Mot_B2, 0);
    delay(1000);
  }
  else if (distance2 > 10 && distance1) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 250);
    analogWrite(Mot_B2, 0);
  }
  else if ( distance1 > 10) {
    analogWrite(Mot_A1, 255);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 255);
    analogWrite(Mot_B2, 0);
    delay(1000);
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 250);
    analogWrite(Mot_B2, 0);
    delay(1000);
  }
  else if (distance3 > 10) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 255);
    delay(1000);
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 250);
    analogWrite(Mot_B2, 0);
    delay(1000);
  }
  else if (distance1 < 10 && distance2 < 10 && distance3 < 10) {
    analogWrite(Mot_A1, 255);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 255);
  }
}

void Yellow() {
  //  strip.clear();
  //  for (int i = 0; i < LED_COUNT; i++) {
  //    strip.setPixelColor(i, strip.Color(140, 255, 0));
  //    Serial.println(i);
  //    Serial.println("Yellow");
  //    strip.show();
  //  }
}
