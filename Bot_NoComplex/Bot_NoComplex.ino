#include <Adafruit_NeoPixel.h>

//PIN (Digital)
#define Mot_A1 9
#define Mot_A2 6  //Right Motor (PWM)
#define Mot_B1 5
#define Mot_B2 11  //Left Motor (PWM)
#define Mot_R1 3
#define Mot_R2 2  //RPM Sensor (Not used)

//Ultrasonic PINs
#define trigR A5  //trigger for L and R is shared
#define trigL 10
#define trigF 12
#define echoL 7
#define echoF 8
#define echoR 13

//IR Sensor PINs
#define D1 A0
#define D3 A1
#define D4 A2
#define D5 A3
#define D6 A6
#define D8 A7

//Utilies PINs
#define Grab A4  //Claw
#define Neo 4    //LED pin

#define LEDCount 4  //For NeoPixels

Adafruit_NeoPixel pixels(LEDCount, Neo, NEO_GRB + NEO_KHZ800);

int pulseWidth = 0;
int echo = 0, trig = 0;
int A = 0, B = 0, C = 0, D = 0, E = 0, F = 0;

//Const-Variables
#define MagicNumber 15
#define BiggerMagicNumber 25
#define MagicDelay 100

int Mot_AnaA1, Mot_AnaA2, Mot_AnaB1, Mot_AnaB2;

unsigned int counterR = 0, counterL = 0;
float distanceR, distanceL, distanceF;
bool starting = false, BnWstate = false, ending = false;
int Black = 0, White = 0;
bool counted = false;
unsigned long enddelay = 0;
unsigned long time = 0;
unsigned long counttime = 0;
bool sw = false, coneB = false;


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

  pinMode(trigL, OUTPUT);
  pinMode(trigR, OUTPUT);
  pinMode(trigF, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(echoF, INPUT);
  pinMode(echoR, INPUT);

  pinMode(Grab, OUTPUT);

  pixels.begin();
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!starting) {
    LEDbeforestart();
    cone();
    LED("Start");
    servo("grab", 90);
    delay(1000);
    calibrateLight();
    followLine("before");
    starting = true;
    enddelay = millis();
  } else if (starting && !ending) {  //during maze
    if (millis() >= enddelay + 10000) {
      if (A > Black || B > Black || C > Black || D > Black || E > Black || F > Black) {
        ending = true;
        analogWrite(Mot_A2, 0);
        analogWrite(Mot_B1, 0);
        return;
      }
    }
    if (millis() >= time + 1000) {
      Serial.println("Crash Detect");
      Serial.print(counterR);
      Serial.println(counterL);
      crashDetect();
      time = millis();
    }
    if (millis() >= time + 100) {
      Serial.println("Main");
      ultrasonic("R");
      ultrasonic("F");  //Checking with different ultrasonic sensors
      ultrasonic("L");
      readAnalogs();
      if (distanceF > BiggerMagicNumber || distanceR > BiggerMagicNumber) {
        followRight();
      } else if (distanceF <= MagicNumber && distanceL > BiggerMagicNumber) {
        analogWrite(Mot_A2, 0);
        analogWrite(Mot_B1, 0);
        turncalibrate();
        left();
      } else if (distanceF <= MagicNumber && distanceR <= MagicNumber && distanceL <= MagicNumber) {
        analogWrite(Mot_A2, 0);
        analogWrite(Mot_B1, 0);
        Serial.println("U");
        uturn();
      }
    }
  } else {  //ending
    followLine("after");
  }
}

void ISR_R() {
  counterR++;
  if (sw) {
    counttime = millis();
    sw = false;
  }
}

void ISR_L() {
  counterL++;
  if (!sw) {
    counttime = millis();
    sw = true;
  }
}

bool cone() {
  ultrasonic("F");
  for (;;) {
    ultrasonic("F");
    if (distanceF <= 30) {
      Serial.println()
      if (!coneB) {
        time = millis();
        coneB = true;
      }
    }
    if (time >= millis() + 1000 && distanceF <= 30) {
      return true;
    } else coneB = false;
  }
}

