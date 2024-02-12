//PIN (Digital)
const int Mot_A1    = 9,  Mot_A2  = 6;  //Right Motor (PWM)
const int Mot_B1    = 5,  Mot_B2  = 11;  //Left Motor (PWM)
const int Mot_R1    = 3,  Mot_R2  = 2;  //RPM Sensor (R/L)
const int trig      = 12, echo    = 13; //Ultrasonic
const int Neo       = 7;                //NeoPixels
const int Grab      = 4;               //Claw
const int ServoMot  = 10;               //Neck Servo
//

//PIN (Analog)
const int D1 = A0, D2 = A1, D3 = A2, D4 = A3;
const int D5 = A4, D6 = A5, D7 = A6, D8 = A7; //For IR B&W Sensor 
//

unsigned int counterL, counterR;

const int diskslot = 20;
const float dia = 6.52;

const int LED_COUNT = 4;                //NeoPixel's LED count
Adafruit_NeoPixel strip(LED_COUNT, Neo, NEO_GRB + NEO_KHZ800);
                                        //NeoPixel's LED init setting
DHT11 dht11(2);                         //DHT's PIN
int       temp;                         //For recording Temp

unsigned long currenttime;              //For millis() functions
unsigned long duration, distance;       //For Ultrasonic functions

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(0, )
}

void ISR_R(){
  counterR++;
}

void ISR_L(){
  counterL++;
}

void loop() {
  //  Moving forward
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 255);
  analogWrite(Mot_B1, 255);
  analogWrite(Mot_B2, 0);
//  //  Moving backward
//  analogWrite(Mot_A1, 255);
//  analogWrite(Mot_A2, 0);
//  analogWrite(Mot_B1, 0);
//  analogWrite(Mot_B2, 255);
//  //  Moving right
//  analogWrite(Mot_A1, 255);
//  analogWrite(Mot_A2, 0);
//  analogWrite(Mot_B1, 255);
//  analogWrite(Mot_B2, 0);
//  //  Moving left
//  analogWrite(Mot_A1, 0);
//  analogWrite(Mot_A2, 255);
//  analogWrite(Mot_B1, 0);
//  analogWrite(Mot_B2, 255);
//}
