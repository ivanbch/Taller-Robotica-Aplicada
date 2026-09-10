#ifndef AUTONOMIA_FSM_H
#define AUTONOMIA_FSM_H

#include <Arduino.h>

// NOTA: Asumimos que navegacion.h y las constantes globales (umbral, tiempoGiro, etc.) 
// ya están incluidas o definidas en el main o previamente. 
// Reutilizamos calcularDistancia() de autonomia.h por ahora.

// 1. Definimos todos los estados posibles del Rover
enum EstadoRover {
  ESTADO_AVANZANDO,
  ESTADO_RETROCEDIENDO,
  ESTADO_GIRANDO_IZQ,
  ESTADO_GIRANDO_DER
};

// 2. Variables de control de tiempo y estado
EstadoRover estadoActual = ESTADO_AVANZANDO;
unsigned long tiempoInicioEstado = 0;

const unsigned long TIEMPO_RETROCESO = 500;  // Medio segundo de retroceso
const unsigned long TIEMPO_GIRO_90 = 500;    // Asumimos 500ms para 90 grados

// Función auxiliar para transicionar de un estado a otro de manera limpia
void cambiarEstado(EstadoRover nuevoEstado) {
  estadoActual = nuevoEstado;
  tiempoInicioEstado = millis(); // "Reseteamos" el cronómetro
}

// 3. La Máquina de Estados: Se llama constantemente desde el loop()
void actualizarAutonomiaFSM() {
  
  // Leemos el sensor (esto tarda microsegundos, es aceptable)
  int dist = calcularDistancia();

  switch (estadoActual) {
    
    // --- ESTADO 1: AVANZANDO ---
    case ESTADO_AVANZANDO:
      if (dist > umbral) {
        avanzar(); // Todo despejado, sigue derecho
      } else {
        // Encontramos un muro, frenamos y cambiamos de estado
        detenerse();
        Serial.println("Obstáculo detectado! Retrocediendo...");
        cambiarEstado(ESTADO_RETROCEDIENDO);
      }
      break;

    // --- ESTADO 2: RETROCEDIENDO ---
    case ESTADO_RETROCEDIENDO:
      marchaAtras();
      // Chequeamos el cronómetro: ¿Ya pasaron 500ms?
      if (millis() - tiempoInicioEstado >= TIEMPO_RETROCESO) {
        detenerse();
        Serial.println("Girando a la izquierda...");
        cambiarEstado(ESTADO_GIRANDO_IZQ);
      }
      break;

    // --- ESTADO 3: GIRANDO A LA IZQUIERDA ---
    case ESTADO_GIRANDO_IZQ:
      girarAntiHorario();
      // ¿Ya terminó de girar 90 grados?
      if (millis() - tiempoInicioEstado >= TIEMPO_GIRO_90) {
        detenerse();
        // Si el camino está libre, avanzamos. Si no, probamos al otro lado.
        if (calcularDistancia() > umbral) {
          Serial.println("Camino libre, avanzamos.");
          cambiarEstado(ESTADO_AVANZANDO);
        } else {
          Serial.println("Sigue bloqueado, probamos a la derecha...");
          cambiarEstado(ESTADO_GIRANDO_DER);
        }
      }
      break;

    // --- ESTADO 4: GIRANDO A LA DERECHA (180 grados desde la pos actual) ---
    case ESTADO_GIRANDO_DER:
      girarHorario();
      // Le damos el doble de tiempo para deshacer el giro izquierdo y mirar al lado contrario
      if (millis() - tiempoInicioEstado >= (TIEMPO_GIRO_90 * 2)) {
        detenerse();
        Serial.println("Intento completado, volvemos a evaluar el avance.");
        cambiarEstado(ESTADO_AVANZANDO); 
        // Si sigue habiendo pared, el estado AVANZANDO lo va a detectar al instante
        // y volver a activar el ciclo de evasión. ¡Magia!
      }
      break;
  }
}

#endif
