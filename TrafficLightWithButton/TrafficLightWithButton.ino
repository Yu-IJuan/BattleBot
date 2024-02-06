
int L1 = 8, L2 = 9, L3 = 10; //LED Traffic Light R,Y,G
int Button = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(6, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(Button) == false){
    lights();
  }
  else{
    digitalWrite(L1, HIGH);
    digitalWrite(L2, HIGH);
    digitalWrite(L3, HIGH);
  }
}

void lights(){
  digitalWrite(L3, HIGH);
  digitalWrite(L1, LOW);
  delay(3000);
  digitalWrite(L1, HIGH);
  digitalWrite(L2, LOW);
  delay(4000);
  digitalWrite(L2, HIGH);
  digitalWrite(L3, LOW);
  delay(1000);
}
