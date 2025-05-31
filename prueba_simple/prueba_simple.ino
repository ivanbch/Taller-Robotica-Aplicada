#define motor_izq_a D8
#define motor_izq_b D7
#define motor_der_a D6
#define motor_der_b D5
#define pinLed 2
int tiempoGiro = 0;
void setup () {
  Serial.begin(9600);
}

void loop () {
  while (tiempoGiro < 20000) {
    Serial.println (tiempoGiro);
    tiempoGiro +=1;
  }
}