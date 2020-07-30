int led1 = A1;
int led2 = A3;
int led3 = 3;
int pot = A4;
int val = 0;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(pot, INPUT);

  Serial.begin(115200);

  analogWrite(led1, 255);
  analogWrite(led2, 150);
  // analogWrite(led3, 100);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(pot);
  Serial.println(val);
  //analogWrite(led3, val/4);
  analogWrite(led3, val/4);
  delay(100);
}
