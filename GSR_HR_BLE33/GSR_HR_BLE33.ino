#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;
unsigned char toSend[4];

const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];    // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;        // Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
long currentMoment = 0;

void setup() {
  Wire.begin(); // Join I2C bus
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();                   // Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED
}

void loop() {
  toSend[3] = 111;
  long irValue = particleSensor.getIR();
  int state = analogRead(A0);
  
  toSend[1] = (unsigned char)(state / 256);
  toSend[2] = (unsigned char)(state % 256);

  if (checkForBeat(irValue) == true) {
    // We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
      rateSpot %= RATE_SIZE;                    // Wrap variable

      // Take average of readings
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }


  if((millis()-currentMoment) > 500){
  Serial.print("Galvanic Skin Resistance=");
  Serial.print(state);
  Serial.println();
  Serial.print(", BPM=");
  toSend[0] = (unsigned char)beatsPerMinute;
  Serial.println(beatsPerMinute);
  Serial.println("Sent: " + String(toSend[0]) + " " + String(toSend[1]) + " " + String(toSend[2])+ " " + String(toSend[3]));
  Serial1.write(toSend, 4);
  Serial1.flush();
  currentMoment = millis();
  Serial.println();
 
  }
   
 



  
}
