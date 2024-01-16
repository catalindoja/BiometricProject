/*
 * uMQTTBroker and AP Creator
 *
 * Minimal Demo: the program simply starts a broker and waits for any client to connect.
 */

#include <ESP8266WiFi.h>

/*
 * Your WiFi config here
 */
char ssid[] = "MadnessPortal"; // your network SSID (name)
char pass[] = "1223334444";    // your network password

int clientsCounter = 0;

#define PIN_LED LED_BUILTIN

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  Serial.begin(115200);

  WiFi.softAP(ssid, pass);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void loop()
{
}
