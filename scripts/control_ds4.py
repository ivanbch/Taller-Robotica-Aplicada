import requests
from inputs import get_gamepad

ROVER_IP = "192.168.4.1"  # Cambiar esto por la ip del rover

# Relaciona movimientos del D-Pad a comandos HTTP
DPAD_COMMANDS = {
    ('ABS_HAT0Y', -1): 'avanzar',    # Flecha arriba
    ('ABS_HAT0Y', 1): 'reversa',     # Flecha abajo
    ('ABS_HAT0X', -1): 'doblarizq',  # Flecha izquierda
    ('ABS_HAT0X', 1): 'doblarder',   # Flecha derecha
}

def send_command(command):
    url = f"http://{ROVER_IP}/{command}"
    try:
        requests.get(url, timeout=0.2)
        print(f"Enviado: {command}")
    except Exception as e:
        print(f"Error enviando {command}: {e}")

def main():
    print("Controla el rover con las flechas del pad. Ctrl+C para salir.")
    last_states = {'ABS_HAT0X': 0, 'ABS_HAT0Y': 0}
    last_combo = None
    while True:
        events = get_gamepad()
        for event in events:
            if event.code in ['ABS_HAT0X', 'ABS_HAT0Y']:
                if event.state != last_states[event.code]:
                    key = (event.code, event.state)
                    if key in DPAD_COMMANDS:
                        send_command(DPAD_COMMANDS[key])
                    elif event.state == 0:
                        send_command('detener')  # Detener si suelto la flecha
                    last_states[event.code] = event.state

        # Detectar combinaciones de flechas para doblar hacia atrás
        if last_states['ABS_HAT0Y'] == 1 and last_states['ABS_HAT0X'] == 1:
            if last_combo != "doblarderatras":
                send_command("doblarderatras")
                last_combo = "doblarderatras"
        elif last_states['ABS_HAT0Y'] == 1 and last_states['ABS_HAT0X'] == -1:
            if last_combo != "doblarizqatras":
                send_command("doblarizqatras")
                last_combo = "doblarizqatras"
        else:
            last_combo = None

if __name__ == "__main__":
    main()