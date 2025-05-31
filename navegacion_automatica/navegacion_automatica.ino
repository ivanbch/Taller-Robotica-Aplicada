#include <ESP8266WiFi.h>
#define motor_izq_a D8
#define motor_izq_b D7
#define motor_der_a D6
#define motor_der_b D5
#define pinLed 2
#define Trigger D2
#define Echo D1

bool reversa = false;
const char* nombre_wifi = "Robotin";
const char* contra = "robotin123";
long duracion;
int distancia;
bool manual = true; 
int tiempoGiro = 0;
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

void setup() {
  Serial.begin(9600);
  WiFi.softAPConfig(ip_rover, puerta_enlace, mascara);
  WiFi.softAP(nombre_wifi, contra);
  Serial.println("punto de acceso iniciado");
  Serial.print("ip: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  //WiFi.config(ip_rover, puerta_enlace, mascara);
  pinMode(pinLed,OUTPUT);
  pinMode(motor_izq_a,OUTPUT);
  pinMode(motor_izq_b,OUTPUT);
  pinMode(motor_der_a,OUTPUT);
  pinMode(motor_der_b,OUTPUT);
  manual = true;
  apagarLed();
  //configurarMotor (motor_izq);
  //configurarMotor (motor_der);
  /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print ("Sin conexión");
  }
  server.begin();
  Serial.println("la ip del rover es:");
  Serial.println(WiFi.localIP());
  */
}

void loop() {
  tiempoGiro = 0;
  unsigned long millisActual = millis();
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
      
    } //modo autónomo
    else {
      if (calcularDistancia() > 15) {
        avanzar ();
        Serial.println("avanza");
      } else {
        while (calcularDistancia() < 15 && tiempoGiro < 500) {
          girarHorario ();
          if (tiempoGiro % 500 == 0) {
            Serial.println("gira horario");
          } 
            
          tiempoGiro +=1;
        } if (calcularDistancia() > 15) {
            while (calcularDistancia() > 15 && tiempoGiro < 500) {
            avanzar ();
            if (tiempoGiro % 500 == 0) {
              Serial.println("avanzar");
            } 
          tiempoGiro +=1;
          if (calculardistancia > 15 ) {
            while (calcularDistancia() < 15 && tiempoGiro < 500) {
              girarAntiHorario ();
              if (tiempoGiro % 500 == 0) {
                Serial.println("gira antihorario");
              } 
            }
          } else {
            tiempoGiro = 0;
            while (calcularDistancia() < 15 && tiempoGiro < 1000) {
              girarAntiHorario ();
              if (tiempoGiro % 500 == 0) {
                Serial.println("gira antihorario");
              } 
              tiempoGiro +=1;
            }
            tiempoGiro = 0;
            while (calcularDistancia() < 15 && tiempoGiro < 2000) {
              marchaAtras ();
              if (tiempoGiro % 500 == 0) {
                Serial.println("marcha atras");
              } 
              tiempoGiro +=1;
            }
        }
        
      }
    }
  }
  cliente.flush();
}
