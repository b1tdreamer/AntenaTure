const int RELE_1 = 2;

void setup() {
  pinMode(RELE_1, OUTPUT);
  digitalWrite(RELE_1, LOW);  // relé OFF al inicio
}

void loop() {
  digitalWrite(RELE_1, HIGH);  // activa relé (High Level Trigger)
  delay(2000);
  digitalWrite(RELE_1, LOW);
  delay(2000);
}