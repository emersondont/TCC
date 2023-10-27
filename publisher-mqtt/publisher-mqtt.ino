#include <WiFi.h>
#include <PubSubClient.h>
#include "Ultrasonic.h"
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include <TimeLib.h>

// Variáveis e objetos para a conexão com a rede WiFi e o servidor MQTT
const char *ssid = "";         // Nome da rede WiFi que a ESP32 deve se conectar
const char *password = "";     // Senha da rede WiFi correspondente ao 'ssid'
const char *mqtt_server = "";  // Endereço IP do servidor MQTT ao qual a ESP32 deve se conectar
const int broker_port = 1884;  // Número da porta do servidor MQTT a ser utilizada na conexão
const char *usernameMQTT = ""; // Username para se conectar ao servidor MQTT
const char *passwordMQTT = ""; // Password para se conectar ao servidor MQTT
const char *topic = "337";        // Tópico que a ESP32 vai publicar as mensagens

// Pinagem dos sensores na ESP32
const int lightSensor = 39;
const int pirSensor = 32;
const int soundSensor = 36;
const int pinTrigger = 4;
const int pinEcho = 5;

// variaveis para definir o threshold do ultrasonic sensor
const int ultrasonicWindowSize = 10;
int ultrasonicValues[ultrasonicWindowSize];
int ultrasonicValuesIndex = 0;

// variaveis de controle de presença
time_t lastPresenceTime = 0;
bool noise = false;

// variavel para controle de publicações
int publishCounter = 0;
const int timeout = 500;        // tempo que a esp vai demorar para verificar os sensores de novo
const int publishInterval = 10; // numero de interações que a esp vai fazer antes de publicar os dados - 10 * 500ms = 5s

// DynamicJsonDocument sensorValues;  // Armazena os valores dos sensores

// Vetor com os nomes dos sensores
std::vector<String> nameSensorsVector = {
    "lightSensor",
    "ultrasonicSensor",
    "pirSensor",
    "soundSensor"};

WiFiClient espClient;           // Utilizada para a comunicação pela rede WiFi
PubSubClient client(espClient); // Gerencia a comunicação MQTT

Ultrasonic ultrasonic(pinTrigger, pinEcho); // Utilizado para ler os dados do sensor Ultrasonic

/* FUNÇÕES PARA SE CONECTAR AO WIFI E AO BROKER */
// Função para se conectar ao WiFi
void initWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// Função para se conectar ao broker MQTT
void reconnect()
{
  // Loop até se reconectar
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Tentando se conectar ao broker MQTT
    if (client.connect("ESP32-337"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 second");
      delay(1000);
    }
  }
}

/* FUNÇÕES PARA LER DADOS DOS SENSORES */

// Função para ler a distância do sensor Ultrasonic
int getDistanceHcsr04()
{
  digitalWrite(pinTrigger, LOW);  // SETA O PINO COM UM PULSO BAIXO "LOW"
  delayMicroseconds(2);           // INTERVALO DE 2 MICROSSEGUNDOS
  digitalWrite(pinTrigger, HIGH); // SETA O PINO COM PULSO ALTO "HIGH"
  delayMicroseconds(10);          // INTERVALO DE 10 MICROSSEGUNDOS
  digitalWrite(pinTrigger, LOW);  // SETA O PINO COM PULSO BAIXO "LOW" NOVAMENTE
  // Função Ranging, faz a conversão do tempo de resposta do ECHO em centímetros, e armazena na variável "distancia"
  int distancia = ultrasonic.Ranging(CM); // Variável global recebe o valor da distância medida
  delay(500);                             // INTERVALO DE 500 MILISSEGUNDOS
  return distancia;
}

// Função que retorna um JSON com os dados de todos os sensores
DynamicJsonDocument getValues()
{
  int valueUltrasonic = getDistanceHcsr04();
  int valueLight = analogRead(lightSensor);
  int valuePir = analogRead(pirSensor);

  // Loop para pegar um intervalo de tempo de dados do sensor de som/ruido
  long valueSound = 0;
  for (int i = 0; i < 32; i++)
  {
    valueSound += analogRead(soundSensor);
    delay(10);
  }
  valueSound >>= 5;

  // Armazena tudo em um JSON
  DynamicJsonDocument jsonDoc(128);
  jsonDoc["lightSensor"] = valueLight;
  jsonDoc["ultrasonicSensor"] = valueUltrasonic;
  jsonDoc["pirSensor"] = valuePir;
  jsonDoc["soundSensor"] = valueSound;

  return jsonDoc;
}

