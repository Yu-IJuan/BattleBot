int Mot_A1 = 5;
int Mot_A2 = 4;
int Mot_B1 = 3;
int Mot_B2 = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(Mot_A1, OUTPUT);
  pinMode(Mot_A2, OUTPUT);
  pinMode(Mot_B1, OUTPUT);
  pinMode(Mot_B2, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  delay(1000);
  digitalWrite(Mot_A1, HIGH);
  digitalWrite(Mot_A2, LOW);
  digitalWrite(Mot_B1, LOW);
  digitalWrite(Mot_B2, HIGH);
  Serial.println("HLHL");
  delay(1000);
  digitalWrite(Mot_A1, LOW);
  digitalWrite(Mot_A2, HIGH);
  digitalWrite(Mot_B1, HIGH);
  digitalWrite(Mot_B2, LOW);
  Serial.println("LHLH");
}
