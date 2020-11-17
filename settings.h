#define STATUSUPDATEFRQ 60000 // sending status MQTT in milliseconds
#define PINTIMEOUT 2000       // pin code entry timeout in milliseconds
#define PINLIMIT 2000         // limit how quickly pin codes can be sent
#define RFIDLIMIT 2000        // limit how quickly rfids can be scanned
#define BRIGHTNESS  64 // Default LED brightness.

const char *ssid = "xxx";                     // Wifi SSID
const char *password = "yyyy";              // Wifi password
const char* mqtt_server = "192.168.1.xx";               // MQTT server address, leave empty to disable MQTT function
const char* mqtt_user = "xxx";                          // MQTT user id
const char* mqtt_password = "xxx";                      // MQTT password
const char* clientID = "rfid_mqtt";                     // MQTT client ID
const char* topicStatus = "rfid/status";          // MQTT topic where the device sends updates every 10 seconds
const char* topicEvent = "rfid/event";          // pin code entered, or card scanned
const char* topicRelay = "rfid/relay";          // energize the relay
const char* topicLight = "rfid/light";          // change the light
