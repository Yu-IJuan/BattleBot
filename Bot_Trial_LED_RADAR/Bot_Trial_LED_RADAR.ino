#include <Adafruit_NeoPixel.h>
#include <MemoryFree.h>

//Forward HIGH R 220, L 255; MEDIUM R 210, L 235; LOW R 200, L 225
//Backward HIGH R 200, L 255; MEDIUM R 175, L 245

//PIN (Digital)
#define Mot_A1 9
#define Mot_A2 6  //Right Motor (PWM)
#define Mot_B1 5
#define Mot_B2 11  //Left Motor (PWM)
#define Mot_R1 3
#define Mot_R2 2  //RPM Sensor

#define trigR A5  //trigger for L and R is shared
#define trigL 10
#define trigF 12
#define echoL 7  //Ultrasonic
#define echoF 8
#define echoR 13

#define Grab A4  //Claw
#define Neo 0   //LED pin
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

Adafruit_NeoPixel pixels(LEDCount, Neo, NEO_GRB + NEO_KHZ800);

unsigned long reportPeriod = 0;    //For millis() functions
unsigned long distanceR = 0, distanceF = 0, distanceL = 0;
unsigned int counterR = 0, counterL = 0;
int SmolAngle = 0;


int pulseWidth = 0;

int stateTurn = 0;
int Black = 0, White = 0;
bool starting = false, BnWstate = false, ending = false;
int A = 0, B = 0, C = 0, D = 0, E = 0, F = 0;
int echo = 0, trig = 0;
bool backCali = 0;

bool started = false, ended = false;

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
  pinMode(Radar, OUTPUT);

  pixels.begin();
  //Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, CHANGE);
}

void loop() {
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  if (!started) {
    LEDbeforestart();
    Serial.println(F("starting"));
    LED("Start");
    servo("grab", 90);
    delay(1000);
    calibrateLight();
    followLine("before");
    started = true;
  }
  if (started && !ended) {
    ultrasonic("R");
    ultrasonic("F");  //Checking with different ultrasonic sensors,
    ultrasonic("L");
    servo("radar", 30);
    if (distanceR >= BiggerMagicNumber) {
      LED("R");
      turncalibrate("forward", "right");
      delay(MagicDelay);
      right();  //90 Degrees turn if left empty
      turncalibrate("advance", "right");
      delay(MagicDelay);
    } else if (distanceF >= MagicNumber) {
      LED("F");
      constforward();  //In center then walk straight
      delay(MagicDelay);
    } else if (distanceL >= BiggerMagicNumber) {
      LED("L");
      turncalibrate("forward", "left");
      delay(MagicDelay);
      left();  //90 Degrees turn if right empty
      turncalibrate("advance", "left");
      delay(MagicDelay);
    } else {
      LED("B");
      radarGetAngle("double");
      rotate(SmolAngle);
      constBackward();  //Backward if no route available
      delay(MagicDelay);
      ultrasonic("L");
      ultrasonic("R");
      if (distanceL >= BiggerMagicNumber) {
        LED("L");
        turncalibrate("backward", "default");
        delay(MagicDelay);
        left();
        delay(MagicDelay);
        LED("F");
        forward(24, 24);
        delay(MagicDelay);
      } else if (distanceR >= BiggerMagicNumber) {
        LED("R");
        right();
        delay(MagicDelay);
        ultrasonic("R");
        ultrasonic("F");
        if (distanceR >= BiggerMagicNumber) {
          LED("R");
          right();
          delay(MagicDelay);
          LED("F");
          forward(48, 48);
          delay(MagicDelay);
        } else if (distanceF >= BiggerMagicNumber) {
          LED("F");
          forward(48, 48);
          delay(MagicDelay);
        }
      }
    }
  } else if (ended) {
  }
}

void ISR_R() {
  counterR++;
  Serial.print(counterR);
}

void ISR_L() {
  counterL++;
  Serial.println(counterL);
}

