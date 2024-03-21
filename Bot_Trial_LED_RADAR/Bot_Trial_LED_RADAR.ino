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

Adafruit_NeoPixel pixels(LEDCount, Neo, NEO_GRB + NEO_KHZ800);

unsigned long currenttime = 0, refreshtime = 0, reportPeriod = 0;  //For millis() functions
int duration = 0, distance = 0;                                    //For Ultrasonic functions
float distancePL = 0, distancePR = 0;
unsigned int counterR = 0, counterL = 0;
int distanceR = 0, distanceL = 0, distanceF = 0;
int SmolAngle = 0;


int pulseWidth = 0;

bool stateServo = false, ranOnce = false;
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

  pinMode(trigLR, OUTPUT);
  pinMode(trigF, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(echoF, INPUT);
  pinMode(echoR, INPUT);

  pinMode(Grab, OUTPUT);
  pinMode(Radar, OUTPUT);

  pixels.begin();
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, CHANGE);
}

void loop() {
  if (!started) {
    Serial.println("starting");
    LED("Start");
    servo("grab", 80);
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
        distanceR = distance;
        ultrasonic("F");
        distanceF = distance;
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
}

void ISR_L() {
  counterL++;
}

void followLine(String dir) {
  readAnalogs();
  analogWrite(Mot_A2, 213);
  analogWrite(Mot_B1, 210);
  Serial.println("followLine");
  if (dir == "before") {
    while (B > Black || C > Black || D > Black || E > Black && A <= White && F < White) {
      readAnalogs();
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    Serial.println("Before");
    delay(1500);
    servo("grab", 30);
    delay(1000);
    left();
    delay(1000);
    forward(6, 6);
    followLine("during");
  } else if ("during") {
    while (A > Black || B > Black || C > Black || D > Black || E > Black || F > Black) {
      readAnalogs();
      Serial.println("During");
      if (C > Black && D > Black) {
        analogWrite(Mot_A2, 247);
        analogWrite(Mot_B1, 250);
      } else if (B > Black || C > Black) {
        analogWrite(Mot_A2, 242);
        analogWrite(Mot_B1, 250);
      } else if (D > Black || E > Black) {
        analogWrite(Mot_A2, 246);
        analogWrite(Mot_B1, 254);
      }
    }
  }
}

void calibrateLight() {
  int tempBlack = 0, tempWhite = 0;
  int i = 0;
  while (i < 5) {
    analogWrite(Mot_A2, 226);
    analogWrite(Mot_B1, 220);
    Serial.println(i);
    int average = (A + B + C + D + E + F) / 6;
    readAnalogs();
    if (C > 600 && D > 600 && !BnWstate) {
      tempBlack = tempBlack + average;
      BnWstate = !BnWstate;
      i++;
    } else if (C < 600 && D < 600 && BnWstate) {
      tempWhite = tempWhite + average;
      BnWstate = !BnWstate;
      i++;
    }
  }

  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  White = tempWhite / 2 + 200;
  Black = tempBlack / 2 - 200;
  Serial.print("White: ");
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

void radarGetAngle(String dir) {
  float SmolDistanceL = 0, SmolDistanceR = 0;
  int SmolAngleL = 0, SmolAngleR = 0;
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
    servo("radar", 30);
  }
  if (dir == "double") {
    if (SmolDistanceR <= 0 || SmolDistanceL <= 0 || SmolAngleR > 30 || SmolAngleR < -30 || SmolAngleL > 30 || SmolAngleL < -30) {
      Serial.println("==============================");
      Serial.println("=================TEST INVAID================");
      Serial.println("==============================");
      radarGetAngle("double");
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
      delay(200);
      radarGetAngle("double");
    } else SmolAngle = map(SmolAngleL, 30, -30, 0, 60);
  } else if (dir == "singleL") {
    if (SmolDistanceL <= 0 || SmolAngleL > 30 || SmolAngleL < -30) {
      Serial.println("==============================");
      Serial.println("=================TEST INVAID================");
      Serial.println("==============================");
      radarGetAngle("singleL");
    }
  } else if (dir == "singleR") {
    if (SmolDistanceR <= 0 || SmolAngleR > 30 || SmolAngleR < -30) {
      Serial.println("==============================");
      Serial.println("=================TEST INVAID================");
      Serial.println("==============================");
      radarGetAngle("singleR");
    }
  }

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
  Serial.println(SmolAngleL);
  Serial.println("==============================");
}

void ultrasonic(String dir) {
  float distanceP = 0;
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
    Serial.println("R");

  } else if (dir == "B") {
    pixels.setPixelColor(0, pixels.Color(255, 255, 255));
    pixels.setPixelColor(1, pixels.Color(255, 255, 255));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    Serial.println("B");
  } else if (dir == "F") {
    for (int i = 0; i < Neo; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
    Serial.println("F");
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
  servo("radar", 30);
  servo("grab", 30);
}

void report() {
  if (millis() >= reportPeriod) {
    reportPeriod = millis() + 3000;
    Serial.print("CounterL&CounterR=>");
    Serial.print(counterL);
    Serial.print("----");
    Serial.println(counterR);
    Serial.print("distanceF=>");
    Serial.println(distanceF);
    Serial.println("///////////////////////////////");
    Serial.print("distanceL=>");
    Serial.println(distanceL);
    Serial.print("LB1Analog=>");
    Serial.println(Mot_AnaB1);
    Serial.print("LB2Analog=>");
    Serial.println(Mot_AnaB2);
    Serial.println("///////////////////////////////");
    Serial.print("distanceR=>");
    Serial.println(distanceR);
    Serial.print("RA1Analog=>");
    Serial.println(Mot_AnaA1);
    Serial.print("RA2Analog=>");
    Serial.println(Mot_AnaA2);
    Serial.println("================================");
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
          Mot_AnaB1 = map(distanceL, 13, 20, 210, 220);
        } else {
          Mot_AnaA2 = map(distanceR, 8, 2, 230, 255);
          Mot_AnaB1 = map(distanceR, 2, 8, 210, 220);
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
        Mot_AnaA2 = map(distanceR, 9, 12, 241, 255);  //R
        Mot_AnaB1 = map(distanceR, 12, 9, 243, 253);  //L
      } else if (distanceR >= 13) {
        LED("WL");
        stateTurn = 2;
        if (distanceL + distanceR <= 35) {
          Mot_AnaA2 = map(distanceL, 8, 2, 210, 220);
          Mot_AnaB1 = map(distanceL, 2, 8, 230, 255);
        } else {
          Mot_AnaA2 = map(distanceR, 20, 13, 210, 220);
          Mot_AnaB1 = map(distanceR, 13, 20, 230, 255);
        }
      }
    } else {
      Mot_AnaA2 = 246;
      Mot_AnaB1 = 250;
    }
    if (Mot_AnaA2 >= 255) Mot_AnaA2 = 255;
    else if (Mot_AnaA2 < 210) Mot_AnaA2 = 210;
    if (Mot_AnaB1 >= 255) Mot_AnaB1 = 255;
    else if (Mot_AnaB1 < 210) Mot_AnaB1 = 210;
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, Mot_AnaA2);
    analogWrite(Mot_B1, Mot_AnaB1);
    analogWrite(Mot_B2, 0);
    report();
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
    Mot_AnaA1 = 244;  //R
    Mot_AnaB2 = 250;  //L
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
  while (stepsR > counterR || stepsL > counterL) {
    if (stepsR > counterR) {
      Mot_AnaA2 = 246;
    } else {
      Mot_AnaA2 = 0;
    }
    if (stepsL > counterL) {
      Mot_AnaB1 = 250;
    } else {
      Mot_AnaB1 = 0;
    }
    analogWrite(Mot_A2, Mot_AnaA2);
    analogWrite(Mot_B1, Mot_AnaB1);
    Serial.print(counterL);
    Serial.print("----");
    Serial.print(counterR);
  }
  Serial.println(" ");
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
      Mot_AnaA1 = 244;
    } else {
      Mot_AnaA1 = 0;
    }
    if (stepsL > counterL) {
      Mot_AnaB2 = 250;
    } else {
      Mot_AnaB2 = 0;
    }
    analogWrite(Mot_A1, Mot_AnaA1);
    analogWrite(Mot_B2, Mot_AnaB2);
    Serial.print(counterL);
    Serial.print("----");
    Serial.print(counterR);
  }
  Serial.println(" ");
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void left() {
  delay(MagicDelay);
  int steps = 34;
  counterL = 0;
  counterR = 0;
  while (steps > counterR) {
    if (steps > counterR) {
      Mot_AnaA2 = 215;
    } else {
      Mot_AnaA2 = 0;
    }
    analogWrite(Mot_A2, Mot_AnaA2);
    Serial.print(counterL);
    Serial.print("----");
    Serial.print(counterR);
  }
  Serial.println(" ");
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
      Mot_AnaB1 = 210;
    } else {
      Mot_AnaB1 = 0;
    }
    analogWrite(Mot_B1, Mot_AnaB1);
    Serial.print(counterL);
    Serial.print("----");
    Serial.print(counterR);
  }
  Serial.println(" ");
  analogWrite(Mot_B1, 0);
  counterL = 0;
  counterR = 0;
}

