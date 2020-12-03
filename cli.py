from PyInquirer import prompt, style_from_dict, Token, Separator
from termcolor import colored
from pyfiglet import Figlet
import serial

ser = serial.Serial('COM16', 9600)

style = style_from_dict({
    Token.Separator: '#cc5454',
    Token.QuestionMark: '#673ab7',
    Token.Selected: '#cc5454',
    Token.Pointer: '#673ab7 bold',
    Token.Instruction: '',
    Token.Answer: '',
    Token.Question: '#f44336 bold',
})

question1 = {
    'type': 'list',
    'name': 'control',
    'message': 'Tipo de control:',
    'choices': ['Control manual', 'Control automatico', 'Inspeccionar CAN bus', 'Salir']
}

question2 = {
    'type': 'list',
    'name': 'element',
    'message': 'Elemento a controlar:',
    'choices': ['Medidor de gasolina', 'Tacometro', 'Lampara de aceite']
}

question3 = {
    'type': 'input',
    'name': 'number',
    'message': 'Ingrese revoluciones por minuto [0 - 3000]:',
}

question4 = {
    'type': 'input',
    'name': 'number',
    'message': 'Ingrese cantidad de mensajes a leer (0 - 256):',
}

question5 = {
    'type': 'list',
    'name': 'accion',
    'message': 'Accion a realizar:',
    'choices': ['Encender', 'Apagar']
}

question6 = {
    'type': 'input',
    'name': 'number',
    'message': 'Ingrese porcentaje de gasolina [0 - 100]:',
}

while(True):
    f = Figlet(font='slant')
    print(f.renderText('PROYECTO FINAL'))
    print(colored(
        "\t\t\tHecho por Mariano y Ernesto.\n\n", 'yellow', attrs=['bold']))
    tipo = prompt(question1, style=style)

    if tipo['control'] == 'Control manual':
        tipo = prompt(question2, style=style)
        if tipo['element'] == "Tacometro":
            vel = prompt(question3, style=style)
            lista = []
            lista.append(int(vel["number"]) // 256)
            lista.append(int(vel["number"]) % 256)

            ser.write("c".encode())
            x = ser.readline()
            # print("RESPONSE: ", x)
            ser.write(bytes(lista))
            x = ser.readline()
            # print("RESPONSE: ", x)

        elif tipo['element'] == "Medidor de gasolina":
            vel = prompt(question6, style=style)
            ser.write("b".encode())
            x = ser.readline()
            # print("RESPONSE: ", x)
            ser.write([int(vel["number"]) % 101])
            x = ser.readline()
            # print("RESPONSE: ", x)

        elif tipo['element'] == "Lampara de aceite":
            ser.write("d".encode())
            tipo = prompt(question5, style=style)
            x = ser.readline()
            # print("RESPONSE: ", x)
            ser.write([1 if tipo["accion"] == "Encender" else 0])
            x = ser.readline()
            # print("RESPONSE: ", x)

    elif tipo['control'] == "Control automatico":
        print("Se ejecuto comando de inicio")
        ser.write("a".encode())
        x = ser.readline()
        # print("RESPONSE: ", x)

    elif tipo['control'] == "Inspeccionar CAN bus":
        vel = prompt(question4, style=style)
        mensajes = int(vel["number"]) % 256
        ser.write("e".encode())
        ser.write(bytes([mensajes]))

        for i in range(mensajes):
            x = ser.readline()
            print("RESPONSE: ", x.decode('utf-8').strip())
    else:
        break
    print("\n")
