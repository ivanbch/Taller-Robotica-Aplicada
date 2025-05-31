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
int tiempoGiro = 0;
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
}

void girarDerMarchaAtras () { //rover gira hacia la derecha marcha atrás; revisar sentido
  girarAtras (motor_izq);
  frenar (motor_der);
}

void girarHorario () {
  girarIzq ();
  girarDerMarchaAtras ();
}

void girarAntiHorario () {
  girarDer ();
  girarIzqMarchaAtras ();
}

void setup() {
  pinMode(Echo, INPUT);
  pinMode(Trigger, OUTPUT); //
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
  unsigned long millisActual = millis();
  WiFiClient cliente= server.available();
  if (cliente) {
    while (!cliente.available()) {
      delay(1);
    }
    String request = cliente.readStringUntil('\r'); // Ignora la solicitud
    cliente.flush();
     if (manual) {
      if (request.indexOf("/avanzar") != -1) {
        reversa = false;
          avanzar ();
        }
        
      if (request.indexOf("/doblarder") != -1) {
        reversa = false;
        girarDer ();
      }
      if (request.indexOf("/doblarizq") != -1) {
        reversa = false;
        girarIzq ();
      }
      if (request.indexOf("/girohor") != -1) {
        reversa = false;
        girarHorario ();
      }
      if (request.indexOf("/giroantihor") != -1) {
        reversa = false;
        girarAntiHorario ();
      }
      if (request.indexOf("/detener") != -1) {
        reversa = false;
        detenerse ();
      }
      if (request.indexOf("/reversa") != -1) {
        reversa = true;
        marchaAtras ();
      }
      if (request.indexOf("/doblarderatras") != -1) {
        reversa = true;
        girarDerMarchaAtras ();
      }
      if (request.indexOf("/doblarizqatras") != -1) {
        reversa = true;
        girarIzqMarchaAtras ();
      }
  
      if (reversa) {
        if (millisActual - millisAnterior >= intervalo) {
            millisAnterior = millisActual;
            if (estadoLed == LOW) {
              estadoLed = HIGH;
            } else {
                estadoLed = LOW;
              }
        digitalWrite(pinLed, estadoLed);
      } else {
          digitalWrite(pinLed, LOW);
        }
        
      }
      if (request.indexOf("/cambiodemodo") != -1) {
        reversa = false;
        manual = !manual;
      }
    //Automático
     } else {
        tiempoGiro = 0;
        if (request.indexOf("/cambiodemodo") != -1) {
          reversa = false;
          manual = !manual;
        }
        if (calcularDistancia () > 15) {
          reversa = false;
          avanzar ();
        } else {
            reversa = false;
            tiempoGiro = 0;
            while (calcularDistancia () < 15 and tiempoGiro < 500) {
              girarHorario ();
              tiempoGiro +=1;
            }
            if (calcularDistancia() < 15) {
              tiempoGiro = 0;
              reversa = false;
              while (calcularDistancia () < 15 and tiempoGiro < 1000) {
                girarAntiHorario ();
                tiempoGiro +=1;
              } 
              tiempoGiro = 0;
            if (calcularDistancia() < 15) {     //poner como varible booleana si hay o no objeto es mas facil
                while (tiempoGiro < 500){
                  girarHorario ();
                  tiempoGiro +=1;
                }
                while (tiempoGiro < 2000) {
                marchaAtras();
                tiempoGiro +=1;
                }
              	
            } 
            }
            
     }
    cliente.println("HTTP/1.1 200 OK");
    cliente.println("Content-Type: text/html");
    cliente.println();
    cliente.println("<html><body>OK</body></html>");
    cliente.stop();
  }
  }
}
