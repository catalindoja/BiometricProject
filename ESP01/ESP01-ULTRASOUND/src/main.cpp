// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED_PIN D4

const int Trigger = D0; // Pin digital 2 para el Trigger del sensor
const int Echo = D4;    // Pin digital 0 para el Echo del sensor
const char *ssid = "MadnessPortal";
const char *password = "1223334444";
const char *mqtt_server = "192.168.4.2";

#define PIN_LED LED_BUILTIN

WiFiClient espClient;
PubSubClient client(espClient);
IPAddress staticIP(192, 168, 4, 11);

void reconnect();

void setup()
{
  Serial.begin(115200);       // iniciailzamos la comunicación
  pinMode(Trigger, OUTPUT);   // pin como salida
  pinMode(Echo, INPUT);       // pin como entrada
  digitalWrite(Trigger, LOW); // Inicializamos el pin con 0

  // Conectar a la red WiFi
  WiFi.config(staticIP, IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 4, 1));
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

  long t; // timepo que demora en llegar el eco
  long d; // distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10); // Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); // obtenemos el ancho del pulso
  d = t / 59;              // escalamos el tiempo a una distancia en cm

  String text = "Distancia: " + String(d) + "cm";
  client.publish("esp01/message", text.c_str());
  Serial.println("Sended messaged to: esp01/message");

  delay(500);
}

void reconnect()
{
  // Loop hasta que nos reconectemos
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("ESP01Producer"))
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