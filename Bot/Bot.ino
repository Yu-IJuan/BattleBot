#include <DHT11.h>
#include <Adafruit_NeoPixel.h>

//PIN (Digital)
const int Mot_A1    = 9,  Mot_A2  = 6;  //Right Motor (PWM)
const int Mot_B1    = 5,  Mot_B2  = 3;  //Left Motor (PWM)
const int Mot_R1    = 4,  Mot_R2  = 2;  //RPM Sensor
const int trig      = 12, echo    = 13; //Ultrasonic
const int Neo       = 7;                //NeoPixels
const int Grab      = 11;               //Claw
const int ServoMot  = 10;               //Neck Servo
//

//PIN (Analog)
const int D1 = A0, D2 = A1, D3 = A2, D4 = A3;
const int D5 = A4, D6 = A5, D7 = A6, D8 = A7; //For IR B&W Sensor 
//

const int LED_COUNT = 4;                //NeoPixel's LED count
Adafruit_NeoPixel strip(LED_COUNT, Neo, NEO_GRB + NEO_KHZ800);
                                        //NeoPixel's LED init setting

unsigned long currenttime;                   //For millis() functions
unsigned long duration, distance;     //For Ultrasonic functions

void setup() {
  // put your setup code here, to run once:
  pinMode(Mot_A1,   OUTPUT);
  pinMode(Mot_A2,   OUTPUT);
  pinMode(Mot_B1,   OUTPUT);
  pinMode(Mot_B2,   OUTPUT);
  pinMode(Mot_R1,   OUTPUT);
  pinMode(Mot_R2,   OUTPUT);

  pinMode(ServoMot, OUTPUT);

  pinMode(trig,     OUTPUT);
  pinMode(echo,     INPUT);

  pinMode(Grab,     OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}