void LEDbeforestart() {
  int rVal, gVal;
  unsigned long currentTime = millis();
  for (;;) {
    readAnalogs();
    if ((A + F / 2) <= 512) {
      rVal = (0.501 * (A + F / 2)) - 0.501;
      gVal = 255;
    } else if ((A + F) / 2 > 512) {
      rVal = 255;
      gVal = (-0.502 * (A + F / 2)) + 513.51;
    }
    if (millis() - currentTime >= 500) pixels.setPixelColor(0, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 1000) pixels.setPixelColor(1, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 1500) pixels.setPixelColor(2, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 2000) pixels.setPixelColor(3, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 2500) break;

    pixels.show();
  }
}

void calibrateLight() {
  int tempBlack = 0, tempWhite = 0;
  counterL = 0;
  counterR = 0;
  int steps = 0;
  int i = 0;
  while (i < 5) {
    readAnalogs();
    int average = (C + D) / 2;
    while (steps > counterR && steps > counterL) {
      if (steps > counterR) {
        analogWrite(Mot_A2, 220);
      } else {
        analogWrite(Mot_A2, 0);
      }
      if (steps > counterL) {
        analogWrite(Mot_B1, 255);
      } else {
        analogWrite(Mot_B1, 0);
      }
    }
    Serial.println();
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    if (B > 700 && C > 700 && D > 700 && E > 700 && !BnWstate) {
      tempBlack = tempBlack + average;
      BnWstate = !BnWstate;
      i++;
      counterL = 0;
      counterR = 0;
      steps = 12;
      Serial.println("Temp Black Total: ");
      Serial.println(tempBlack);
    } else if (B < 600 && C < 600 && D < 600 && E < 600 && BnWstate) {
      tempWhite = tempWhite + average;
      BnWstate = !BnWstate;
      i++;
      counterL = 0;
      counterR = 0;
      steps = 12;
      Serial.println("Temp White Total: ");
      Serial.println(tempWhite);
    }
  }

  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  White = tempWhite / 2 + 100;
  Black = tempBlack / 3 - 100;
  Serial.print("White: ");
  Serial.println(White);
  Serial.print("Black: ");
  Serial.println(Black);
}

void crashDetect() {
  if (millis() >= counttime + 1000) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);
    Serial.println("Crashed");
    analogWrite(Mot_A1, 225);
    analogWrite(Mot_B2, 255);
    delay(350);
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_B2, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_A2, 235);
    analogWrite(Mot_B2, 0);
    analogWrite(Mot_B2, 235);
    delay(150);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B2, 0);
    counterR = 0;
    counterL = 0;
  }
}

