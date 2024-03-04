#include <DHT11.h>
#include <U8g2lib.h>
#include <Wire.h>


// Backward Left UNDERPOWERED
// Yu-I 12:40

//PIN (Digital)
const int Mot_A1 = 9, Mot_A2 = 6;   //Right Motor (PWM)
const int Mot_B1 = 5, Mot_B2 = 11;  //Left Motor (PWM)
const int Mot_R1 = 3, Mot_R2 = 2;   //RPM Sensor
const int trig = 12, echo;          //Ultrasonic
const int Grab = 4;                 //Claw
//7 ,8 ,13 Reserved for ultrasonic sensors' echo pins
//10, Free


const int MagicNumber = 35, MagicDelay = 300;

//PIN (Analog)
const int D1 = A0, D3 = A1, D4 = A2;
const int D5 = A3, D6 = A6, D8 = A7;  //For IR B&W Sensor
//

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
DHT11 dht11(1);  //DHT's PIN
int temp;        //For recording Temp

unsigned long currenttime, refreshtime;  //For millis() functions
int duration, distance;                  //For Ultrasonic functions
unsigned int counterR, counterL;

int distanceR, distanceL, distanceF;
int pulseWidth;

bool stateServo = false;

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_8x13O_tr);
  u8g2.setCursor(0, 0);
  u8g2.print("Booting...");
  u8g2.sendBuffer();
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

  pinMode(trig, OUTPUT);
  pinMode(13, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  pinMode(Grab, OUTPUT);
  //  for (int i; i < 50; i++) moving(20);
  //  for (int i; i < 50; i++) moving(70);
  temp = dht11.readTemperature();
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
  }
  moving(30);


  if (millis() >= currenttime) {
    currenttime = millis() + MagicNumber;
    ultrasonic(13);
    distanceR = distance;
    ultrasonic(8);  //Checking with different ultrasonic sensors,
    distanceF = distance;
    ultrasonic(7);         //But they all share same trigger pin,
    distanceL = distance;  //So it saves pin resources.
  }
  if (millis() >= refreshtime) {
    refreshtime = millis() + MagicDelay;
    screen("Default");
  }
  if (distanceR >= MagicNumber) {
    screen("R");
    turncalibrate("forward", "right");
    delay(MagicDelay);
    right();  //90 Degrees turn if left empty
    screen("F");
    delay(MagicDelay);
    forward(24, 24);
    delay(MagicDelay);
  } else if (distanceF >= MagicNumber) {
    screen("F");
    constforward();  //In center then walk straight
    delay(MagicDelay);
  } else if (distanceL >= MagicNumber) {
    screen("L");
    turncalibrate("forward", "left");
    delay(MagicDelay);
    left();  //90 Degrees turn if right empty
    screen("F");
    delay(MagicDelay);
    forward(24, 24);
    delay(MagicDelay);
  } else {
    screen("B");
    constBackward();  //Backward if no route available
    delay(MagicDelay);
    ultrasonic(7);
    distanceL = distance;
    ultrasonic(13);
    distanceR = distance;
    if (distanceL >= MagicNumber) {
      screen("L");
      turncalibrate("backward", "default");
      delay(MagicDelay);
      left();
      delay(MagicDelay);
      screen("F");
      forward(24, 24);
      delay(MagicDelay);
    } else if (distanceR >= MagicNumber) {
      screen("R");
      right();
      delay(MagicDelay);
      ultrasonic(13);
      distanceR = distance;
      ultrasonic(8);
      distanceF = distance;
      if (distanceR >= MagicNumber) {
        screen("R");
        right();
        delay(MagicDelay);
        screen("F");
        forward(24, 24);
        delay(MagicDelay);
      } else if (distanceF >= MagicNumber) {
        screen("F");
        forward(24, 24);
        delay(MagicDelay);
      }
    }
  }
}


void ultrasonic(int echo) {
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  delay(50);
  distance = duration * (0.0331 + 0.0006 * temp) / 2;
  if (distance >= 400) distance = 1;  //Maximum effective range of HC-SR04 is 400CM
}

void screen(String dir) {
  u8g2.clearBuffer();
  u8g2.setCursor(100, 15);
  if (dir == "L") u8g2.print("L");
  else if (dir == "R") u8g2.print("R");
  else if (dir == "B") u8g2.print("B");
  else if (dir == "F") u8g2.print("F");
  u8g2.setCursor(15, 10);
  u8g2.print("Front:");
  u8g2.print(distanceF);
  u8g2.setCursor(15, 25);
  u8g2.print("Right:");
  u8g2.print(distanceR);
  u8g2.setCursor(15, 40);
  u8g2.print("Left:");
  u8g2.print(distanceL);
  u8g2.sendBuffer();
  moving(30);
}

