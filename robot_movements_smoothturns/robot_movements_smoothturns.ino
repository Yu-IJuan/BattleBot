#include <DHT11.h>

//PIN (Digital)
const int Mot_A1 = 9, Mot_A2 = 6;   //Right Motor (PWM)
const int Mot_B1 = 5, Mot_B2 = 11;  //Left Motor (PWM)
const int Mot_R1 = 3, Mot_R2 = 2;   //RPM Sensor (R/L)
const int trig = 12, echo;          //Ultrasonic
const int Grab = 4;                 //Claw
const int ServoMot = 10;            //Neck Servo
//

//PIN (Analog)
const int D1 = A0, D2 = A1, D3 = A2, D4 = A3;
const int D5 = A4, D6 = A5, D7 = A6, D8 = A7;  //For IR B&W Sensor
//
const int MagicNumber = 40, MagicDelay = 100;

unsigned int counterL, counterR;

const int diskslot = 20;
const float dia = 6.52;

int distance, distanceF, distanceL, distanceR, temp;

unsigned long currenttime;  //For millis() functions
unsigned long duration;     //For Ultrasonic functions
DHT11 dht11(1);             //DHT's PIN

void setup() {
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
  pinMode(ServoMot, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(13, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  pinMode(Grab, OUTPUT);

  temp = dht11.readTemperature();
  // put your setup code here, to run once:
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
  // ultrasonic(8);
  // distanceF = distance;
  // Serial.println(distanceF);
  // constforward();
  // delay(5000);
  // Serial.println("Right");
  // right();
  // delay(3000);
  // uturn();
  // delay(3000);
  // constBackward();
  left();
  delay(3000);


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

void ultrasonic(int echo) {
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * (0.0331 + 0.0006 * temp) / 2;
  Serial.println("Ultra");
  if (distance >= 400) {
    distance = 1;  //Maximum effective range of HC-SR04 is 400CM
  }
}

void constforward() {
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  int steps;
  counterL = 0;
  counterR = 0;
  steps = 4;
  while (distanceF >= 20) {
    if (steps > counterR || steps > counterL) {
      if (steps > counterR) {
        analogWrite(Mot_A1, 0);
        analogWrite(Mot_A2, 254);
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
    }
    steps = steps + 4;
    ultrasonic(8);
    distanceF = distance;
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
  detachInterrupt(digitalPinToInterrupt(Mot_R1));
  detachInterrupt(digitalPinToInterrupt(Mot_R2));
}

void forward(int steps) {
  counterL = 0;
  counterR = 0;
  while (steps > counterR || steps > counterL) {
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

void constBackward() {
  int Mot_AlaR = 253;
  int Mot_AlaL = 255;
  analogWrite(Mot_A1, Mot_AlaR);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, Mot_AlaL);
  while (distanceL <= MagicNumber) {
    ultrasonic(7);
    distanceL = distance;
    if (distanceL >= MagicNumber) break;
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
}

void left() {
  int steps = 8;
  counterL = 0;
  counterR = 0;
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  while (steps > counterR || steps > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 205);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps > counterL) {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 205);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 205);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 205);
  delay(41);
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
  detachInterrupt(digitalPinToInterrupt(Mot_R1));
  detachInterrupt(digitalPinToInterrupt(Mot_R2));
}

void right() {
  int steps = 8;
  counterL = 0;
  counterR = 0;
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  while (steps > counterR || steps > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 205);
      analogWrite(Mot_A2, 0);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps > counterL) {
      analogWrite(Mot_B1, 205);
      analogWrite(Mot_B2, 0);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
  }
  analogWrite(Mot_A1, 205);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 205);
  analogWrite(Mot_B2, 0);
  delay(41);
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
  detachInterrupt(digitalPinToInterrupt(Mot_R1));
  detachInterrupt(digitalPinToInterrupt(Mot_R2));
}
void uturn() {
  int steps = 17;
  counterL = 0;
  counterR = 0;
  while (steps > counterR || steps - 1 > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 220);
      analogWrite(Mot_A2, 0);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps - 1 > counterL) {
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
