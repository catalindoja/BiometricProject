#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "MadnessPortal";
const char *password = "1223334444";
const char *mqtt_server = "192.168.4.2";

// Nueva IP estática
IPAddress staticIP(192, 168, 4, 10);

#define PIN_LED LED_BUILTIN

WiFiClient espClient;
PubSubClient client(espClient);

String incomingMessage = "";

void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  // Configurar IP estática
  WiFi.config(staticIP, IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 4, 1));
  
  // Conectar a la red WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to ESP01 AP...");
  }

  Serial.println("Connected to ESP01 AP!");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  delay(500);
}

void reconnect()
{
  // Loop hasta que nos reconectemos
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("ESP01Consumer"))
    {
      Serial.println("Conectado al broker MQTT");
      // Suscribirse al tema después de la conexión exitosa
      client.subscribe("esp01/message");
    }
    else
    {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando nuevamente en 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en el tema: ");
  Serial.println(topic);

  Serial.print("Contenido del mensaje: ");
  incomingMessage = "";  // Reiniciar la variable antes de acumular el nuevo mensaje

  for (int i = 0; i < length; i++) {
    incomingMessage += (char)payload[i];
  }

  Serial.println(incomingMessage);
}
