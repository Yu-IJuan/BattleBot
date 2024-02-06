int D1 = 8, D2 = 9, D3 = 10;
int Button1 = 11, Button2 = 12, Button3 = 13;
bool State = false;

unsigned long time = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
  pinMode(Button3, INPUT);

  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() >= time) {
    State = !State;
    digitalWrite(D1, State);
    time = millis() + 1000;
  }
  if(!digitalRead(Button1) || !digitalRead(Button2) || !digitalRead(Button3)){
    digitalWrite(D3, LOW);
    Serial.println("On");
  } else {
    digitalWrite(D3, HIGH);
    Serial.println("Off");
  }
  Serial.println(digitalRead(Button1));
  Serial.println("1");
  Serial.println(digitalRead(Button2));
  Serial.println("2");
  Serial.println(digitalRead(Button3));
  Serial.println("3");
  delay(500);
}