void followLine(String dir) {
  readAnalogs();
  analogWrite(Mot_A2, 220);
  analogWrite(Mot_B1, 255);
  delay(500);
  Serial.println(F("followLine"));
  if (dir == "before") {
    for (;;) {
      readAnalogs();
      if (A > Black && F > Black) break;
      else if (C > Black && D > Black) {
        analogWrite(Mot_A2, 220);
        analogWrite(Mot_B1, 255);
        Serial.println(F("CD"));
      } else if (B > Black && C > Black) {
        analogWrite(Mot_A2, 230);
        analogWrite(Mot_B1, 255);
        Serial.println(F("BC"));
      } else if (D > Black && E > Black) {
        analogWrite(Mot_A2, 210);
        analogWrite(Mot_B1, 255);
        Serial.println(F("DE"));
      } else if (B > Black) {
        analogWrite(Mot_A2, 240);
        analogWrite(Mot_B1, 255);
        Serial.println(F("A"));
      } else if (E > Black) {
        analogWrite(Mot_A2, 200);
        analogWrite(Mot_B1, 255);
        Serial.println(F("A"));
      }
      printAnalogs();
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    Serial.println(F("Before"));
    delay(1500);
    servo("grab", 30);
    analogWrite(Mot_A2, 220);
    analogWrite(Mot_B1, 255);
    delay(370);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    delay(1000);
    left();
    delay(1000);
    forward(6, 6);
    followLine("during");
  } else if ("during") {
    for (;;) {
      readAnalogs();
      Serial.println(F("During"));
      if (C > Black && D > Black) {
        analogWrite(Mot_A2, 220);
        analogWrite(Mot_B1, 255);
        Serial.println(F("CD"));
      } else if (B > Black && C > Black) {
        analogWrite(Mot_A2, 230);
        analogWrite(Mot_B1, 255);
        Serial.println(F("BC"));
      } else if (D > Black && E > Black) {
        analogWrite(Mot_A2, 210);
        analogWrite(Mot_B1, 255);
        Serial.println(F("DE"));
      } else if (B > Black) {
        analogWrite(Mot_A2, 240);
        analogWrite(Mot_B1, 255);
        Serial.println(F("A"));
      } else if (E > Black) {
        analogWrite(Mot_A2, 200);
        analogWrite(Mot_B1, 255);
        Serial.println(F("A"));
      } else if (A > Black) {
        analogWrite(Mot_A2, 220);
        analogWrite(Mot_B1, 0);
        Serial.println(F("A"));
      } else if (F > Black) {
        analogWrite(Mot_A2, 0);
        analogWrite(Mot_B1, 255);
        Serial.println(F("F"));
      } else break;
      printAnalogs();
    }
  }
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
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
    if (millis() - currentTime >= 250) pixels.setPixelColor(0, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 500) pixels.setPixelColor(1, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 750) pixels.setPixelColor(2, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 1000) pixels.setPixelColor(3, pixels.Color(rVal, gVal, 0));
    if (millis() - currentTime >= 1250) break;

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
      Serial.print(counterR);
      Serial.print(counterL);
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
      Serial.println(F("Temp Black Total: "));
      Serial.println(tempBlack);
      delay(100);
    } else if (B < 600 && C < 600 && D < 600 && E < 600 && BnWstate) {
      tempWhite = tempWhite + average;
      BnWstate = !BnWstate;
      i++;
      counterL = 0;
      counterR = 0;
      steps = 12;
      Serial.println(F("Temp White Total: "));
      Serial.println(tempWhite);
      delay(100);
    }
  }

  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  White = tempWhite / 2 + 100;
  Black = tempBlack / 3 - 100;
  Serial.print(F("White: "));
  Serial.println(White);
  Serial.print("Black: ");
  Serial.println(Black);
}

void readAnalogs() {
  A = analogRead(D1);
  B = analogRead(D3);
  C = analogRead(D4);
  D = analogRead(D5);
  E = analogRead(D6);
  F = analogRead(D8);
}

void printAnalogs() {
  Serial.print(F("A: "));
  Serial.println(A);
  Serial.print(F("B: "));
  Serial.println(B);
  Serial.print(F("C: "));
  Serial.println(C);
  Serial.print(F("D: "));
  Serial.println(D);
  Serial.print(F("E: "));
  Serial.println(E);
  Serial.print(F("F: "));
  Serial.println(F);
}

