#include <Adafruit_NeoPixel.h>

//PIN (Digital)
#define Mot_A1 9
#define Mot_A2 6  //Right Motor (PWM)
#define Mot_B1 5
#define Mot_B2 11  //Left Motor (PWM)
#define Mot_R1 3
#define Mot_R2 2  //RPM Sensor

#define trigLR A5 //trigger for L and R is shared
#define trigF 12
#define echoL 7  //Ultrasonic
#define echoF 8
#define echoR 13

#define Grab A4   //Claw
#define Neo 10   //LED pin
#define Radar 4  //Radar pin

#define MagicNumber 15
#define BiggerMagicNumber 25
#define MagicDelay 100
#define LEDCount 4
int Mot_AnaA1, Mot_AnaA2, Mot_AnaB1, Mot_AnaB2;

//PIN (Analog)
#define D1 A0
#define D3 A1
#define D4 A2
#define D5 A3
#define D6 A6
#define D8 A7  //For IR B&W Sensor
//

void setup() {
  // put your setup code here, to run once:
  pinMode(Mot_A1, OUTPUT);
  pinMode(Mot_A2, OUTPUT);
  pinMode(Mot_B1, OUTPUT);
  pinMode(Mot_B2, OUTPUT);

  pinMode(D1, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D8, INPUT);

  pinMode(trigLR, OUTPUT);
  pinMode(trigF, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(echoF, INPUT);
  pinMode(echoR, INPUT);

  pinMode(Grab, OUTPUT);
  pinMode(Radar, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  Serial.println("radar, 60");
  servo("radar", 60);
  delay(500);
  Serial.println("radar, 60");
  servo("radar", 50);
  delay(500);
  Serial.println("radar, 60");
  servo("radar", 40);
  delay(500);
  Serial.println("radar, 30");
  servo("radar", 30); //Center
  delay(500);
  Serial.println("radar, 60");
  servo("radar", 20);
  delay(500);
  Serial.println("radar, 60");
  servo("radar", 10);
  delay(500);
  Serial.println("radar, 30");
  servo("radar", 0);
  delay(500);
  Serial.println("radar, 30");
  servo("radar", 30);
  delay(500);
  Serial.println("radar, 30");
  servo("grab", 60);
  delay(500);
  Serial.println("radar, 30");
  servo("grab", 30);
  delay(500);
}

void servo(String var, int angle) {
  int pulseWidth = map(angle, 0, 180, 544, 2400);
  int Pin;
  if (var == "grab") Pin = Grab;
  else if (var == "radar") Pin = Radar;
  for (int i = 0; i < 5; i++) {
    digitalWrite(Pin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(Pin, LOW);
    delayMicroseconds(20000 - pulseWidth);
    Serial.println(Pin);
    Serial.println(pulseWidth);
    Serial.println(i);
  }
}
