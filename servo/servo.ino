#include <DHT11.h>

//PIN (Digital)
const int Mot_A1 = 9, Mot_A2 = 6;   //Right Motor (PWM)
const int Mot_B1 = 5, Mot_B2 = 11;  //Left Motor (PWM)
const int Mot_R1 = 3, Mot_R2 = 2;   //RPM Sensor
const int trig = 12, echo = 13;     //Ultrasonic
const int Grab = 4;                 //Claw
const int ServoMot = 10;            //Neck Servo
//

//PIN (Analog)
const int D1 = A0, D3 = A1, D4 = A2;
const int D5 = A3, D6 = A6, D8 = A7;  //For IR B&W Sensor
//

const int diskslot = 20;
const float dia = 6.52;
//NeoPixel's LED init setting
DHT11 dht11(1);  //DHT's PIN
int temp;        //For recording Temp

unsigned long currenttime;         //For millis() functions
unsigned long duration, distance;  //For Ultrasonic functions

int distance1, distance2, distance3, distanceR, distanceL, distanceF;



void setup() {
  // put your setup code here, to run once:
  pinMode(Mot_A1, OUTPUT);
  pinMode(Mot_A2, OUTPUT);
  pinMode(Mot_B1, OUTPUT);
  pinMode(Mot_B2, OUTPUT);
  pinMode(Mot_R1, OUTPUT);
  pinMode(Mot_R2, OUTPUT);

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
  Serial.begin(115200);
}

void loop() {
  ultrasonic(8);
    moving(0);

    moving(90);

    moving(180);
}

void moving(int angle) {
  int pulseWidth = map(angle, 0, 180, 544, 2400);

  digitalWrite(ServoMot, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(ServoMot, LOW);
  delay(20);
}
