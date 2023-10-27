import paho.mqtt.client as mqtt
import json
import datetime

# Informações de conexão ao broker MQTT
mqtt_broker = ''
mqtt_port = 1884
usernameMQTT = "";          # Username para se conectar ao servidor MQTT
passwordMQTT = "";  # Password para se conectar ao servidor MQTT
topic = '337/+'

def saveToArqValues(sensor_data):
    current_datetime = datetime.datetime.now()
    with open("filename.txt", "a") as file:
        file.write(
            str(current_datetime)
            + " "
            + str(sensor_data["lightSensor"])
            + " "
            + str(sensor_data["ultrasonicSensor"])
            + " "
            + str(sensor_data["pirSensor"])
            + " "
            + str(sensor_data["soundSensor"])
            + "\n"
        )

def saveToArqPresence(data):
    current_datetime = datetime.datetime.now()
    with open("filename2.txt", "a") as file:
        file.write(
            str(current_datetime)
            + " "
            + str(data)
            + "\n"
        )


# Callback chamada quando uma conexão é estabelecida com o broker MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado ao broker MQTT com código de resultado:", rc)
    # Subscreve ao tópico após se conectar
    client.subscribe(topic)

# Callback chamada quando uma mensagem é recebida no tópico subscrito
def on_message(client, userdata, msg):
    print(f"Recebido mensagem no tópico {msg.topic}: {json.loads(msg.payload.decode('utf-8'))}")
    if(msg.topic == '337/values'):
        saveToArqValues(json.loads(msg.payload.decode('utf-8')))
    elif (msg.topic == '337/presence'):
        saveToArqPresence(json.loads(msg.payload.decode('utf-8')))

# Configuração do cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Autenticação no broker MQTT:
# client.username_pw_set(usernameMQTT, passwordMQTT)

# Conecta ao broker MQTT
client.connect(mqtt_broker, mqtt_port)

# Inicia o loop de comunicação com o broker MQTT
client.loop_forever()
