#include <DHT11.h>
#include <Adafruit_NeoPixel.h>

//PIN (Digital)
const int Mot_A1    = 9,  Mot_A2  = 6;  //Right Motor (PWM)
const int Mot_B1    = 5,  Mot_B2  = 11;  //Left Motor (PWM)
const int Mot_R1    = 3,  Mot_R2  = 2;  //RPM Sensor
const int trig      = 12, echo    = 13; //Ultrasonic
const int Neo       = 7;                //NeoPixels
const int Grab      = 4;               //Claw
const int ServoMot  = 10;               //Neck Servo
//

//PIN (Analog)
const int D1 = A0, D2 = A1, D3 = A2, D4 = A3;
const int D5 = A4, D6 = A5, D7 = A6, D8 = A7; //For IR B&W Sensor 
//

const int diskslot = 20;
const float = 6.52;

const int LED_COUNT = 4;                //NeoPixel's LED count
Adafruit_NeoPixel strip(LED_COUNT, Neo, NEO_GRB + NEO_KHZ800);
                                        //NeoPixel's LED init setting
DHT11 dht11(2);                         //DHT's PIN
int       temp;                         //For recording Temp

unsigned long currenttime;              //For millis() functions
unsigned long duration, distance;       //For Ultrasonic functions

void setup() {
  pinMode(Mot_A1,   OUTPUT);
  pinMode(Mot_A2,   OUTPUT);
  pinMode(Mot_B1,   OUTPUT);
  pinMode(Mot_B2,   OUTPUT);
  pinMode(Mot_R1,   OUTPUT);
  pinMode(Mot_R2,   OUTPUT);

  pinMode(D1,       INPUT);
  pinMode(D2,       INPUT);
  pinMode(D3,       INPUT);
  pinMode(D4,       INPUT);
  pinMode(D5,       INPUT);
  pinMode(D6,       INPUT);
  pinMode(D7,       INPUT);
  pinMode(D8,       INPUT);
  pinMode(ServoMot, OUTPUT);

  pinMode(trig,     OUTPUT);
  pinMode(echo,     INPUT);

  pinMode(Grab,     OUTPUT);

  temp = dht11.readTemperature();
  Serial.begin(115200);
}

void loop() {

  if (!state) {
        if (millis() - 500 >= currenttime){
          currenttime = millis();
          ultrasonic();
      }
    }
}

void see(int angle) {
  int pulseWidth = map(angle, 0, 180, 450, 2500);
  digitalWrite(Servomot, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(Servomot, LOW);
  delay(20);
  ultrasonic();
  delay(200);
}

void ultrasonic(){
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * (0.0331 + 0.6 * temp) /2;
}

void Red(){
  state = true;
  Serial.println("Right");
  moving(0);

  Serial.println("Front");
  moving(90);

  Serial.println("Left");
  moving(180);

  Serial.println("Dir detect complete");
  delay(5000);
  state = false;
}

void Start(){

}

void End(){

}
