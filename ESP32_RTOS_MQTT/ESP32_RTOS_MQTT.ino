#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
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

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;
int pinServo = 14;
int pos = 0;

void ServoTask(void *pvParameters) {
  while(1) {
    // Check if there's available data on the Serial port
    
      // Read the incoming bytes as a string until newline character
      String input = incomingMessage;
      // Convert the string to an integer
      int degrees = input.toInt();
      // Move the servo to the specified position
      servo.write(degrees);
      vTaskDelay(pdMS_TO_TICKS(2000));
      servo.write(10);
      vTaskDelay(pdMS_TO_TICKS(2000));
    
    vTaskDelay(pdMS_TO_TICKS(300)); // Small delay to prevent task starvation
  }
}

void LCDTask(void *pvParameters) {
  while(1) {
    // Check if there's available data on the Serial port
    
      // Read the incoming bytes as a string until newline character
      
      // Display the received value on the LCD
      lcd.setCursor(0, 0);
      lcd.print("Received value: ");
      lcd.setCursor(0, 1);
      lcd.print(incomingMessage);
      vTaskDelay(pdMS_TO_TICKS(1000));
      lcd.clear();
    
    vTaskDelay(pdMS_TO_TICKS(300)); // Small delay to prevent task starvation
  }
}

void MQTTTask(void *pvParameters) {
  // MQTT setup and data reception logic
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  servo.attach(pinServo, 500, 2500);

  //mqtt
  Serial.begin(115200);

  // Configurar IP estática
  WiFi.config(staticIP, IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 4, 1));
  
  // Conectar a la red WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.println("Connecting to ESP01 AP...");
  }

  Serial.println("Connected to ESP01 AP!");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  xTaskCreate(ServoTask, "ServoTask", 2048, NULL, 1, NULL);
  xTaskCreate(LCDTask, "LCDTask", 2048, NULL, 2, NULL);
  //xTaskCreate(MQTTTask, "MQTTTask", 2048, NULL, 3, NULL);
}

void loop() {
  // Code that doesn't require FreeRTOS handling
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  vTaskDelay(pdMS_TO_TICKS(500));
}





//////////////////

void reconnect()
{
  // Loop hasta que nos reconectemos
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("ESP32"))
    {
      Serial.println("Conectado al broker MQTT");
      // Suscribirse al tema después de la conexión exitosa
      client.subscribe("esp01/ultrasound");
    }
    else
    {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando nuevamente en 5 segundos");
      vTaskDelay(pdMS_TO_TICKS(5000));
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

