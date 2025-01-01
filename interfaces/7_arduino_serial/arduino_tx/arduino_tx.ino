void setup() {
  Serial.begin(9600);
}
void loop() {
  Serial.println("Ping: Hello from Arduino!");
  delay(1000);
  Serial.println("Pong: Hello from Arduino!");
  delay(1000);
}
