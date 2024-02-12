//PIN (Analog)
const int D1 = A0, D2 = A1, D3 = A2, D4 = A3;
const int D5 = A4, D6 = A5, D7 = A6, D8 = A7; //For IR B&W Sensor 
//

void setup() {
  // put your setup code here, to run once:
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);
  pinMode(D8, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(analogRead(D1));
  Serial.println(".");
  Serial.print(analogRead(D2));
  Serial.println(".");
  Serial.print(analogRead(D3));
  Serial.println(".");
  Serial.print(analogRead(D4));
  Serial.println(".");
  Serial.print(analogRead(D5));
  Serial.println(".");
  Serial.print(analogRead(D6));
  Serial.println(".");
  Serial.print(analogRead(D7));
  Serial.println(".");
  Serial.print(analogRead(D8));
  Serial.println(".");
  Serial.println("==============");
}
