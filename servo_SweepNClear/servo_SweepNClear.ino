#include <Adafruit_NeoPixel.h>

//PIN (Digital)
#define Mot_A1 9
#define Mot_A2 6  //Right Motor (PWM)
#define Mot_B1 5
#define Mot_B2 11  //Left Motor (PWM)
#define Mot_R1 3
#define Mot_R2 2  //RPM Sensor

#define trigLR A5  //trigger for L and R is shared
#define trigF 12
#define echoL 7  //Ultrasonic
#define echoF 8
#define echoR 13

#define Grab A4  //Claw
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
unsigned long currenttime, refreshtime, reportPeriod;  //For millis() functions
int duration, distance;                                //For Ultrasonic functions
float distancePL, distancePR;
unsigned int counterR, counterL;
int distanceR, distanceL, distanceF;
int SmolAngle;


int pulseWidth;

bool stateServo = false, ranOnce = false;
int stateTurn;
int Black, White;
bool starting = false, BnWstate = false, ending = false;
int A, B, C, D, E, F;
int echo, trig;

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
  servo("radar", 0);
  delay(200);
  radarGetAngle();
  Serial.println("radar, 30");
  servo("radar", SmolAngle);
  delay(3000);
  Serial.println("radar, 30");
  servo("radar", 30);
  delay(2000);
  Serial.println("radar, 30");
  servo("grab", 60);
  delay(500);
  Serial.println("radar, 30");
  servo("grab", 30);
  delay(500);
}

void radarGetAngle() {
  float SmolDistanceL, SmolDistanceR;
  int SmolAngleL, SmolAngleR;
  SmolDistanceL = 30;
  SmolDistanceR = 30;
  for (int i = 0; i <= 10; i++) {
    int angle = i * 6;
    servo("radar", angle);
    delay(100);
    ultrasonic("PreciseL");
    delay(50);
    ultrasonic("PreciseR");
    Serial.print("DistanceL: ");
    Serial.println(distancePL);
    Serial.print("DistanceR: ");
    Serial.println(distancePR);
    if (distancePL < SmolDistanceL) {
      SmolDistanceL = distancePL;
      SmolAngleL = map(angle, 0, 60, 30, -30);
      Serial.println(".");
    }
    if (distancePR < SmolDistanceR) {
      SmolDistanceR = distancePR;
      SmolAngleR = map(angle, 0, 60, -30, 30);
      Serial.println(".");
    }
    Serial.print("angle: ");
    Serial.println(map(angle, 0, 60, 30, -30));
  }

  if (SmolDistanceR <= 0 || SmolDistanceL <= 0 || SmolAngleR > 30 || SmolAngleR < -30 || SmolAngleL > 30 || SmolAngleL < -30) {
    Serial.println("==============================");
    Serial.println("=================TEST INVAID================");
    Serial.println("==============================");
    radarGetAngle();
  } else if (SmolAngleR + SmolAngleL > 12) {
    Serial.println("==============================");
    Serial.print("SmolAngleL: ");
    Serial.println(SmolAngleL);
    Serial.print("DistanceL: ");
    Serial.println(SmolDistanceL);
    Serial.print("SmolAngleR: ");
    Serial.println(SmolAngleR);
    Serial.print("DistanceR: ");
    Serial.println(SmolDistanceR);
    Serial.println("==============================");
    Serial.print("Error By: ");
    Serial.println(SmolAngleR + SmolAngleL);
    delay(3000);
    radarGetAngle();
  } else SmolAngle = map(SmolAngleL, 30, -30, 0, 60);

  Serial.println("===============TESTING COMPLETE===============");
  Serial.print("SmolAngleL: ");
  Serial.println(SmolAngleL);
  Serial.print("DistanceL: ");
  Serial.println(SmolDistanceL);
  Serial.print("SmolAngleR: ");
  Serial.println(SmolAngleR);
  Serial.print("DistanceR: ");
  Serial.println(SmolDistanceR);
  Serial.print("SmolAngle: ");
  Serial.println(SmolAngle);
  Serial.println("==============================");
}

void ultrasonic(String dir) {
  float distanceP;
  if (dir == "L" || dir == "R" || dir == "PreciseL" || dir == "PreciseR") {
    trig = trigLR;
    if (dir == "L" || dir == "PreciseL") echo = echoL;
    else if (dir == "R" || dir == "PreciseR") echo = echoR;
  } else if (dir == "F") {
    trig = trigF;
    echo = echoF;
  }
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  delay(20);
  if (dir == "L" || dir == "R" || dir == "F") distance = (duration * 0.0340 / 2);
  if (dir == "PreciseL" || dir == "PreciseR") distanceP = (duration * 0.0340 / 2);
  if (distance >= 400) distance = 1;  //Lock random value due to sensor is touching the wall
  if (dir == "L") distanceL = distance;
  else if (dir == "R") distanceR = distance;
  else if (dir == "F") distanceF = distance;
  else if (dir == "PreciseL") distancePL = distanceP;
  else if (dir == "PreciseR") distancePR = distanceP;
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
    Serial.print(i);
  }
}
