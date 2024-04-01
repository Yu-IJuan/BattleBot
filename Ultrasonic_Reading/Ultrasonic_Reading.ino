// #include <Adafruit_NeoPixel.h>

// //PIN (Digital)
// #define Mot_A1 9
// #define Mot_A2 6  //Right Motor (PWM)
// #define Mot_B1 5
// #define Mot_B2 11  //Left Motor (PWM)
// #define Mot_R1 3
// #define Mot_R2 2  //RPM Sensor

#define trigR A5  //trigger for L and R is shared
#define trigL 10
#define trigF 12
#define echoL 7  //Ultrasonic
#define echoF 8
#define echoR 13

// #define Grab A4  //Claw
// #define Neo 0    //LED pin
// #define Radar 4  //Radar pin

// #define MagicNumber 15
// #define BiggerMagicNumber 25
// #define MagicDelay 100
// #define LEDCount 4
// int Mot_AnaA1, Mot_AnaA2, Mot_AnaB1, Mot_AnaB2;

// //PIN (Analog)
// #define D1 A0
// #define D3 A1
// #define D4 A2
// #define D5 A3
// #define D6 A6
// #define D8 A7  //For IR B&W Sensor
// //

// Adafruit_NeoPixel pixels(LEDCount, Neo, NEO_GRB + NEO_KHZ800);

// unsigned long currenttime, refreshtime, reportPeriod;  //For millis() functions
float distancePL, distancePR;
// unsigned int counterR, counterL;
float distanceR, distanceL, distanceF;
int SmolAngle;


// int pulseWidth;

// bool stateServo = false, ranOnce = false;
// int stateTurn;
// int Black, White;
// bool starting = false, BnWstate = false, ending = false;
// int A, B, C, D, E, F;
int echo, trig;
// bool backCali;

void setup() {
  // put your setup code here, to run once:
  // pinMode(Mot_A1, OUTPUT);
  // pinMode(Mot_A2, OUTPUT);
  // pinMode(Mot_B1, OUTPUT);
  // pinMode(Mot_B2, OUTPUT);

  // pinMode(D1, INPUT);
  // pinMode(D3, INPUT);
  // pinMode(D4, INPUT);
  // pinMode(D5, INPUT);
  // pinMode(D6, INPUT);
  // pinMode(D8, INPUT);

  pinMode(trigR, OUTPUT);
  pinMode(trigL, OUTPUT);
  pinMode(trigF, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(echoF, INPUT);
  pinMode(echoR, INPUT);

  // pinMode(Grab, OUTPUT);
  // pinMode(Radar, OUTPUT);
  Serial.begin(9600);
  // pixels.begin();
  // pixels.clear();
  // for (int i = 0; i < LEDCount; i++) {
  //   pixels.setPixelColor(i, pixels.Color(255, 255, 0));
  // }
  // pixels.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  ultrasonic("R");
  ultrasonic("F");
  ultrasonic("L");

  Serial.println("Distance");
  Serial.println(distanceR);
  Serial.println(distanceF);
  Serial.println(distanceL);
}

void ultrasonic(String dir) {
  float distanceP = 0, distance = 0, duration = 0;
  if (dir == "R") {
    trig = trigR;
    echo = echoR;
  } else if (dir == "L") {
    trig = trigL;
    echo = echoL;
  } else if (dir == "F") {
    trig = trigF;
    echo = echoF;
  }
  float distanceTemp = 0.0;
  for (int i = 0; i < 3; i++) {
    digitalWrite(trig, LOW);
    delay(5);
    digitalWrite(trig, HIGH); 
    delay(10);
    digitalWrite(trig, LOW);
    duration = pulseIn(echo, HIGH);
    delay(10);
    distance = (duration * 0.0340 / 2);
    if (distance >= 400) distance = 1;
    if (distance == 1) i = i - 1;
    else {
      distanceTemp = distanceTemp + distance;
    }
    Serial.print(i);
  }
  distanceP = distanceTemp / 3;
  if (dir == "L") distanceL = distanceP;
  else if (dir == "R") distanceR = distanceP;
  else if (dir == "F") distanceF = distanceP;
}
