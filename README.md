# BLE Bridge: Arduino UNO R4 WiFi to ESP32-H2 Gateway

Detta projekt demonstrerar en trådlös sensorbrygga med **Bluetooth Low Energy (BLE)**. En **Arduino UNO R4 WiFi** agerar som en perifer enhet (sensor) och en **ESP32-H2** agerar som en central enhet (gateway) som tar emot data via *Notifications*.

---

## Systemöversikt

* **Sensor Node (Arduino):** Skapar en BLE-tjänst och "annonserar" (advertises) sin närvaro. Varannan sekund räknar den upp ett värde och skickar en notis till den anslutna klienten.
* **Gateway (ESP32-H2):** Scannar efter enheter med specifika UUID:s. Vid träff ansluter den automatiskt och prenumererar på uppdateringar.

---

## Hårdvara

* **Server (Sensor):** Arduino UNO R4 WiFi
* **Client (Gateway):** ESP32-H2 Dev Module
* **Protokoll:** BLE (Bluetooth Low Energy)

---

## UUID & Verktyg

För att BLE-enheter ska hitta varandra krävs unika identifieringsnummer (UUID). Om du vill utöka projektet med fler tjänster (t.ex. temperatur eller fuktighet) kan du generera nya koder här:

* **Generator:** [uuidgenerator.net](https://www.uuidgenerator.net/)
* **Instruktion:** Använd "Version 4 UUID".

### Nuvarande konfiguration "exempel":
| Typ | UUID |
| :--- | :--- |
| **Service UUID** | `19B10000-E8F2-537E-4F6C-D104768A1214` |
| **Characteristic UUID** | `19B10001-E8F2-537E-4F6C-D104768A1214` |

---

## Installation & Inställningar

### 1. Bibliotek
Installera följande via *Library Manager* i Arduino IDE:
* `ArduinoBLE` (av Arduino)
* `ESP32 BLE Arduino` (ingår i ESP32 board-paketet)

### 2. Specifik inställning för ESP32-H2
För att kunna se utskrifter i Serial Monitor måste USB-drivrutinen aktiveras i koden vid start:
1. Gå till menyn **Tools** i Arduino IDE.
2. Leta upp **USB CDC On Boot**.
3. Sätt denna till **Enabled**.
4. Ladda upp koden igen.

---

## Teknisk Beskrivning: NOTIFY-metoden

Istället för att Gateway-enheten hela tiden frågar efter data (*polling*), använder detta projekt **Notify**. 

1. **Effektivitet:** Arduinon "knackar på" hos ESP32 endast när värdet faktiskt har ändrats.
2. **Prestanda:** Gateway-enheten kan fokusera på andra uppgifter (som framtida MQTT-kommunikation) och reagerar omedelbart via en `callback`-funktion när data landar.

---

## Felsökning

* **Tom Serial Monitor (ESP32):** Dubbelkolla att baudrate är **115200** och att *USB CDC On Boot* är **Enabled**.
* **Hittar inte enhet:** Tryck på **Reset** på Arduinon. BLE-enheter kan sluta annonsera om de tror att en gammal koppling fortfarande är aktiv.
* **Anslutning nekas:** Se till att enheterna är placerade nära varandra (0.5 - 2 meter) vid första anslutning.

---

**Projektstatus:** BLE-länk etablerad och fungerande.