//função para publicar os dados dos sensores no broker mqtt
void publish(DynamicJsonDocument values)
{
  char buffer[5];
  String jsonString;
  String subtopic = "values";

  String topic_subtopic = topic + subtopic;

  serializeJson(values, jsonString);
  client.publish(topic_subtopic, jsonString.c_str());
}

// função que calcula o threshold do sensor ultrassonico
int calculateUltrasonicAverage()
{
  int sum = 0;
  for (int i = 0; i < ultrasonicWindowSize; i++)
  {
    sum += ultrasonicValues[i];
  }

  return sum / ultrasonicWindowSize;
}

bool checkPresence(DynamicJsonDocument values)
{
  // Define os limiares para cada sensor
  const int lightThreshold = 1000;
  const int pirThreshold = 4000;
  const int soundThreshold = 100;

  // se está acima de 2000 é porque tem ruido
  if (values["ultrasonicSensor"] > 2000)
  {
    values["ultrasonicSensor"] = ultrasonicValues[ultrasonicValuesIndex - 1]; // pegando o valor anterior do sensor ultrassonico(valor sem ruido)
  }

  int ultrasonicThreshold = calculateUltrasonicAverage();

  // Luz está acesa
  if (values["lightSensor"] > lightThreshold)
  {
    if (values["ultrasonicSensor"] < ultrasonicThreshold - 10 || values["pirSensor"] > pirThreshold)
    {
      lastPresenceTime = now();
      return true; // Luz está acesa e há alguém na sala
    }
    else if (lastPresenceTime + 600 < now())
    {
      return false; // Luz está acesa e não detectou presença a pelo menos 600 segundos
    }
    else
    {
      return true; // Luz está acesa e detectou presença a pelo menos 600 segundos
    }
  }
  else if (values["ultrasonicSensor"] < ultrasonicThreshold - 10 || values["pirSensor"] > pirThreshold)
  {
    if (lastPresenceTime + 10 > now())
    {
      lastPresenceTime = now();
      noise = false; // não é ruido
      return true;   // Luz está apagada e há alguém na sala
    }
    lastPresenceTime = now();
    noise = true; // é ruido
    return false; // Luz está apagada e a presença que detectou provavelmente é ruido
  }
  else if (lastPresenceTime + 30 > now() && !noise)
  {
    return true; // Luz está apagada e detectou presença a menos de 12 segundos que não é ruido
  }
  else
  {
    return false; // Luz está apagada e não detectou presença a pelo menos 12 segundos
  }

  // Armazena o novo valor do sensor ultrassonico na janela de amostragem
  ultrasonicValues[ultrasonicValuesIndex] = newValue;
  ultrasonicValuesIndex = (ultrasonicValuesIndex + 1) % ultrasonicWindowSize;
}

void setup()
{
  Serial.begin(115200);
  // Inicializa os pinos dos sensores
  pinMode(pinEcho, INPUT);     // DEFINE O PINO COMO ENTRADA (RECEBE)
  pinMode(pinTrigger, OUTPUT); // DEFINE O PINO COMO SAÍDA (ENVIA)
  // Inicializa o vetor ultrasonicValues com zeros
  for (int i = 0; i < ultrasonicWindowSize; i++)
  {
    ultrasonicValues[i] = 0;
  }
  // Inicializa a conexão WiFi e a conexão com o broker MQTT
  initWiFi();
  client.setServer(mqtt_server, broker_port);
}

void loop()
{
  // Se conectando ao broker MQTT
  if (!client.connected())
    reconnect();

  // Lê os dados dos sensores
  DynamicJsonDocument values = getValues();
  // verifica se há presença na sala
  bool presence = checkPresence(values);

  String subtopic = "presence";
  String topic_subtopic = topic + subtopic;

  // Publica os dados dos sensores no broker MQTT
  publish(values);
  if (publishCounter == publishInterval)
  {

    // publica dado do sensor virtual
    if (presence)
    {
      client.publish(topic_subtopic, "1");
    }
    else
    {
      client.publish(topic_subtopic, "0");
    }

    publishCounter = 0;
  }

  publishCounter++;

  delay(timeout);
}
