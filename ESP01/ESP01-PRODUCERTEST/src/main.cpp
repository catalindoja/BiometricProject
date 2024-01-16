/*
 * MQTT Client ESP8266
 *
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "MadnessPortal";
const char *password = "1223334444";
const char *mqtt_server = "192.168.4.1";

#define PIN_LED LED_BUILTIN

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect();

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  // Conectar a la red WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to ESP01 AP...");
  }

  Serial.println("Connected to ESP01 AP!");

  client.setServer(mqtt_server, 1883);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  // Publicar un mensaje en el tema "esp01/message"
  client.publish("esp01/message", "Hola desde el cliente MQTT");
  Serial.println("Sended messaged to: esp01/message");

  delay(5000);
}

void reconnect()
{
  // Loop hasta que nos reconectemos
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("Test4321"))
    {
      Serial.println("Conectado al broker MQTT");
    }
    else
    {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando nuevamente en 5 segundos");
      // Esperar 5 segundos antes de volver a intentar
      delay(5000);
    }
  }
}
