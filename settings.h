#define STATUSUPDATEFRQ 60000 // sending status MQTT in milliseconds
#define PINTIMEOUT 2000       // pin code entry timeout in milliseconds
#define PINLIMIT 2000         // limit how quickly pin codes can be sent
#define RFIDLIMIT 2000        // limit how quickly rfids can be scanned
#define IBUTTONLIMIT 2000     // limit how quickly same ibutton can be scanned
#define BRIGHTNESS  64        // Default LED brightness.
#define ANALOGRATE 250        // Analog input query frequency
#define PULSEDELAY 250        // Length of the pulse output

const char *ssid = "xxx";                     // Wifi SSID
const char *password = "xxx";              // Wifi password
const char* mqtt_server = "192.168.1.xx";               // MQTT server address, leave empty to disable MQTT function
const char* mqtt_user = "xxx";                          // MQTT user id
const char* mqtt_password = "xxxx";                      // MQTT password
const char* clientID = "rfid2";                     // MQTT client ID
const char* topicStatus = "rfid/status";          // MQTT topic where the device sends updates every 10 seconds
const char* topicEvent = "rfid/event";          // pin code entered, or card scanned
const char* topicRelay1 = "rfid/relay1";          // Relay 1 On/Off
const char* topicRelay2 = "rfid/relay2";          // Relay 2 On/Off
const char* topicRelay3 = "rfid/relay3";          // Relay 3 On/Off
const char* topicRelay4 = "rfid/relay4";          // Relay 4 On/Off
const char* topicPulse1 = "rfid/pulse1";          // Short Pulse on Relay 1
const char* topicPulse2 = "rfid/pulse2";          // Short Pulse on Relay 2
const char* topicLight = "rfid/light";          // change the light
const char* topicInput = "rfid/input";          // input sensor value
