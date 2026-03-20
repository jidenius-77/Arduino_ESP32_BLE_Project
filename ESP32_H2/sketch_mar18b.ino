// ESP32H2 DEV MODULE

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// UUID (måste matcha Arduino) https://www.uuidgenerator.net/
static BLEUUID serviceUUID("19B10000-E8F2-537E-4F6C-D104768A1214");
static BLEUUID charUUID("19B10001-E8F2-537E-4F6C-D104768A1214");

// Globala variabler
BLEAdvertisedDevice* myDevice = nullptr;
BLERemoteCharacteristic* pRemoteCharacteristic = nullptr;
BLEClient* pClient = nullptr;

bool doConnect = false;
bool connected = false;
bool doScan = true;

// Callback när data tas emot (NOTIFY)
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

  if (length == 4) {
    int value;
    memcpy(&value, pData, sizeof(value));

    Serial.print("Mottaget värde: ");
    Serial.println(value);
  }
}

// Hantera disconnect
class MyClientCallback : public BLEClientCallbacks {
  void onDisconnect(BLEClient* pclient) {
    Serial.println("Förlorade anslutning");
    connected = false;
    doScan = true;
  }
};

// Scan callback – hitta Arduino
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(serviceUUID)) {

      Serial.println("Hittade Arduino!");

      BLEDevice::getScan()->stop();

      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false;
    }
  }
};

// Anslut till server
bool connectToServer() {

  Serial.println("Försöker ansluta...");

  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  if (!pClient->connect(myDevice)) {
    Serial.println("Misslyckades att ansluta");
    return false;
  }

  Serial.println("Ansluten!");

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.println("Service hittades inte");
    pClient->disconnect();
    return false;
  }

  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.println("Characteristic hittades inte");
    pClient->disconnect();
    return false;
  }

  // Aktivera NOTIFY
  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
    Serial.println("Notify aktiverad!");
  } else {
    Serial.println("Characteristic stödjer inte notify");
  }

  connected = true;
  return true;
}

// Setup
void setup() {
  Serial.begin(115200);
  Serial.println("Startar ESP32 BLE Client...");

  BLEDevice::init("");

  BLEScan* pScan = BLEDevice::getScan();
  pScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pScan->setActiveScan(true);
}

// Loop
void loop() {

  // Starta scan om behövs
  if (doScan) {
    Serial.println("Scannar efter Arduino...");
    BLEDevice::getScan()->start(5, false);
    delay(1000);
  }

  // Anslut
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Redo att ta emot data!");
    } else {
      doScan = true;
    }
    doConnect = false;
  }

  // Kontrollera anslutning
  if (connected && !pClient->isConnected()) {
    Serial.println("Koppling tappad");
    connected = false;
    doScan = true;
  }

  delay(1000);
}