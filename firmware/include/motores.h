#include <Arduino.h>
#define motor_izq_a D8
#define motor_izq_b D7
#define motor_der_a D6
#define motor_der_b D5

struct motor {
  int pin1;
  int pin2;
};
motor motor_izq {motor_izq_a,motor_izq_b};
motor motor_der {motor_der_a,motor_der_b};
void configurarMotor (motor m) {
  pinMode(m.pin1,OUTPUT);
  pinMode(m.pin2,OUTPUT);
}
/*
Funciones de motor
*/
void girarAdelante (motor m) {  //esto es para controlar el giro de un motor en específico hacia adelante, habría que confirmar
  digitalWrite(m.pin1,HIGH);           // que sea el correcto, sino reasigno los pines en la instancia del motor y listo
  digitalWrite(m.pin2,LOW);
}

void girarAtras (motor m) {
  digitalWrite(m.pin1,LOW);
  digitalWrite(m.pin2,HIGH);
}

void frenar (motor m) { //frena UN motor solo
  digitalWrite(m.pin1,LOW);
  digitalWrite(m.pin2,LOW);
}
