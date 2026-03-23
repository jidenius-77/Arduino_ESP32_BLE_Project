// ARDUINO UNO R4 WIFI

#include <ArduinoBLE.h>

// UUID https://www.uuidgenerator.net/
BLEService customService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEIntCharacteristic levelCharacteristic(
  "19B10001-E8F2-537E-4F6C-D104768A1214",
  BLERead | BLENotify
);

int sensorValue = 0;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(9600);

  if (!BLE.begin()) {
    Serial.println("Kunde inte starta BLE!");
    while (1);
  }

  BLE.setLocalName("Saker_Sensor");
  BLE.setAdvertisedService(customService);

  customService.addCharacteristic(levelCharacteristic);
  BLE.addService(customService);

  levelCharacteristic.writeValue(sensorValue);

  BLE.advertise();

  Serial.println("Arduino annonserar och väntar på klient...");
}

void loop() {
  BLEDevice central = BLE.central();

  // När en klient ansluter
  if (central) {
    Serial.print("Ansluten till: ");
    Serial.println(central.address());

    // Kör så länge klienten är ansluten
    while (central.connected()) {

      // Skicka var 2 sek (utan delay-block)
      if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();

        sensorValue++;

        levelCharacteristic.writeValue(sensorValue); // notify

        Serial.print("Skickar: ");
        Serial.println(sensorValue);
      }

      // Låt BLE jobba
      BLE.poll();
    }

    Serial.println("Klient kopplade ner");
  }
}