from flask import Flask

app = Flask(__name__)

@app.route('/avanzar')
def avanzar():
    msj = "avanza"
    print (msj)
    return "avanza"

@app.route('/reversa')
def reversa():
    msj = "marcha atrás"
    print (msj)
    return "marcha atrás"

@app.route('/detener')
def detener():
    msj = "se detiene"
    print (msj)
    return "se detiene"
    
@app.route('/doblarder')
def doblarder():
    msj = "dobla a la derecha"
    print (msj)
    return "dobla a la derecha"
    
@app.route('/doblarizq')
def doblarizq():
    msj = "dobla a la izquierda"
    print (msj)
    return "dobla a la izquierda"

@app.route('/doblarizqatras')
def doblarizqatras():
    msj = "dobla a la izquierda para atrás"
    print (msj)
    return "dobla a la izquierda para atrás"

@app.route('/doblarderatras')
def doblarderatras():
    msj = "dobla a la derecha para atrás"
    print (msj)
    return "dobla a la derecha para atrás"


if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000)
    app.run(host='0.0.0.0', port=5000)