void rotate(int angle) {
  angle = map(angle, 0, 60, 30, -30);
  delay(MagicDelay);
  Serial.print("Rotate: ");
  Serial.println(angle);
  counterL = 0;
  counterR = 0;
  if (angle > 0) {  //Making Right Rotate
    int steps = map(angle, 0, 30, 0, 5);
    while (steps > counterR || steps > counterL) {
      if (steps > counterR) {
        Mot_AnaA1 = 230;
      } else {
        Mot_AnaA1 = 0;
      }
      if (steps > counterL) {
        Mot_AnaB1 = 230;
      } else {
        Mot_AnaB1 = 0;
      }
      analogWrite(Mot_A1, Mot_AnaA1);
      analogWrite(Mot_B1, Mot_AnaB1);
      Serial.print(counterL);
      Serial.print("----");
      Serial.println(counterR);
    }
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);
  } else {
    int steps = map(angle, -30, 0, 5, 0);
    while (steps > counterR || steps > counterL) {
      if (steps > counterR) {
        Mot_AnaA2 = 230;
      } else {
        Mot_AnaA2 = 0;
      }
      if (steps > counterL) {
        Mot_AnaB2 = 230;
      } else {
        Mot_AnaB2 = 0;
      }
      analogWrite(Mot_A2, Mot_AnaA2);
      analogWrite(Mot_B2, Mot_AnaB2);
      Serial.print(counterL);
      Serial.print("----");
      Serial.println(counterR);
    }
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);
  }
}

void turncalibrate(String advancedir, String dir) {
  if (advancedir == "forward") {
    int Mot_AlaR = 246;
    int Mot_AlaL = 250;
    analogWrite(Mot_A1, Mot_AlaR);
    analogWrite(Mot_B2, Mot_AlaL);
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
      analogWrite(Mot_A2, 248);
      analogWrite(Mot_B1, 250);
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
    int Mot_AlaR = 246;
    int Mot_AlaL = 250;
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
    analogWrite(Mot_A2, 246);
    analogWrite(Mot_B1, 250);
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