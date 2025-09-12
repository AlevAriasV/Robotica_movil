const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
long duration;
int distance;
//long baud_rate = 9600;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
  //Serial.begin(baud_rate);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(10);

  //Pone al TRIG_PIN en HIGH Por 10 micro segundos
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  //Calcula el tiempo de viaje de la onda sonora en microsegundos
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034/2;

  //Imprime la distancia en el serial monitor
  //Serial.print("Distance: ");
  Serial.print(distance);

}
