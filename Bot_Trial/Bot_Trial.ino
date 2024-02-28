#include <DHT11.h>
#include <U8g2lib.h>
#include <Wire.h>

//PIN (Digital)
const int Mot_A1 = 9, Mot_A2 = 6;   //Right Motor (PWM)
const int Mot_B1 = 5, Mot_B2 = 11;  //Left Motor (PWM)
const int Mot_R1 = 3, Mot_R2 = 2;   //RPM Sensor
const int trig = 12, echo;          //Ultrasonic
const int Grab = 4;                 //Claw
//7,8,13 Reserved for ultrasonic sensors' echo pins
//

const int MagicNumber = 40, MagicDelay = 300;

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

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_8x13O_tr);
  u8g2.setCursor(0, 0);
  u8g2.print("Booting...");
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

  temp = dht11.readTemperature();
}

void ISR_R() {
  counterR++;
}

void ISR_L() {
  counterL++;
}

void loop() {
  if (millis() > currenttime) {
    currenttime = millis() + 500;
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
  if (distanceL >= MagicNumber) {
    forward(12, 12);
    screen("L");
    left();  //90 Degrees turn if left empty
    screen("F");
    forward(24, 24);
    delay(MagicDelay);
  } else if (distanceF >= MagicNumber) {
    screen("F");
    constforward();  //In center then walk straight
    delay(MagicDelay);
  } else if (distanceR >= MagicNumber) {
    screen("R");
    right();  //90 Degrees turn if right empty
    screen("F");
    forward(24, 24);
    delay(MagicDelay);
  } else {
    while (distanceR <= MagicNumber && distanceL <= MagicNumber) {
      screen("B");
      backward(24);  //Backward if no route available
      delay(MagicDelay);
      ultrasonic(13);
      distanceR = distance;
      ultrasonic(7);
      distanceL = distance;
    }
    if (distanceR >= MagicNumber) {
      screen("R");
      right();
      delay(MagicDelay);
      screen("F");
      forward(24, 24);
      delay(MagicDelay);
    } else if (distanceL >= MagicNumber) {
      screen("L");
      left();
      delay(MagicDelay);
      if (distanceL >= MagicNumber) {
        screen("L");
        left();
        delay(MagicDelay);
        screen("F");
        forward(24, 24);
        delay(MagicDelay);
      } else {
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

  u8g2.drawBox(123, 0, 2, 62);
  u8g2.drawBox(3, 0, 2, 62);
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
}

void constforward() {
  int Mot_AlaR = 254;
  int Mot_AlaL = 250;
  while (distanceF >= 20) {
    u8g2.clearBuffer();
    int offset = distanceL - distanceR;
    if (offset >= 3 || offset <= -3) {
      if (offset < 7 && offset >= 3) {
        Mot_AlaR = 255;
        Mot_AlaL = 245;
        u8g2.setCursor(30, 30);
        u8g2.print("SmolL");
        delay(500);
        Mot_AlaR = 254;
        Mot_AlaL = 250;
      } else if (offset >= 7) {
        Mot_AlaR = 255;
        Mot_AlaL = 240;
        u8g2.setCursor(30, 30);
        u8g2.print("BigL");
        delay(750);
        Mot_AlaR = 254;
        Mot_AlaL = 250;
      } else if (-7 < offset && offset <= -3) {
        Mot_AlaR = 245;
        Mot_AlaL = 255;
        u8g2.setCursor(30, 30);
        u8g2.print("SmolR");
        delay(500);
        Mot_AlaR = 254;
        Mot_AlaL = 250;
      } else if (offset <= -7) {
        Mot_AlaR = 240;
        Mot_AlaL = 255;
        u8g2.setCursor(30, 30);
        u8g2.print("BigR");
        delay(750);
        Mot_AlaR = 254;
        Mot_AlaL = 250;
      } else {
        Mot_AlaR = 254;
        Mot_AlaL = 250;
      }
    } else {
      Mot_AlaR = 254;
      Mot_AlaL = 250;
    }
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, Mot_AlaR);
    analogWrite(Mot_B1, Mot_AlaL);
    analogWrite(Mot_B2, 0);
    u8g2.setCursor(96, 30);
    u8g2.print(offset);
    u8g2.sendBuffer();
    ultrasonic(8);
    distanceF = distance;
    ultrasonic(7);
    distanceL = distance;
    ultrasonic(13);
    distanceR = distance;
    if (distanceL >= MagicNumber) break;
  }
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 0);
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
      analogWrite(Mot_A2, 254);
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

void left() {
  int steps = 8;
  counterL = 0;
  counterR = 0;
  u8g2.setCursor(30, 55);
  u8g2.print("Steps:");
  u8g2.setCursor(90, 55);
  u8g2.print(steps);
  u8g2.sendBuffer();
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
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

void right() {
  int steps = 8;
  counterL = 0;
  counterR = 0;
  u8g2.setCursor(30, 55);
  u8g2.print("Steps:");
  u8g2.setCursor(90, 55);
  u8g2.print(steps);
  u8g2.sendBuffer();
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
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


void backward(int steps) {
  counterL = 0;
  counterR = 0;
  attachInterrupt(digitalPinToInterrupt(Mot_R1), ISR_R, RISING);
  attachInterrupt(digitalPinToInterrupt(Mot_R2), ISR_L, RISING);
  while (steps > counterR || steps > counterL) {
    if (steps > counterR) {
      analogWrite(Mot_A1, 254);
      analogWrite(Mot_A2, 0);
    } else {
      analogWrite(Mot_A1, 0);
      analogWrite(Mot_A2, 0);
    }
    if (steps > counterL) {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 250);
    } else {
      analogWrite(Mot_B1, 0);
      analogWrite(Mot_B2, 0);
    }
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
