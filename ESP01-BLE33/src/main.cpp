// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Import the InfluxDB libraries
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

const char *ssid = "MadnessPortal";
const char *password = "1223334444";
const char *mqtt_server = "192.168.4.2";
unsigned char buf[3] = {0};
unsigned char start[1] = {0};

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
Point hrSensor("hearthRateField");
Point gsrSensor("gsrSensorField");

WiFiClient espClient;
PubSubClient client(espClient);
IPAddress staticIP(192, 168, 4, 12);

void reconnect();

void setup()
{
  Serial.begin(115200);

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

  // Loop hasta que nos reconectemos
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("NANO33Producer"))
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

  while (!Serial.available())
    ;
  while (Serial.available())
  {
    Serial.readBytes(start, 1);
    Serial.println("Start: " + String(start[0]));
    if (start[0] == 111)
    {

      Serial.readBytes(buf, 3);

      Serial.println("Received: " + String(buf[0]) + " " + String(buf[1]) + " " + String(buf[2]));

      // Working
      String heartRate = String(buf[0]);
      String gsr = String(buf[1] * 256 + buf[2]);

      hrSensor.addField("heartRate", heartRate.toInt());
      gsrSensor.addField("gsr", gsr.toInt());

      // Publish message to MQTT broker
      client.publish("nano33/healthInfo", heartRate.c_str());
      Serial.println("Sent message to: nano33/healthInfo");
      client.publish("nano33/gsr", gsr.c_str());
      Serial.println("Sent message to: nano33/gsr");

      // Store data using influxdb client
      // Check server connection
      if (!influxClient.validateConnection())
      {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(influxClient.getLastErrorMessage());
      }

      if (!influxClient.writePoint(hrSensor))
      {
        Serial.print("InfluxDB write failed: ");
        Serial.println(influxClient.getLastErrorMessage());
      }

      if (!influxClient.writePoint(gsrSensor))
      {
        Serial.print("InfluxDB write failed: ");
        Serial.println(influxClient.getLastErrorMessage());
      }

      if (!influxClient.writePoint(hrSensor))
      {
        Serial.print("InfluxDB write failed: ");
        Serial.println(influxClient.getLastErrorMessage());
      }
    }
  }
}

void reconnect()
{
  // Loop hasta que nos reconectemos
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("NANO33Producer"))
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
