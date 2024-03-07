

int MOTOR_A1 = 5, MOTOR_A2 = 4; //Right Motor
int MOTOR_B1 = 3, MOTOR_B2 = 2; //Left Motor
int trig = 12, echo = 13;
const int Neo = 6;
int MOTOR_Analog1 = 9, MOTOR_Analog2 = 10;
int Grab = 11;


unsigned long time = 0, duration = 0, distance = 0, RGB_TIME;

int LED_COUNT = 4, BRIGHTNESS = 255;

Adafruit_NeoPixel strip(LED_COUNT, Neo, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(MOTOR_B1, OUTPUT);
  pinMode(MOTOR_B2, OUTPUT);
  pinMode(MOTOR_Analog1, OUTPUT);
  pinMode(MOTOR_Analog2, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(Grab, OUTPUT);
  pinMode(echo, INPUT);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

}

void loop() {
  if(distance <= 30){
    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, LOW);
    digitalWrite(MOTOR_B1, LOW);
    digitalWrite(MOTOR_B2, LOW);
    Serial.println("Stop");
    Red();
  }
  else if (distance <= 70 && distance > 30){
    Yellow();
  } 
  else {
    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, HIGH);
    digitalWrite(MOTOR_B1, HIGH);
    digitalWrite(MOTOR_B2, LOW);
    Serial.println("Run");
    analogWrite(MOTOR_Analog1, 255);
    analogWrite(MOTOR_Analog2, 250);
    Green();
  }
  if(millis() - 500 >= time) {
    ultrasonic();
    time = millis();
  }
  delay(50);
}

void ultrasonic(){
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 /2;
}

void Green(){
  strip.clear();
  for(int i=0; i< Neo; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    Serial.println("G");
    Serial.println(i);
  }
}

void Red(){
  strip.clear();
  for(int i=0; i< Neo; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
    strip.show();
    Serial.println("R");
    Serial.println(i);
  }
  digitalWrite(MOTOR_A1, HIGH);
  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, LOW);
  digitalWrite(MOTOR_B2, HIGH);
  delay(1000);
  digitalWrite(MOTOR_A1, HIGH);
  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, HIGH);
  digitalWrite(MOTOR_B2, LOW);
}

void Yellow(){
  strip.clear();
  for(int i = 0; i < Neo; i++){
    strip.setPixelColor(i, strip.Color(140, 255, 0));
    Serial.println(i);
    Serial.println("Yellow");
    strip.show();
  }
}
