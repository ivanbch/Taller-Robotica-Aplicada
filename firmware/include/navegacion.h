#include <motores.h>
#include <led.h>

void girarIzq () { //rover gira hacia la izquierda; revisar sentido
  frenar (motor_izq);
  girarAdelante (motor_der);
  prenderLed ();
}

void girarDer () { //lo mismo pero hacia la derecha
  frenar (motor_der);
  girarAdelante (motor_izq);
  prenderLed ();
}

void avanzar () {
  girarAdelante (motor_izq);
  girarAdelante (motor_der);
  prenderLed ();
} 

void detenerse () {
  frenar (motor_izq);
  frenar (motor_der);
  apagarLed ();
} 

void marchaAtras () {
  girarAtras (motor_izq);
  girarAtras (motor_der);
  prenderLed ();
}

void girarIzqMarchaAtras () { //rover gira hacia la izquierda marcha atrás; revisar sentido
  girarAtras (motor_der);
  frenar (motor_izq);
  prenderLed ();
}

void girarHorario () { //rover gira hacia la izquierda marcha atrás; revisar sentido
  girarAtras (motor_der);
  girarAdelante (motor_izq);
  prenderLed ();
}

void girarAntiHorario () { //rover gira hacia la izquierda marcha atrás; revisar sentido
  girarAtras (motor_izq);
  girarAdelante (motor_der);
  prenderLed ();
}

void girarDerMarchaAtras () { //rover gira hacia la derecha marcha atrás; revisar sentido
  girarAtras (motor_izq);
  frenar (motor_der);
  prenderLed ();
}