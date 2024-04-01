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
#define Neo 4   //LED pin

#define LEDCount 4 //For NeoPixels

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
bool starting = false, BnWstate = false, ending = false;
int Black = 0, White = 0;


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
  //Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!starting) {
    LEDbeforestart();
    LED("Start");
    servo("grab", 90);
    delay(1000);
    calibrateLight();
    followLine("before");
    starting = true;
  } else if (starting && !ending) {

  } else {

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
  Serial.print("White: ");
  Serial.println(White);
  Serial.print("Black: ");
  Serial.println(Black);
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
      } else if (B > Black && C > Black) {
        analogWrite(Mot_A2, 230);
        analogWrite(Mot_B1, 255);
      } else if (D > Black && E > Black) {
        analogWrite(Mot_A2, 210);
        analogWrite(Mot_B1, 255);
      } else if (B > Black) {
        analogWrite(Mot_A2, 240);
        analogWrite(Mot_B1, 255);
      } else if (E > Black) {
        analogWrite(Mot_A2, 200);
        analogWrite(Mot_B1, 255);
      }
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
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
      if (C > Black && D > Black) {
        analogWrite(Mot_A2, 220);
        analogWrite(Mot_B1, 255);
      } else if (B > Black && C > Black) {
        analogWrite(Mot_A2, 230);
        analogWrite(Mot_B1, 255);
      } else if (D > Black && E > Black) {
        analogWrite(Mot_A2, 210);
        analogWrite(Mot_B1, 255);
      } else if (B > Black) {
        analogWrite(Mot_A2, 240);
        analogWrite(Mot_B1, 255);
      } else if (E > Black) {
        analogWrite(Mot_A2, 200);
        analogWrite(Mot_B1, 255);
      } else if (A > Black) {
        analogWrite(Mot_A2, 220);
        analogWrite(Mot_B1, 0);
      } else if (F > Black) {
        analogWrite(Mot_A2, 0);
        analogWrite(Mot_B1, 255);
      } else break;
    }
  } else if (dir == "ending") {
    
  }
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
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
  }
  Serial.println();
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
  }
  pixels.show();
  servo("grab", 30);
}