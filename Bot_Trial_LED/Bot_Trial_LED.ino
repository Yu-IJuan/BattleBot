#include <Adafruit_NeoPixel.h>

//end of forward still drifting to right

//PIN (Digital)
#define Mot_A1 9
#define Mot_A2 6  //Right Motor (PWM)
#define Mot_B1 5
#define Mot_B2 11  //Left Motor (PWM)
#define Mot_R1 3
#define Mot_R2 2  //RPM Sensor
int trig, echo;   //Ultrasonic
#define Grab 4    //Claw
#define Neo 10    //LED pin
//7 ,8 ,13 Reserved for ultrasonic sensors' echo pins
//12, A5 Reserved for ultrasonic sensors' trig pins

#define MagicNumber 15
#define BiggerMagicNumber 30
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

unsigned long currenttime, refreshtime, reportPeriod;  //For millis() functions
int duration, distance;                                //For Ultrasonic functions
unsigned int counterR, counterL;

int distanceR, distanceL, distanceF;
int pulseWidth;

bool stateServo = false, ranOnce = false;
int stateTurn;

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

  pinMode(12, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(13, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  pinMode(Grab, OUTPUT);
  //  for (int i; i < 50; i++) moving(20);
  //  for (int i; i < 50; i++) moving(70);
  pixels.begin();
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
}

void ISR_R() {
  counterR++;
}

void ISR_L() {
  counterL++;
}

void loop() {
  if (!stateServo) {
    moving(90);
    delay(200);
    stateServo = true;
    report();
  }

  ultrasonic(13);
  distanceR = distance;
  ultrasonic(8);  //Checking with different ultrasonic sensors,
  distanceF = distance;
  ultrasonic(7);         //But they all share same trigger pin,
  distanceL = distance;  //So it saves pin resources.

  moving(30);

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
    constBackward();  //Backward if no route available
    delay(MagicDelay);
    ultrasonic(7);
    distanceL = distance;
    ultrasonic(13);
    distanceR = distance;
    if (distanceL >= BiggerMagicNumber) {
      LED("L");
      turncalibrate("backward", "default");
      delay(MagicDelay);
      left();
      delay(MagicDelay);
      LED("F");
      forward(12, 12);
      delay(MagicDelay);
    } else if (distanceR >= BiggerMagicNumber) {
      LED("R");
      right();
      delay(MagicDelay);
      ultrasonic(13);
      distanceR = distance;
      ultrasonic(8);
      distanceF = distance;
      if (distanceR >= BiggerMagicNumber) {
        LED("R");
        right();
        delay(MagicDelay);
        LED("F");
        forward(24, 24);
        delay(MagicDelay);
      } else if (distanceF >= BiggerMagicNumber) {
        LED("F");
        forward(24, 24);
        delay(MagicDelay);
      }
    }
  }
}

