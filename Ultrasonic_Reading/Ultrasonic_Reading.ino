
#include <DHT11.h>

//PIN (Digital)
const int Mot_A1 = 9, Mot_A2 = 6;   //Right Motor (PWM)
const int Mot_B1 = 5, Mot_B2 = 11;  //Left Motor (PWM)
const int Mot_R1 = 3, Mot_R2 = 2;   //RPM Sensor
int trig, echo;          //Ultrasonic
const int Grab = 4;                 //Claw
const int ServoMot = 10;            //Neck Servo
//7,8,13 Reserved for ultrasonic sensors' echo pins
//

//PIN (Analog)
const int D1 = A0, D3 = A1, D4 = A2;
const int D5 = A3, D6 = A6, D8 = A7;  //For IR B&W Sensor
//

DHT11 dht11(1);  //DHT's PIN
int temp;        //For recording Temp

unsigned long currenttime, refreshtime;  //For millis() functions
int duration, distance;                  //For Ultrasonic functions
unsigned int counterR, counterL;

int distanceR, distanceL, distanceF;
int pulseWidth;

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
  pinMode(ServoMot, OUTPUT);

  pinMode(12, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(13, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  pinMode(Grab, OUTPUT);

  temp = dht11.readTemperature();
  delay(1000);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  ultrasonic(13);
  distanceR = distance;
  ultrasonic(8);  //Checking with different ultrasonic sensors,
  distanceF = distance;
  ultrasonic(7);         //But they all share same trigger pin,
  distanceL = distance;  //So it saves pin resources.

  delay(500);
  Serial.println("Distance");
  Serial.println(distanceR);
  Serial.println(distanceL);
  Serial.println(distanceF);  
}

void ultrasonic(int echo) {
  if (echo == 7) trig = A5;
  else if (echo == 8) trig = 12;
  else trig = A4;
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