void constforward() {
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  int Mot_AlaR = 245;
  int Mot_AlaL = 250;
  screen("F");
  int stepsL = 4;
  int stepsR = 4;
  counterL = 0;
  counterR = 0;
  while (distanceF >= MagicNumber) {
    while (stepsR - 1 > counterR || stepsL > counterL) {
      if (stepsR - 1 > counterR) {
        analogWrite(Mot_A1, 0);
        analogWrite(Mot_A2, 246);
      } else {
        analogWrite(Mot_A1, 0);
        analogWrite(Mot_A2, 0);
      }
      if (stepsL > counterL) {
        analogWrite(Mot_B1, 250);
        analogWrite(Mot_B2, 0);
      } else {
        analogWrite(Mot_B1, 0);
        analogWrite(Mot_B2, 0);
      }
      ultrasonic(8);
      distanceF = distance;
      ultrasonic(13);
      distanceR = distance;
    }
    stepsL = stepsL + 4;
    stepsR = stepsR + 4;

    if (distanceR >= MagicNumber) break;
    else if (distanceF <= MagicNumber) break;
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  detachInterrupt(digitalPinToInterrupt(Mot_R1));
  detachInterrupt(digitalPinToInterrupt(Mot_R2));
}

void constBackward() {
  int stepsR, stepsL;
  stepsR = stepsL = 2;
  counterL = 0;
  counterR = 0;
  u8g2.sendBuffer();
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  while (distanceL <= MagicNumber || distanceR <= MagicNumber) {
    ultrasonic(7);
    distanceL = distance;
    ultrasonic(13);
    distanceR = distance;
    if (distanceL >= MagicNumber) break;
    else if (distanceR >= MagicNumber) break;
    while (stepsR > counterR || stepsL > counterL) {
      if (stepsR > counterR) {
        analogWrite(Mot_A1, 245);
        analogWrite(Mot_A2, 0);
      } else {
        analogWrite(Mot_A1, 0);
        analogWrite(Mot_A2, 0);
      }
      if (stepsL > counterL) {
        analogWrite(Mot_B1, 0);
        analogWrite(Mot_B2, 250);
      } else {
        analogWrite(Mot_B1, 0);
        analogWrite(Mot_B2, 0);
      }
    }
    stepsL = stepsL + 2;
    stepsR = stepsR + 2;
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  detachInterrupt(digitalPinToInterrupt(Mot_R1));
  detachInterrupt(digitalPinToInterrupt(Mot_R2));
}

void forward(int stepsR, int stepsL) {
  counterL = 0;
  counterR = 0;
  u8g2.sendBuffer();
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  while (stepsR > counterR || stepsL > counterL) {
    if (stepsR > counterR) {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 245);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (stepsL > counterL) {
      analogWrite(Mot_B1, 250);
      analogWrite(Mot_B2, 0);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
  }
  detachInterrupt(digitalPinToInterrupt(Mot_R1));
  detachInterrupt(digitalPinToInterrupt(Mot_R2));
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void backward(int stepsR, int stepsL) {
  counterL = 0;
  counterR = 0;
  u8g2.sendBuffer();
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  while (stepsR -1 > counterR || stepsL > counterL) {
    if (stepsR -1 > counterR) {
      analogWrite(Mot_A1, 250);
      analogWrite(Mot_A2, 0);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (stepsL > counterL) {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 253);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
  }
  detachInterrupt(digitalPinToInterrupt(Mot_R1));
  detachInterrupt(digitalPinToInterrupt(Mot_R2));
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
  counterL = 0;
  counterR = 0;
}

void left() {
  int steps = 8;
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  counterL = 0;
  counterR = 0;
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
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  counterL = 0;
  counterR = 0;
  while (steps -1 > counterR || steps > counterL) {
    if (steps -1 > counterR) {
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

void turncalibrate(String advancedir, String dir) {
  if (advancedir == "forward") {
    int Mot_AlaR = 246;
    int Mot_AlaL = 250;
    analogWrite(Mot_A1, Mot_AlaR);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, Mot_AlaL);
    if (dir == "left") {
      while (distanceL >= MagicNumber) {
        ultrasonic(7);
        distanceL = distance;
      }
    } else if (dir == "right") {
      while (distanceR >= MagicNumber) {
        ultrasonic(13);
        distanceR = distance;
      }
    }
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);
    delay(MagicDelay);
    forward(22, 22);
  } else {
    int Mot_AlaR = 250;
    int Mot_AlaL = 253;
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, Mot_AlaR);
    analogWrite(Mot_B1, Mot_AlaL);
    analogWrite(Mot_B2, 0);
    while (distanceL >= MagicNumber) {
      ultrasonic(7);
      distanceL = distance;
    }
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);
    delay(MagicDelay);
    backward(6, 6);
    return;
  }
}
