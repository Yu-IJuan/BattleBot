
int trig = 12, Servomot = 10, Mot_A1 = 9, Mot_A2 = 6, Mot_B1 = 5, Mot_B2 = 3, echo;
unsigned long currenttime, duration, sensetime;
int distance, distance1, distance2, distance3, distanceF, distanceR, distanceL;
bool state = false;
unsigned int counterL, counterR;

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(8, INPUT);
  pinMode(7, INPUT);
  pinMode(13, INPUT);
  pinMode(Servomot, OUTPUT);
  pinMode(Mot_A1, OUTPUT);
  pinMode(Mot_A2, OUTPUT);
  pinMode(Mot_B1, OUTPUT);
  pinMode(Mot_B2, OUTPUT);
  Serial.begin(9600);
}

//logic for moving forward or stopping
void loop() {
  if (distanceF < 20 || distanceL > 20 && distanceR > 20) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 0);
    analogWrite(Mot_B1, 0);
    analogWrite(Mot_B2, 0);
    Serial.println("Stop");
    Stop();
  }
  else if (distanceF > 20 && distanceL < 20 && distanceR < 20) {
    analogWrite(Mot_A1, 0);
    analogWrite(Mot_A2, 255);
    analogWrite(Mot_B1, 255);
    analogWrite(Mot_B2, 0);
    Serial.println("Go");
    moving(90);
  }
  delay(50);
  if (!state) {
    if (millis() - 500 >= currenttime) {
      currenttime = millis;
      ultrasonicformovement(1);
      ultrasonicformovement(2);
      ultrasonicformovement(3);
    }
  }
}

//calculating the distance for movement
void ultrasonicformovement(int number) {
  //  distance forward
  if (number == 1) {
    echo = 8;
    digitalWrite(trig, LOW);
    delay(5);
    digitalWrite(trig, HIGH);
    delay(10);
    digitalWrite(trig, LOW);
    duration = pulseIn(echo, HIGH);
    distanceF = duration * 0.034 / 2;
    Serial.println("Distance forward: ");
    Serial.println(distanceF);
  }
  //  distance left
  else if (number == 2) {
    echo = 7;
    digitalWrite(trig, LOW);
    delay(5);
    digitalWrite(trig, HIGH);
    delay(10);
    digitalWrite(trig, LOW);
    duration = pulseIn(echo, HIGH);
    distanceL = duration * 0.034 / 2;
    Serial.println("Distance to the left: ");
    Serial.println(distanceL);
  }
  //  distance right
  else {
    echo = 13;
    digitalWrite(trig, LOW);
    delay(5);
    digitalWrite(trig, HIGH);
    delay(10);
    digitalWrite(trig, LOW);
    duration = pulseIn(echo, HIGH);
    distanceR = duration * 0.034 / 2;
    Serial.println("Distance to the right: ");
    Serial.println(distanceR);
  }
}

void ultrasonicforneck() {
  echo = 8;
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance:");
  Serial.println(distance);
  Serial.print("Echo:");
  Serial.println(echo);
}

//Moving for the neck
int moving(int angle) {
  int pulseWidth = map(angle, 0, 180, 490, 2500);

  digitalWrite(Servomot, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(Servomot, LOW);
  delay(125);
  ultrasonicforneck();
  delay(200);
}

//Stopping and checking
void Stop(){
  state = true;
  while (state) {
    Serial.println("Right");
    moving(0);
    distance1 = distance;

    Serial.println("Front");
    moving(90);
    distance2 = distance;

    Serial.println("Left");
    moving(180);
    distance3 = distance;

    Serial.println("Dir detect complete");

    state = false;

    Serial.print("DistanceR for neck: ");
    Serial.println(distance1);
    Serial.print("DistanceF for neck: ");
    Serial.println(distance2);
    Serial.print("DistanceL for neck: ");
    Serial.println(distance3);

    moving(90);
    delay(2000);
  }
}
