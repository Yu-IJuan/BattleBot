int trig = 12, echo = 13;

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  

}
void ultrasonic(){
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034/2;
}