void moving(int angle) {
  int pulseWidth = map(angle, 0, 180, 544, 2400);

  for (int i; i < 10; i++) {
    digitalWrite(Grab, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(Grab, LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}


void ultrasonic(int echo) {
  if (echo == 7 || echo == 13) trig = A5;
  else if (echo == 8) trig = 12;
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  delay(50);
  distance = (duration * 0.0340 / 2);
  if (distance >= 400) distance = 1;  //Maximum effective range of HC-SR04 is 400CM
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
  }
  pixels.show();
  moving(30);
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
    ultrasonic(7);
    distanceL = distance;
    ultrasonic(8);
    distanceF = distance;
    ultrasonic(13);
    distanceR = distance;
    if (distanceR >= BiggerMagicNumber) break;
    else if (distanceF <= MagicNumber) break;

    if (distanceR <= BiggerMagicNumber) {
      if (distanceR <= 6) {
        LED("WR");
        stateTurn = 1;
        if (distanceL + distanceR <= 35) {
          Mot_AnaA2 = map(distanceL, 14, 10, 230, 255);
          Mot_AnaB1 = map(distanceL, 10, 14, 210, 220);
        } else {
          Mot_AnaA2 = map(distanceR, 6, 2, 230, 255);
          Mot_AnaB1 = map(distanceR, 2, 6, 210, 220);
        }
      } else if (distanceR > 6 && distanceR < 10) {
        LED("WF");
        Mot_AnaA2 = map(distanceR, 7, 9, 241, 255);  //R
        Mot_AnaB1 = map(distanceR, 9, 7, 243, 253);  //L
      } else if (distanceR >= 10) {
        LED("WL");
        stateTurn = 2;
        if (distanceL + distanceR <= 35) {
          Mot_AnaA2 = map(distanceL, 6, 2, 210, 220);
          Mot_AnaB1 = map(distanceL, 2, 6, 230, 255);
        } else {
          Mot_AnaA2 = map(distanceR, 14, 10, 210, 220);
          Mot_AnaB1 = map(distanceR, 10, 14, 230, 255);
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
  while (distanceL <= MagicNumber || distanceR <= MagicNumber) {
    ultrasonic(7);
    distanceL = distance;
    ultrasonic(13);
    distanceR = distance;
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
  LED("F");
  Mot_AnaA1 = 0;
  Mot_AnaA2 = 0;
  Mot_AnaB1 = 0;
  Mot_AnaB2 = 0;
  counterL = 0;
  counterR = 0;
  while (stepsR > counterR || stepsL > counterL) {
    if (stepsR > counterR) {
      Mot_AnaA2 = 230;
    } else {
      Mot_AnaA1 = 100;
      Mot_AnaA2 = 0;
    }
    if (stepsL > counterL) {
      Mot_AnaB1 = 234;
    } else {
      Mot_AnaB1 = 0;
      Mot_AnaB2 = 100;
    }

    analogWrite(Mot_A2, Mot_AnaA2);
    analogWrite(Mot_B1, Mot_AnaB1);
    analogWrite(Mot_A1, Mot_AnaA1);
    analogWrite(Mot_B2, Mot_AnaB2);
    report();
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_B2, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  Mot_AnaA1 = 0;
  Mot_AnaA2 = 0;
  Mot_AnaB1 = 0;
  Mot_AnaB2 = 0;
  counterL = 0;
  counterR = 0;
}

void backward(int stepsR, int stepsL) {
  counterL = 0;
  counterR = 0;
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
    report();
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void left() {
  delay(MagicDelay);
  int steps = 16;
  counterL = 0;
  counterR = 0;
  while (steps > counterR) {
    if (steps > counterR) {
      Mot_AnaA2 = 215;
    } else {
      Mot_AnaA2 = 0;
    }
    analogWrite(Mot_A2, Mot_AnaA2);
    report();
  }
  analogWrite(Mot_A2, 210);
  delay(88);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void right() {
  delay(MagicDelay);
  int steps = 15;
  counterL = 0;
  counterR = 0;
  while (steps > counterL) {
    if (steps > counterL) {
      Mot_AnaB1 = 210;
    } else {
      Mot_AnaB1 = 0;
    }
    analogWrite(Mot_B1, Mot_AnaB1);
    report();
  }
  analogWrite(Mot_B1, 210);
  delay(88);
  analogWrite(Mot_B1, 0);
  counterL = 0;
  counterR = 0;
}

void turncalibrate(String advancedir, String dir) {
  if (advancedir == "forward") {
    int Mot_AlaR = 245;
    int Mot_AlaL = 252;
    analogWrite(Mot_A1, Mot_AlaR);
    analogWrite(Mot_B2, Mot_AlaL);
    if (dir == "left") {
      while (distanceL >= BiggerMagicNumber) {
        ultrasonic(7);
        distanceL = distance;
      }
    } else if (dir == "right") {
      while (distanceR >= BiggerMagicNumber) {
        ultrasonic(13);
        distanceR = distance;
      }
    }
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_B2, 0);
    delay(200);
    ultrasonic(8);
    distanceF = distance;
    if (distanceF <= 30) {
      LED("Detect");
      analogWrite(Mot_A2, 246);
      analogWrite(Mot_B1, 250);
      while (distanceF > 14) {
        ultrasonic(8);
        distanceF = distance;
      }
    } else {
      LED("F");
      forward(14, 14);
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    delay(MagicDelay);
    return;
  } else if (advancedir == "backward") {
    int Mot_AlaR = 250;
    int Mot_AlaL = 253;
    analogWrite(Mot_A2, Mot_AlaR);
    analogWrite(Mot_B1, Mot_AlaL);
    while (distanceL >= BiggerMagicNumber) {
      ultrasonic(7);
      distanceL = distance;
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    delay(MagicDelay);
    backward(18, 18);
    return;
  } else {
    LED("DetectF");
    analogWrite(Mot_A2, 246);
    analogWrite(Mot_B1, 250);
    if (dir == "left") {
      while (distanceL >= BiggerMagicNumber) {
        ultrasonic(7);
        distanceL = distance;
      }
    } else if (dir == "right") {
      while (distanceR >= BiggerMagicNumber) {
        ultrasonic(13);
        distanceR = distance;
      }
    }
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    delay(MagicDelay);
    forward(12, 12);
    return;
  }
}
