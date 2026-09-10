#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <navegacion.h>
bool reversa = false;
bool manual = true; 
const char* nombre_wifi = "Robotin";
const char* contra = "robotin123";
IPAddress ip_rover(192,168,4,1);       // ip del rover (confirmar que pueda servir la misma)
IPAddress puerta_enlace(192,168,4,1);        // Gateway (igual que la IP en modo punto de acceso)
IPAddress mascara(255,255,255,0);        // Máscara de subred
//IPAddress ip_rover (192,168,253,10);        // (si quiero usar el hotspot como router)
//IPAddress ip_rover (192,168,137,10);        // (si quiero usar mi pc como router)
//IPAddress puerta_enlace (192,168,253,154); // (si quiero usar el hotspot como router)
//IPAddress puerta_enlace (192,168,137,1); // (si quiero usar mi pc como router)
//IPAddress mascara(255,255,255,0);
WiFiServer server(80);
WiFiClient cliente = server.available();

void iniciarWifi() {
  WiFi.softAPConfig(ip_rover, puerta_enlace, mascara);
  WiFi.softAP(nombre_wifi, contra);
  server.begin();
  Serial.println("\n\n\n===== NUEVA LECTURA =====\n\n\n");
  Serial.println("punto de acceso iniciado");
  Serial.print("ip: ");
  Serial.println(WiFi.softAPIP());
}
void manejarMiCliente () {
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
}