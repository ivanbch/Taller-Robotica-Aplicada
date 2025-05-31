#include <ESP8266WiFi.h>
#define motor_izq_a D8
#define motor_izq_b D7
#define motor_der_a D6
#define motor_der_b D5
#define pinLed D4
#define Trigger D2
#define Echo D1
#define tiempoGiro 500
#define tiempoAvance 1500
#define umbral 15
bool reversa = false;
const char* nombre_wifi = "Robotin";
const char* contra = "robotin123";
long duracion;
int distancia;
bool manual = true; 
/*

creo que lo mejor es usar el rover como Access Point

*/
IPAddress ip_rover(192,168,4,1);       // ip del rover (confirmar que pueda servir la misma)
IPAddress puerta_enlace(192,168,4,1);        // Gateway (igual que la IP en modo punto de acceso)
IPAddress mascara(255,255,255,0);        // Máscara de subred



//IPAddress ip_rover (192,168,253,10);        // (si quiero usar el hotspot como router)
//IPAddress ip_rover (192,168,137,10);        // (si quiero usar mi pc como router)
//IPAddress puerta_enlace (192,168,253,154); // (si quiero usar el hotspot como router)
//IPAddress puerta_enlace (192,168,137,1); // (si quiero usar mi pc como router)
//IPAddress mascara(255,255,255,0);


struct motor {
  int pin1;
  int pin2;
};

motor motor_izq {motor_izq_a,motor_izq_b};
motor motor_der {motor_der_a,motor_der_b};

WiFiServer server(80);
int estadoLed = LOW;
const long intervalo = 200;
unsigned long millisAnterior = 0;
void configurarMotor (motor m) {
  pinMode(m.pin1,OUTPUT);
  pinMode(m.pin2,OUTPUT);
}

void prenderLed () {
  digitalWrite(pinLed,LOW);
}
void apagarLed () {
  digitalWrite(pinLed,HIGH);
}

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

/*
Funciones de navegación
*/
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
// funciones autónomas

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
        detenerse ();
        delay(500);
        distancia = calcularDistancia ();
        if (distancia > umbral) {
          frenteLibre = true;
        } else {
          girarAntiHorario ();
          delay (500);
          detenerse();
          delay(500);
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
        delay(500);
        detenerse();      
        delay(500);             //paralelo al obstáculo hasta que cumpla el tiempoAvance máximo. 
        distancia = calcularDistancia ();
        if (distancia > umbral) {
          frenteLibre = true;
        } else {
          girarHorario ();
          delay (500);
          detenerse();
          delay(100);
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
  Serial.println("gira 180");
}
//distancia en 2 lugares??
void setup() {
  Serial.begin(115200);
  WiFi.softAPConfig(ip_rover, puerta_enlace, mascara);
  WiFi.softAP(nombre_wifi, contra);
  server.begin();
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
  Serial.println("\n\n\n===== NUEVA LECTURA =====\n\n\n");
  Serial.println("punto de acceso iniciado");
  Serial.print("ip: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  WiFiClient cliente = server.available();
  if (cliente) {
    while (!cliente.available()) {
      delay(1);
    }    
    String request = cliente.readStringUntil('\n');
    if (request.indexOf("/cambiodemodo") != -1) {
        reversa = false;
        manual = !manual;
        Serial.println("cambio de modo");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>cambio de modo</body></html>\n");
        detenerse ();
      }

    if (manual) {
      if (request.indexOf("/avanzar") != -1) {
        reversa = false;
        avanzar();
        Serial.println("avanza");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>avanza</body></html>\n");

      }
      if (request.indexOf("/doblarder") != -1) {
        reversa = false;
        girarDer();
        Serial.println("gira a la derecha");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>gira a la derecha</body></html>\n");
      }
      if (request.indexOf("/doblarizq") != -1) {
        reversa = false;
        girarIzq();
        Serial.println("gira a la izquierda");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>gira a la izquierda</body></html>\n");
      }
      if (request.indexOf("/detener") != -1) {
        reversa = false;
        detenerse();
        Serial.println("se detiene");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>se detiene</body></html>\n");
      }
      if (request.indexOf("/reversa") != -1) {
        reversa = true;
        marchaAtras();
        Serial.println("marcha atras");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>marcha atras</body></html>\n");

      }
      if (request.indexOf("/doblardratras") != -1) {
        reversa = true;
        girarDerMarchaAtras();
        Serial.println("dobla a la derecha marcha atras");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>dobla a la derecha marcha atras</body></html>\n");
      }
      if (request.indexOf("/doblariatras") != -1) {
        reversa = true;
        girarIzqMarchaAtras();
        Serial.println("dobla a la izquierda marcha atras");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>dobla a la izquierda marcha atras</body></html>\n");
      }
      if (request.indexOf("/girohor") != -1) {
        reversa = false;
        girarHorario();
        Serial.println("gira en sentido horario");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>gira en sentido horario</body></html>\n");
      }
      if (request.indexOf("/giroantihor") != -1) {
        reversa = false;
        girarAntiHorario();
        Serial.println("gira en sentido antihorario");
        cliente.println("HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n<html><body>gira en sentido antihorario</body></html>\n");
      }
    }
    cliente.flush(); 
  }
   if (!manual) {
    distancia = calcularDistancia();
    Serial.println ("distancia a obstáculo: " + String(distancia));
    if (distancia > umbral) {   
      Serial.println("avanza");
      avanzar();
      delay(50);
    } else {
        bool evadirIzq = girar90Izq ();
        if (evadirIzq == false) {
          detenerse ();
          delay (500);
          bool evadirDer = girar90Der ();
          if (evadirDer == false) {
            detenerse ();
            delay(500);
            marchaAtras();
            delay(500);
            giro180();
          }
        }
      } 
   }
  } 