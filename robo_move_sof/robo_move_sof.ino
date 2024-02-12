#include <Adafruit_NeoPixel.h>

int trig = 12, echo = 13, Servomot = 10, Mot_A1 = 9, Mot_A2 = 6, Mot_B1 = 5, Mot_B2 = 3;
const int Neo = 7;
unsigned long currenttime, distance, duration;
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
  if (distance < 10) {
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
    Yellow();
  }
  delay(50);
  if (!state) {
      if (millis() - 500 >= currenttime){
        currenttime = millis();
    }
  }
}

void ultrasonic() {
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance:");
  Serial.println(distance);
}

void moving(int angle) {
  int pulseWidth = map(angle, 0, 180, 450, 2500);

  digitalWrite(Servomot, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(Servomot, LOW);
  delay(20);
  ultrasonic();
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
  Serial.println("Right");
  moving(0);

  Serial.println("Front");
  moving(90);

  Serial.println("Left");
  moving(180);

  Serial.println("Dir detect complete");
  delay(4000);
  state = false;
}

void Yellow() {
  strip.clear();
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(140, 255, 0));
    Serial.println(i);
    Serial.println("Yellow");
    strip.show();
  }
}
