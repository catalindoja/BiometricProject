// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Import the InfluxDB libraries
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define LED_PIN D4

const int Trigger = D0; // Pin digital 2 para el Trigger del sensor
const int Echo = D4;    // Pin digital 0 para el Echo del sensor
const char *ssid = "MadnessPortal";
const char *password = "1223334444";
const char *mqtt_server = "192.168.4.2";

#define PIN_LED LED_BUILTIN

// InfluxDB v2 server url (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "http://192.168.4.2:8086/"

// InfluxDB v2 server or cloud API authentication token (Use: InfluxDB UI -> Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN "JQ9mU8IKw_x8j0O_CBZMAmpMft6Xtpaq0QpSyTcy7c1YYBlbSdZNlVZDp-5SJEber3E02_S_uDCJzi-9uNZAow=="

// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "SamerTeam"

// InfluxDB v2 bucket name (Use: InfluxDB UI -> Data -> Buckets)
#define INFLUXDB_BUCKET "IoT-APP"

// Set timezone string according to <https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html>
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Create your Data Point here
Point sensor("ultrasound");

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

  String text = String(d);
  sensor.addField("humidity", d);
  client.publish("esp01/ultrasound", text.c_str());
  Serial.println("Sended messaged to: esp01/ultrasound");

  // Store data using influxdb client
  // Check server connection
  if (!influxClient.validateConnection())
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }

  if (!influxClient.writePoint(sensor))
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  } else {
    Serial.println("InfluxDB write success");
  }

  delay(500);
}

void reconnect()
{
  // Loop hasta que nos reconectemos
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("UltrasoundSensorProducer"))
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