#include <navegacion.h>
#define tiempoGiro 500
#define tiempoAvance 1500
#define umbral 15
#define Trigger D2
#define Echo D1
// funciones autónomas
long duracion;
int distancia;
int calcularDistancia() {
 digitalWrite(Trigger, LOW);
 delayMicroseconds(2);
//Configurar trigPin en alto por 10 microsegundos
 digitalWrite(Trigger, HIGH);
 delayMicroseconds(10);
 digitalWrite(Trigger, LOW);
// Leer el pin echo
 duracion = pulseIn(Echo, HIGH);
 distancia = duracion*0.034/2; //ojo con las unidades
 return distancia;
}


bool girar90Izq () {
  int i = 0;
  detenerse();
  delay(500);
  while (i < tiempoGiro) {       //tiempo para que gire 90 grados en ms, fijarse
    girarAntiHorario ();
    delay (100);
    i += 100;
  }
  i = 0;
  detenerse ();
  delay(500);
  int distancia = calcularDistancia ();
  bool frenteLibre = false;
  if (distancia > umbral) {
    while (distancia > umbral && i < tiempoAvance && frenteLibre == false) {  
      if (i%500 == 0) {    //explicado en la función de abajo
        girarHorario ();
        delay(500);
        distancia = calcularDistancia ();
        if (distancia > umbral) {
          frenteLibre = true;
        } else {
          girarAntiHorario ();
          delay (500);
        }
      }
      if (frenteLibre == false) {
        avanzar ();
        delay (100);
        i += 100;
        distancia = calcularDistancia ();
      }
      
    }
    if (frenteLibre == false) {   
      int tAvanz= i;       //avanzó una vez que dobló pero era muy poco (depende del ancho del objeto)
      i = 0;
      detenerse ();
      delay(500);
      while (i < tAvanz) {    
        marchaAtras ();
        delay (100);
        i += 100;
      }
      detenerse ();
      delay(500);
      i = 0;
      while (i < tiempoGiro) {
        girarHorario ();   
        delay (100);
        i += 100;
      }
    } 
  } else {        
      detenerse ();
      delay(500);
      i = 0;               //deshace el giro (no llegó a avanzar)
      while (i < tiempoGiro) {
        girarHorario ();   
        delay (100);
        i += 100;
      }
    }
  if (frenteLibre == true) {
    Serial.println("giró bien izq 90");
  } else {
    Serial.println("NO giró izq 90");
  }
  return frenteLibre;
}


bool girar90Der () {
  int i = 0;
  detenerse();
  delay (500);
  while (i < tiempoGiro) {       //tiempo para que gire 90 grados en ms, fijarse
    girarHorario ();
    delay (100);
    i += 100;
  }
  i = 0;
  detenerse();
  delay (500);
  int distancia = calcularDistancia ();
  bool frenteLibre = false;
  if (distancia > umbral) {
    while (distancia > umbral && i < tiempoAvance && frenteLibre == false) {  
      if (i%500 == 0) {   //como tiene un radar en el frente nada más, cada medio segundo gira 90 grados para quedar en la dirección original,
        girarAntiHorario ();   //analiza si el frente está libre, si no lo está gira 90 grados para el otro lado, y sigue avanzando
        delay(500);             //paralelo al obstáculo hasta que cumpla el tiempoAvance máximo. 
        distancia = calcularDistancia ();
        if (distancia > umbral) {
          frenteLibre = true;
        } else {
          girarHorario ();
          delay (500);
        }
      }
      if (frenteLibre  == false) {
        avanzar ();
        delay (100);
        i += 100;
        distancia = calcularDistancia ();
      }
    }
    if (frenteLibre == false) {      //retrocede
      int tAvanz= i;    //tiempo que avanzó
      i = 0;
      detenerse();
      delay (500);
      while (i < tAvanz) {    
        marchaAtras ();
        delay (100);
        i += 100;
      }
      i = 0;
      detenerse();
      delay (500);
      while (i < tiempoGiro) {
        girarAntiHorario ();   
        delay (100);
        i += 100;
      }
    } 
  } else {        
      i = 0;
      detenerse();
      delay (500);               //deshace el giro (no llegó a avanzar)
      while (i < tiempoGiro) {
        girarAntiHorario ();   
        delay (100);
        i += 100;
      }
    }
  if (frenteLibre == true) {
    Serial.println("giró bien der 90");
  } else {
    Serial.println("NO giró der 90");
  }
  return frenteLibre;
}
/*
bool esquivarXIzq () {
  bool pudo = true;
  int i = 0;
  int distancia = calcularDistancia ();
  detenerse();
  delay (500);
  bool giro90 = girar90Izq ();
  if (giro90) {
    while (i < tiempoGiro) {
      girarHorario ();
      delay (100);
      i += 100;
    }
    distancia = calcularDistancia();
    if (distancia < umbral) {
      pudo = false;
      i = 0;
      detenerse();
      delay (500);
      while (i < tiempoGiro) {            // creo que haría la L invertida
        marchaAtras ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (500);
      i = 0;
      while (i < tiempoGiro) {
        girarAntiHorario ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (500);
      i = 0;
      while (i < tiempoGiro) {
        marchaAtras ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (500);
      i = 0;
      while (i < tiempoGiro) {
        girarHorario ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (800);
    }
  } else {
    pudo = false;
  }
  if (pudo == true) {
    Serial.println("esquivó bien x izq");
  } else {
    Serial.println("NO esquivó x izq");
  } 
 return pudo; 
} 

bool esquivarXDer () {
  bool pudo = true;
  int i = 0;
  int distancia = calcularDistancia ();
  detenerse();
  delay (500);
  bool giro90 = girar90Der ();
  if (giro90) {
    while (i < tiempoGiro) {
      girarAntiHorario ();
      delay (100);
      i += 100;
    }
    distancia = calcularDistancia ();
    if (distancia < umbral) {
      pudo = false;
      detenerse();
      delay (500);
      i = 0;
      while (i < tiempoGiro) {            // creo que haría la L invertida
        marchaAtras ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (500);
      i = 0;
      while (i < tiempoGiro) {
        girarHorario ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (500);
      i = 0;
      while (i < tiempoGiro) {
        marchaAtras ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (500);
      i = 0;
      while (i < tiempoGiro) {
        girarAntiHorario ();
        delay (100);
        i += 100;
      }
      detenerse();
      delay (800);
    }
  } else {
    pudo = false;
  } 
  if (pudo == true) {
    Serial.println("esquivó bien x der");
  } else {
    Serial.println("NO esquivó x der");
  }
 return pudo; 
}

*/
void giro180 () {
  int i = 0;
  detenerse();
  delay(500);
  while (i < 2*tiempoGiro) { //tiempo giro 180 grados, calcular
    girarHorario ();
    delay (100);
    i += 100;
  }
}

void autonomiaSimple () {
    distancia = calcularDistancia();
    Serial.println ("distancia a obstáculo: " + String(distancia));
    if (distancia > umbral) {   
      Serial.println("avanza");
      avanzar();
      delay(50);
    } else {
        bool evadirIzq = girar90Izq ();
        if (evadirIzq == false) {
          bool evadirDer = girar90Der ();
          if (evadirDer == false) {
            Serial.println("gira 180");
            marchaAtras();
            delay(500);
            giro180();
          }
        }
    } 
}