#include <Arduino.h>
#include <autonomia.h>
#include <autonomia_fsm.h> // <-- ¡Nuestra nueva máquina de estados!
#include <navegacion.h>
#include <conexion.h>
/*
creo que lo mejor es usar el rover como Access Point
*/
int estadoLed = LOW;
void setup() {
  Serial.begin(115200);
  pinMode(pinLed,OUTPUT);
  manual = true;
  configurarMotor (motor_izq);
  configurarMotor (motor_der);
  for (int c = 0;c<4;c++) { //para que no arranque tan rápido el serial y todo eso
    prenderLed ();
    delay (800);
    apagarLed();
    delay(800);
  }
  iniciarWifi();
}

void loop() {
  manejarMiCliente();
  if (!manual) {
    // autonomiaSimple(); // <-- Esta era tu función vieja (bloqueante)
    actualizarAutonomiaFSM(); // <-- ¡Nuestra nueva función no bloqueante!
  }
} 