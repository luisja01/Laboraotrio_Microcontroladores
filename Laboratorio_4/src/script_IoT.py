'''Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Laboratorio 4
Sismógrafo
Archivo para envío de datos a servidor IoT
Ciclo: II-2022
'''

#Se importan las librerias
import serial, time, json
import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        client.connected = True
        print("Conexión exitosa")
    else: 
        print("No conectado, código: ", rc)
        client.loop_stop()
def on_disconnect(client, userdata, rc):
    if(rc == 0):
        print("Desconexión exitosa")
    else:
        print("Sistema desconectado mediante el código: ", rc)
def on_publish(client, userdata, mid):
    print("Mensaje: ", mid, " ha abandonado el cliente")


datos = serial.Serial("/dev/ttyACM1",115200,timeout=1) 
print("Conectado al puerto serial /dev/ttyACM1")
client = mqtt.Client("B93840")
client.connected = False
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish

broker ="iot.eie.ucr.ac.cr"
port = 1883
topic = "v1/devices/me/telemetry"
device = "KxFqn3GMc1Rkq7aojP0C"
client.username_pw_set(device)
client.connect(broker, port)
dict = dict()
while client.connected != True:
    client.loop()
    time.sleep(2)

while (1):
    data = datos.readline().decode('utf-8').replace('\r', "").replace('\n', "")
    data = data.split('\t')
    dict["Eje X"] = data[0]
    dict["Eje Y"] = data[1]
    dict["Eje Z"] = data[2]
    dict["Voltaje de Bateria"] = data[3]

    if(float( data[3]) < 7):
        dict["Bateria Baja"] = "Si"
    else:
        dict["Bateria Baja"] = "No"
    
    output = json.dumps(dict)
    print(output)
    client.publish(topic, output)
    time.sleep(5)
    
