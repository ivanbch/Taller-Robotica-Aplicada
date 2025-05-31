from flask import Flask, render_template_string, redirect, url_for

app = Flask(__name__)

estado = "Detenido"

TEMPLATE = '''
<!DOCTYPE html>
<html>
<head>
    <title>Control de Prueba</title>
</head>
<body>
    <h1>Estado actual: {{ estado }}</h1>
    <form method="post" action="{{ url_for('cambiar_estado', nuevo_estado='Avanzando') }}">
        <button type="submit">Avanzar</button>
    </form>
    <form method="post" action="{{ url_for('cambiar_estado', nuevo_estado='Detenido') }}">
        <button type="submit">Detenerse</button>
    </form>
    <form method="post" action="{{ url_for('cambiar_estado', nuevo_estado='Girando a la Izquierda') }}">
        <button type="submit">Girar Izquierda</button>
    </form>
    <form method="post" action="{{ url_for('cambiar_estado', nuevo_estado='Girando a la Derecha') }}">
        <button type="submit">Girar Derecha</button>
    </form>
</body>
</html>
'''

@app.route('/', methods=['GET'])
def index():
    global estado
    return render_template_string(TEMPLATE, estado=estado)

@app.route('/cambiar/<nuevo_estado>', methods=['POST'])
def cambiar_estado(nuevo_estado):
    global estado
    estado = nuevo_estado
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(host='localhost', port=5000)