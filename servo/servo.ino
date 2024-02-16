const int ServoMot = 10;            //Neck Servo



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
