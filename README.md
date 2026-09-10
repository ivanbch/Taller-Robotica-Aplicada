# Taller Robótica Aplicada - Rover ESP8266

Este repositorio contiene todo el código y herramientas para el Rover desarrollado durante el Taller de Robótica Aplicada de la Fundación INVAP y UNRN. El proyecto está enfocado en proporcionarle al rover capacidades de movimiento manual (vía app/scripts) y autónomo (evasión de obstáculos), utilizando un ESP8266.

## Estructura del Repositorio

Para mantener el proyecto organizado, los recursos se dividen en:

- 📂 `/firmware`: Contiene el proyecto de **PlatformIO** con el código C++ del ESP8266. El código está modularizado para mayor claridad (`motores`, `navegacion`, `conexion`).
- 📂 `/app`: Aplicaciones y recursos para Android (archivos `.apk` y proyecto de App Inventor `.aia`).
- 📂 `/scripts`: Scripts de Python para controlar el rover desde la computadora (por ejemplo, con un joystick PS4 o teclado).

## Requisitos y Compilación (Firmware)

El código del microcontrolador fue migrado a **PlatformIO** para tener un entorno de desarrollo más profesional y modular. 

1. Descargar e instalar [Visual Studio Code](https://code.visualstudio.com/).
2. Instalar la extensión de **PlatformIO**.
3. Abrir la carpeta `firmware/` desde VSCode.
4. Conectar el ESP8266 y darle a "Upload" (la flechita en la barra inferior de PIO).

## Módulos del Código (C++)

En la carpeta `firmware/include` encontrarás los headers que separan la lógica del rover:
- `conexion.h`: Maneja la creación del Access Point WiFi y el servidor web.
- `motores.h`: Configuración de pines para el driver de los motores.
- `navegacion.h`: Funciones de movimiento básico (avanzar, girar, retroceder).
- `autonomia.h`: Lógica y máquina de estados para la evasión de obstáculos con el sensor ultrasónico.
- `led.h`: Control básico del LED de estado.

## Uso

1. Al encender el ESP8266, este creará una red WiFi llamada **Robotin** (contraseña: `robotin123`).
2. Conéctate a la red con tu celular o PC.
3. Puedes utilizar la aplicación de Android (en la carpeta `/app`) o correr los scripts de Python (en `/scripts`) para enviar los comandos al Rover de manera manual, o indicarle que pase a modo autónomo.

---
**Autor:** Iván Bachmann.
