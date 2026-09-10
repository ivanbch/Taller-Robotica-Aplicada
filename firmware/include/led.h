#include <Arduino.h>
#define pinLed D4
void prenderLed () {
  digitalWrite(pinLed,LOW);
}
void apagarLed () {
  digitalWrite(pinLed,HIGH);
}