void servo(String var, int angle) {
  int pulseWidth = map(angle, 0, 180, 544, 2400);
  int Pin = 0;
  if (var == "grab") Pin = Grab;
  for (int i = 0; i < 5; i++) {
    digitalWrite(Pin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(Pin, LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}

void followLine(String dir) {
  readAnalogs();
  analogWrite(Mot_A2, 225);  //220
  analogWrite(Mot_B1, 235);  //255
  delay(500);
  Serial.println("followLine");
  if (dir == "before" || dir == "after") {
    for (;;) {
      readAnalogs();
      if (A > Black && F > Black) break;
      else if (C > Black && D > Black) {  //Forward
        analogWrite(Mot_A2, 225);
        analogWrite(Mot_B1, 235);
      } else if (B > Black && C > Black) {  //Little R
        analogWrite(Mot_A2, 235);
        analogWrite(Mot_B1, 235);
      } else if (D > Black && E > Black) {  //Little L
        analogWrite(Mot_A2, 215);
        analogWrite(Mot_B1, 235);
      } else if (B > Black) {  //Extreme R
        analogWrite(Mot_A2, 255);
        analogWrite(Mot_B1, 235);
      } else if (E > Black) {  //Extreme L
        analogWrite(Mot_A2, 225);
        analogWrite(Mot_B1, 255);
      }
    }
    if (dir == "before") {
      analogWrite(Mot_A2, 0);
      analogWrite(Mot_B1, 0);
      delay(1500);
      servo("grab", 30);
      analogWrite(Mot_A2, 225);
      analogWrite(Mot_B1, 235);
      delay(370);
      analogWrite(Mot_A2, 0);
      analogWrite(Mot_B1, 0);
      delay(1000);
      left();
      delay(1000);
      forward(6, 6);
      followLine("during");
    } else if (dir == "after") {
      analogWrite(Mot_A2, 0);
      analogWrite(Mot_B1, 0);
      delay(1500);
      servo("grab", 90);
      analogWrite(Mot_A1, 225);
      analogWrite(Mot_B2, 255);
      delay(800);
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_B2, 0);
      for (;;) delay(1000);
    }
  } else if ("during") {
    for (;;) {
      readAnalogs();
      if (C > Black && D > Black) {  //Forward
        analogWrite(Mot_A2, 225);
        analogWrite(Mot_B1, 235);
      } else if (B > Black && C > Black) {  //Little R
        analogWrite(Mot_A2, 235);
        analogWrite(Mot_B1, 235);
      } else if (D > Black && E > Black) {  //Little L
        analogWrite(Mot_A2, 215);
        analogWrite(Mot_B1, 235);
      } else if (B > Black) {  //Extreme R
        analogWrite(Mot_A2, 245);
        analogWrite(Mot_B1, 235);
      } else if (E > Black) {  //Extreme L
        analogWrite(Mot_A2, 225);
        analogWrite(Mot_B1, 255);
      } else if (A > Black) {
        analogWrite(Mot_A2, 225);
        analogWrite(Mot_B1, 0);
      } else if (F > Black) {
        analogWrite(Mot_A2, 0);
        analogWrite(Mot_B1, 235);
      } else break;
    }
  }
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
}

void followRight() {
  int offsetR = distanceR * 10;
  if (offsetR <= 45) {
    LED("WR");
    Mot_AnaA2 = map(offsetR, 45, 20, 250, 255);
    Mot_AnaB1 = map(offsetR, 20, 45, 100, 160);
  } else if (offsetR <= 85 && offsetR > 45) {
    LED("WR");
    Mot_AnaA2 = map(offsetR, 95, 46, 240, 250);
    Mot_AnaB1 = map(offsetR, 46, 95, 160, 210);
  } else if (offsetR > 95 && offsetR < 105) {
    LED("WF");
    Mot_AnaA2 = map(offsetR, 96, 104, 207, 217);  //R 220, 225
    Mot_AnaB1 = map(offsetR, 104, 96, 220, 230);  //L 235
  } else if (offsetR >= 95 && offsetR < 201) {
    LED("WL");
    Mot_AnaA2 = map(offsetR, 200, 105, 160, 215);
    Mot_AnaB1 = map(offsetR, 105, 200, 240, 245);
  } else if (offsetR >= 201) {
    Mot_AnaA2 = map(offsetR, 250, 201, 100, 160);
    Mot_AnaB1 = map(offsetR, 201, 250, 245, 255);
  } else {
    Mot_AnaA2 = 225;
    Mot_AnaB1 = 235;
  }
  if (Mot_AnaA2 >= 255) Mot_AnaA2 = 255;
  else if (Mot_AnaA2 < 100) Mot_AnaA2 = 100;
  if (Mot_AnaB1 >= 255) Mot_AnaB1 = 255;
  else if (Mot_AnaB1 < 100) Mot_AnaB1 = 100;
  analogWrite(Mot_A2, Mot_AnaA2);
  analogWrite(Mot_B1, Mot_AnaB1);
}

void readAnalogs() {
  A = analogRead(D1);
  B = analogRead(D3);
  C = analogRead(D4);
  D = analogRead(D5);
  E = analogRead(D6);
  F = analogRead(D8);
}

void forward(int stepsR, int stepsL) {
  delay(MagicDelay);
  Mot_AnaA2 = 0;
  Mot_AnaB1 = 0;
  counterL = 0;
  counterR = 0;
  while (stepsR > counterR && stepsL > counterL) {
    if (stepsR > counterR) {
      Mot_AnaA2 = 225;
    } else {
      Mot_AnaA2 = 0;
    }
    if (stepsL > counterL) {
      Mot_AnaB1 = 235;
    } else {
      Mot_AnaB1 = 0;
    }
    analogWrite(Mot_A2, Mot_AnaA2);
    analogWrite(Mot_B1, Mot_AnaB1);
    Serial.print(counterR);
    Serial.println(counterL);
  }
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  Mot_AnaA2 = 0;
  Mot_AnaB1 = 0;
  counterL = 0;
  counterR = 0;
}

void left() {
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_A2, 235);
  delay(1000);
  analogWrite(Mot_A2, 0);
}

void right() {
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B1, 235);
  delay(1100);
  analogWrite(Mot_B1, 0);
}

