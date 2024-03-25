
//PIN (Digital)
#define Mot_A1 9
#define Mot_A2 6  //Right Motor (PWM)
#define Mot_B1 5
#define Mot_B2 11  //Left Motor (PWM)
#define Mot_R1 3
#define Mot_R2 2  //RPM Sensor

#define trigLR A5  //trigger for L and R is shared
#define trigF 12
#define echoL 7  //Ultrasonic
#define echoF 8
#define echoR 13

#define Grab A4  //Claw
#define Neo 10   //LED pin
#define Radar 4  //Radar pin

#define MagicNumber 15
#define BiggerMagicNumber 25
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

unsigned long currenttime = 0, refreshtime = 0, reportPeriod = 0;  //For millis() functions
int duration = 0, distance = 0;                                    //For Ultrasonic functions
float distancePL = 0, distancePR = 0;
unsigned int counterR = 0, counterL = 0;
int distanceR = 0, distanceL = 0, distanceF = 0;
int SmolAngle = 0;


int pulseWidth = 0;

bool stateServo = false, ranOnce = false;
int stateTurn = 0;
int Black = 0, White = 0;
bool starting = false, BnWstate = false, ending = false;
int A = 0, B = 0, C = 0, D = 0, E = 0, F = 0;
int echo = 0, trig = 0;
bool backCali = 0;

bool started = false, ended = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(D1, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D8, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("A: ");
  Serial.println(analogRead(D1));
  Serial.print("B: ");
  Serial.println(analogRead(D3));
  Serial.print("C: ");
  Serial.println(analogRead(D4));
  Serial.print("D: ");
  Serial.println(analogRead(D5));
  Serial.print("E: ");
  Serial.println(analogRead(D6));
  Serial.print("F: ");
  Serial.println(analogRead(D8));
  Serial.println("=======================================");
}