void radarGetAngle(String dir) {
  float SmolDistanceL = 0, SmolDistanceR = 0;
  int SmolAngleL = 0, SmolAngleR = 0;
  SmolDistanceL = 30;
  SmolDistanceR = 30;
  for (int i = 0; i <= 10; i++) {
    int angle = i * 6;
    servo("radar", angle);
    ultrasonic("L");
    ultrasonic("R");
    Serial.print(F("DistanceL: "));
    Serial.println(distanceL);
    Serial.print(F("DistanceR: "));
    Serial.println(distanceR);
    if (distanceL < SmolDistanceL) {
      SmolDistanceL = distanceL;
      SmolAngleL = map(angle, 0, 60, 30, -30);
      Serial.println(".");
    }
    if (distanceR < SmolDistanceR) {
      SmolDistanceR = distanceR;
      SmolAngleR = map(angle, 0, 60, -30, 30);
      Serial.println(F("."));
    }
    Serial.print(F("angle: "));
    Serial.println(map(angle, 0, 60, 30, -30));
    servo("radar", 30);
  }
  if (dir == "double") {
    if (SmolDistanceR <= 0 || SmolDistanceL <= 0 || SmolAngleR > 30 || SmolAngleR < -30 || SmolAngleL > 30 || SmolAngleL < -30) {
      Serial.println(F("=============================="));
      Serial.println(F("=================TEST INVAID================"));
      Serial.println(F("=============================="));
      radarGetAngle("double");
    } else if (SmolAngleR + SmolAngleL > 12) {
      Serial.println(F("=============================="));
      Serial.print(F("SmolAngleL: "));
      Serial.println(SmolAngleL);
      Serial.print(F("DistanceL: "));
      Serial.println(SmolDistanceL);
      Serial.print(F("SmolAngleR: "));
      Serial.println(SmolAngleR);
      Serial.print(F("DistanceR: "));
      Serial.println(SmolDistanceR);
      Serial.println(F("=============================="));
      Serial.print(F("Error By: "));
      Serial.println(SmolAngleR + SmolAngleL);
      delay(200);
      radarGetAngle("double");
    } else SmolAngle = map(SmolAngleL, 30, -30, 0, 60);
  } else if (dir == "singleL") {
    if (SmolDistanceL <= 0 || SmolAngleL > 30 || SmolAngleL < -30) {
      Serial.println(F("=============================="));
      Serial.println(F("=================TEST INVAID================"));
      Serial.println(F("=============================="));
      radarGetAngle("singleL");
    }
  } else if (dir == "singleR") {
    if (SmolDistanceR <= 0 || SmolAngleR > 30 || SmolAngleR < -30) {
      Serial.println(F("=============================="));
      Serial.println(F("=================TEST INVAID================"));
      Serial.println(F("=============================="));
      radarGetAngle("singleR");
    }
  }

  Serial.println(F("===============TESTING COMPLETE==============="));
  Serial.print(F("SmolAngleL: "));
  Serial.println(SmolAngleL);
  Serial.print(F("DistanceL: "));
  Serial.println(SmolDistanceL);
  Serial.print(F("SmolAngleR: "));
  Serial.println(SmolAngleR);
  Serial.print(F("DistanceR: "));
  Serial.println(SmolDistanceR);
  Serial.print(F("SmolAngle: "));
  Serial.println(SmolAngleL);
  Serial.println(F("=============================="));
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
    delay(20);
    distance = (duration * 0.0340 / 2);
    if (distance >= 400) distance = 400;
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

void servo(String var, int angle) {
  int pulseWidth = map(angle, 0, 180, 544, 2400);
  int Pin = 0;
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

void LED(String dir) {
  pixels.clear();

  if (dir == "L") {
    pixels.setPixelColor(0, pixels.Color(140, 255, 0));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(140, 255, 0));
    Serial.println("L");
  } else if (dir == "R") {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(140, 255, 0));
    pixels.setPixelColor(2, pixels.Color(140, 255, 0));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    Serial.println(F("R"));

  } else if (dir == "B") {
    pixels.setPixelColor(0, pixels.Color(255, 255, 255));
    pixels.setPixelColor(1, pixels.Color(255, 255, 255));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    Serial.println(F("B"));
  } else if (dir == "F") {
    for (int i = 0; i < LEDCount; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
    Serial.println(F("F"));
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
  } else if (dir == "DetectF") {
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.setPixelColor(1, pixels.Color(0, 0, 255));
    pixels.setPixelColor(2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(3, pixels.Color(0, 255, 0));
  } else if (dir == "Start") {
    pixels.setPixelColor(0, pixels.Color(255, 0, 255));
    pixels.setPixelColor(1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(2, pixels.Color(0, 0, 255));
    pixels.setPixelColor(3, pixels.Color(255, 255, 255));
  }
  pixels.show();
  servo("grab", 30);
}

void report() {
  if (millis() >= reportPeriod) {
    reportPeriod = millis() + 3000;
    Serial.print(F("CounterL&CounterR=>"));
    Serial.print(counterL);
    Serial.print(F("----"));
    Serial.println(counterR);
    Serial.print(F("distanceF=>"));
    Serial.println(distanceF);
    Serial.println(F("///////////////////////////////"));
    Serial.print(F("distanceL=>"));
    Serial.println(distanceL);
    Serial.print(F("LB1Analog=>"));
    Serial.println(Mot_AnaB1);
    Serial.print(F("LB2Analog=>"));
    Serial.println(Mot_AnaB2);
    Serial.println(F("///////////////////////////////"));
    Serial.print(F("distanceR=>"));
    Serial.println(distanceR);
    Serial.print(F("RA1Analog=>"));
    Serial.println(Mot_AnaA1);
    Serial.print(F("RA2Analog=>"));
    Serial.println(Mot_AnaA2);
    Serial.println(F("================================"));
    Serial.print(F("Free Memory: "));
    Serial.println(freeMemory());
    Serial.println(F("================================"));
  }
}

void constforward() {
  while (distanceR <= BiggerMagicNumber || distanceF >= MagicNumber) {
    ultrasonic("L");
    ultrasonic("F");
    ultrasonic("R");
    if (distanceR >= BiggerMagicNumber) break;
    else if (distanceF <= MagicNumber) break;

    if (distanceR <= BiggerMagicNumber) {
      if (distanceR <= 8) {
        LED("WR");
        stateTurn = 1;
        if (distanceL + distanceR <= 35) {
          Mot_AnaA2 = map(distanceL, 20, 13, 230, 255);
          Mot_AnaB1 = map(distanceL, 13, 20, 200, 210);
        } else {
          Mot_AnaA2 = map(distanceR, 8, 2, 230, 255);
          Mot_AnaB1 = map(distanceR, 2, 8, 200, 210);
        }
      } else if (distanceR > 8 && distanceR < 13) {
        LED("WF");
        if (stateTurn != 0 && distanceL + distanceR <= 35) {
          analogWrite(Mot_A1, 0);
          analogWrite(Mot_A2, 0);
          analogWrite(Mot_B1, 0);
          analogWrite(Mot_B2, 0);
          radarGetAngle("double");
          rotate(SmolAngle);
          stateTurn = 0;
        }
        Mot_AnaA2 = map(distanceR, 9, 12, 218, 222);  //R
        Mot_AnaB1 = map(distanceR, 12, 9, 233, 237);  //L
      } else if (distanceR >= 13) {
        LED("WL");
        stateTurn = 1;
        if (distanceL + distanceR <= 35) {
          Mot_AnaA2 = map(distanceL, 8, 2, 200, 210);
          Mot_AnaB1 = map(distanceL, 2, 8, 230, 255);
        } else {
          Mot_AnaA2 = map(distanceR, 20, 13, 200, 210);
          Mot_AnaB1 = map(distanceR, 13, 20, 230, 255);
        }
      }
    } else {
      Mot_AnaA2 = 246;
      Mot_AnaB1 = 250;
    }
    if (Mot_AnaA2 >= 255) analogWrite(Mot_A2, 255);
    else if (Mot_AnaA2 < 200) analogWrite(Mot_A2, 200);
    if (Mot_AnaB1 >= 255) analogWrite(Mot_B1, 255);
    else if (Mot_AnaB1 < 200) analogWrite(Mot_B1, 200);
    analogWrite(Mot_A2, Mot_AnaA2);
    analogWrite(Mot_B1, Mot_AnaB1);
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
}

void constBackward() {
  if (backCali != true) {
    radarGetAngle("double");
    rotate(SmolAngle);
    backCali = true;
  }
  while (distanceL <= MagicNumber || distanceR <= MagicNumber) {
    ultrasonic("L");
    ultrasonic("R");
    Mot_AnaA1 = 175;  //R
    Mot_AnaB2 = 245;  //L
    analogWrite(Mot_A1, Mot_AnaA1);
    analogWrite(Mot_B2, Mot_AnaB2);
    if (distanceL >= BiggerMagicNumber) break;
    else if (distanceR >= BiggerMagicNumber) break;
    report();
  }

  analogWrite(Mot_A1, 0);
  analogWrite(Mot_B2, 0);
}

void forward(int stepsR, int stepsL) {
  delay(MagicDelay);
  Mot_AnaA2 = 0;
  Mot_AnaB1 = 0;
  counterL = 0;
  counterR = 0;
  stepsR = stepsR;
  stepsL = stepsL;
  while (stepsR > counterR && stepsL > counterL) {
    if (stepsR > counterR) {
      Mot_AnaA2 = 200;
    } else {
      Mot_AnaA2 = 0;
    }
    if (stepsL > counterL) {
      Mot_AnaB1 = 225;
    } else {
      Mot_AnaB1 = 0;
    }
    analogWrite(Mot_A2, Mot_AnaA2);
    analogWrite(Mot_B1, Mot_AnaB1);
    Serial.print(counterR);
    Serial.print(counterL);
  }
  Serial.println();
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  Mot_AnaA2 = 0;
  Mot_AnaB1 = 0;
  counterL = 0;
  counterR = 0;
}

void backward(int stepsR, int stepsL) {
  delay(MagicDelay);
  counterL = 0;
  counterR = 0;
  stepsR = stepsR * 2;
  stepsL = stepsL * 2;
  while (stepsR > counterR || stepsL > counterL) {
    if (stepsR > counterR) {
      Mot_AnaA1 = 200;
    } else {
      Mot_AnaA1 = 0;
    }
    if (stepsL > counterL) {
      Mot_AnaB2 = 225;
    } else {
      Mot_AnaB2 = 0;
    }
    analogWrite(Mot_A1, Mot_AnaA1);
    analogWrite(Mot_B2, Mot_AnaB2);
    Serial.print(counterR);
    Serial.print(counterL);
  }
  Serial.println();
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void left() {
  delay(MagicDelay);
  int steps = 35;
  counterL = 0;
  counterR = 0;
  while (steps > counterR) {
    if (steps > counterR) {
      analogWrite(Mot_A2, 235);
    } else {
      analogWrite(Mot_A2, 0);
    }
    Serial.print(counterR);
    Serial.print(counterL);
  }
  Serial.println();
  analogWrite(Mot_A2, 0);
  counterL = 0;
  counterR = 0;
}

void right() {
  delay(MagicDelay);
  int steps = 33;
  counterL = 0;
  counterR = 0;
  while (steps > counterL) {
    if (steps > counterL) {
      analogWrite(Mot_B1, 235);
    } else {
      analogWrite(Mot_B1, 0);
    }
    Serial.print(counterR);
    Serial.print(counterL);
  }
  Serial.println();
  analogWrite(Mot_B1, 0);
  counterL = 0;
  counterR = 0;
}

void rotate(int angle) {
  angle = map(angle, 0, 60, 30, -30);
  delay(MagicDelay);
  Serial.print(F("Rotate: "));
  Serial.println(angle);
  counterL = 0;
  counterR = 0;
  if (angle > 0) {  //Making Right Rotate
    int steps = map(angle, 0, 30, 0, 5);
    while (steps > counterR && steps > counterL) {
      if (steps > counterR) {
        analogWrite(Mot_A1, 220);
      } else {
        analogWrite(Mot_A1, 0);
      }
      if (steps > counterL) {
        analogWrite(Mot_B1, 255);
      } else {
        analogWrite(Mot_B1, 0);
      }
      Serial.print(counterR);
      Serial.print(counterL);
    }
    Serial.println();
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_B1, 0);
  } else {
    int steps = map(angle, -30, 0, 5, 0);
    while (steps > counterR && steps > counterL) {
      if (steps > counterR) {
        analogWrite(Mot_A2, 230);
      } else {
        analogWrite(Mot_A2, 0);
      }
      if (steps > counterL) {
        analogWrite(Mot_B2, 230);
      } else {
        analogWrite(Mot_B2, 0);
      }
      Serial.print(counterR);
      Serial.print(counterL);
    }
    Serial.println();
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B2, 0);
  }
}

void turncalibrate(String advancedir, String dir) {
  if (advancedir == "forward") {
    analogWrite(Mot_A1, 220);
    analogWrite(Mot_B2, 255);
    if (dir == "left") {
      while (distanceL >= BiggerMagicNumber) {
        ultrasonic("L");
      }
    } else if (dir == "right") {
      while (distanceR >= BiggerMagicNumber) {
        ultrasonic("R");
      }
    }
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_B2, 0);
    delay(200);
    ultrasonic("F");
    if (distanceF <= 30) {
      LED("Detect");
      analogWrite(Mot_A2, 220);
      analogWrite(Mot_B1, 255);
      while (distanceF > 6) {
        ultrasonic("F");
      }
    } else {
      LED("F");
      forward(40, 40);
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    delay(MagicDelay);
    return;
  } else if (advancedir == "backward") {
    int Mot_AlaR = 175;
    int Mot_AlaL = 245;
    analogWrite(Mot_A2, Mot_AlaR);
    analogWrite(Mot_B1, Mot_AlaL);
    while (distanceL >= BiggerMagicNumber) {
      ultrasonic("L");
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    delay(MagicDelay);
    backward(10, 10);
    return;
  } else {
    LED("DetectF");
    analogWrite(Mot_A2, 220);
    analogWrite(Mot_B1, 255);
    if (dir == "left") {
      while (distanceL >= BiggerMagicNumber) {
        ultrasonic("L");
      }
    } else if (dir == "right") {
      while (distanceR >= BiggerMagicNumber) {
        ultrasonic("R");
      }
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    delay(MagicDelay);
    forward(24, 24);
    return;
  }
}