void uturn() {
  if (distanceL <= 12) {
    analogWrite(Mot_A1, 225);
    analogWrite(Mot_B1, 250);
    delay(900);
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_B1, 0);
  } else {
    analogWrite(Mot_A2, 225);
    analogWrite(Mot_B2, 240);
    delay(1400);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B2, 0);
  }
}

void turncalibrate() {
  LED("Detect");
  if (distanceF <= 30 && distanceF >= 14) {
    analogWrite(Mot_A2, 225);
    analogWrite(Mot_B1, 235);
    while (distanceF > 14) {
      ultrasonic("F");
      crashDetect();
    }
  } else if (distanceF < 14) {
    analogWrite(Mot_A1, 225);
    analogWrite(Mot_B2, 255);
    while (distanceF < 14) {
      ultrasonic("F");
      crashDetect();
    }
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
}

void LED(String dir) {
  pixels.clear();

  if (dir == "L") {
    pixels.setPixelColor(0, pixels.Color(140, 255, 0));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(140, 255, 0));
  } else if (dir == "R") {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(140, 255, 0));
    pixels.setPixelColor(2, pixels.Color(140, 255, 0));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
  } else if (dir == "F") {
    for (int i = 0; i < LEDCount; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
  } else if (dir == "Start") {
    pixels.setPixelColor(0, pixels.Color(255, 0, 255));
    pixels.setPixelColor(1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(2, pixels.Color(0, 0, 255));
    pixels.setPixelColor(3, pixels.Color(255, 255, 255));
  } else if (dir == "WL") {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(255, 255, 255));
    pixels.setPixelColor(2, pixels.Color(255, 255, 255));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
  } else if (dir == "WR") {
    pixels.setPixelColor(0, pixels.Color(255, 255, 255));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(255, 255, 255));
  } else if (dir == "WF") {
    pixels.setPixelColor(0, pixels.Color(140, 255, 0));
    pixels.setPixelColor(1, pixels.Color(140, 255, 0));
    pixels.setPixelColor(2, pixels.Color(140, 255, 0));
    pixels.setPixelColor(3, pixels.Color(140, 255, 0));
  } else if (dir == "Detect") {
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.setPixelColor(1, pixels.Color(0, 0, 255));
    pixels.setPixelColor(2, pixels.Color(0, 0, 255));
    pixels.setPixelColor(3, pixels.Color(0, 0, 255));
  } else if (dir == "UTurn") {
    for (int i = 0; i < LEDCount; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    }
  }
  pixels.show();
  servo("grab", 50);
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
  }
  distanceP = distanceTemp / 3;
  if (dir == "L") distanceL = distanceP;
  else if (dir == "R") distanceR = distanceP;
  else if (dir == "F") distanceF = distanceP;
}