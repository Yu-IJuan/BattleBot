int Mot_A1 = 9, Mot_A2 = 6, Mot_B1 = 5, Mot_B2 = 3;

void setup() {
  pinMode(Mot_A1, OUTPUT);
  pinMode(Mot_A2, OUTPUT);
  pinMode(Mot_B1, OUTPUT);
  pinMode(Mot_B2, OUTPUT);
}

void loop() {
  //  Moving forward
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 255);
  analogWrite(Mot_B1, 255);
  analogWrite(Mot_B2, 0);
  //  Moving backward
  analogWrite(Mot_A1, 255);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 255);
  //  Moving right
  analogWrite(Mot_A1, 255);
  analogWrite(Mot_A2, 0);
  analogWrite(Mot_B1, 255);
  analogWrite(Mot_B2, 0);
  //  Moving left
  analogWrite(Mot_A1, 0);
  analogWrite(Mot_A2, 255);
  analogWrite(Mot_B1, 0);
  analogWrite(Mot_B2, 255);
}
