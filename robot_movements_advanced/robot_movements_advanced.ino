#include <DHT11.h>

//PIN (Digital)
const int Mot_A1 = 9, Mot_A2 = 6;   //Right Motor (PWM)
const int Mot_B1 = 5, Mot_B2 = 11;  //Left Motor (PWM)
const int Mot_R1 = 3, Mot_R2 = 2;   //RPM Sensor (R/L)
const int trig = 12, echo1 = 13, echo2 = 7, echo3 = 8;     //Ultrasonic
const int Grab = 4;                 //Claw
const int ServoMot = 10;            //Neck Servo
//

//PIN (Analog)
const int D1 = A0, D2 = A1, D3 = A2, D4 = A3;
const int D5 = A4, D6 = A5, D7 = A6, D8 = A7;  //For IR B&W Sensor
//

unsigned int counterL, counterR;

const int diskslot = 20;
const float dia = 6.52;

unsigned long currenttime;         //For millis() functions
unsigned long duration, distance;  //For Ultrasonic functions

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  Serial.begin(9600);
}

void ISR_R() {
  counterR++;
}

void ISR_L() {
  counterL++;
}

void loop() {
  //   Serial.println("Forward");
  //   forward(25);
  //   delay(1000);
  //   Serial.println("Backward");
  //   backward(25);
  //   delay(1000);
  // Serial.println("Left");
  // left();
  // delay(3000);
  Serial.println("Right");
  right();
  delay(3000);
  // uturn();
  // delay(3000);


  // analogWrite(Mot_A1, 255);
  // analogWrite(Mot_A2, 0);
  // analogWrite(Mot_B1, 255);
  // analogWrite(Mot_B2, 0);
  // //  Moving left
  // analogWrite(Mot_A1, 0);
  // analogWrite(Mot_A2, 255);
  // analogWrite(Mot_B1, 0);
  // analogWrite(Mot_B2, 255);
}

void forward(int steps) {
  counterL = 0;
  counterR = 0;
  while (steps > counterR && steps > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 255);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps > counterL) {
      analogWrite(Mot_B1, 250);
      analogWrite(Mot_B2, 0);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
    Serial.println(counterL);
    Serial.println(counterR);
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void backward(int steps) {
  counterL = 0;
  counterR = 0;
  while (steps > counterR && steps > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 249);
      analogWrite(Mot_A2, 0);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps > counterL) {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 255);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
    Serial.println(counterL);
    Serial.println(counterR);
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void left() {
  int steps = 8;
  counterL = 0;
  counterR = 0;
  while (steps > counterR || steps > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 225);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps > counterL) {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 230);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
    Serial.println(counterL);
    Serial.println(counterR);
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void right() {
  int steps = 8;
  counterL = 0;
  counterR = 0;
  while (steps > counterR || steps > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 225);
      analogWrite(Mot_A2, 0);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps > counterL) {
      analogWrite(Mot_B1, 225);
      analogWrite(Mot_B2, 0);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
    Serial.println(counterL);
    Serial.println(counterR);
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void uturn() {
  int steps = 17;
  counterL = 0;
  counterR = 0;
  while (steps > counterR || steps -1 > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 220);
      analogWrite(Mot_A2, 0);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps -1> counterL) {
      analogWrite(Mot_B1, 220);
      analogWrite(Mot_B2, 0);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
    Serial.println(counterL);
    Serial.println(counterR);
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}
