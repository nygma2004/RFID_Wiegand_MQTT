// Wiegand RFID access control reader
// Repo: https://github.com/nygma2004/RFID_Wiegand_MQTT
// author: Csongor Varga, csongor.varga@gmail.com
//
// Libraries:
// - Wiegand library: https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino
// - FastLED by Daniel Garcia
// Hardware:
// RFID reader: https://www.aliexpress.com/item/4001034161299.html
//
// NodeMCU pinout:
// D6: green wire of the reader
// D7: white wire of the reader
// GND: black wire of the reader
// 5V: 5V of the neopixel
// GND: GND of the neopixel
// D5: DataIn of the neopixel

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>           // MQTT support
#include <Wiegand.h>
#include "icons.h"
#include "globals.h"
#include "settings.h"
#include <FastLED.h>
#include <ArduinoJson.h>

WIEGAND wg;
MDNSResponder mdns;
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient mqtt(mqtt_server, 1883, 0, espClient);
CRGB leds[NUM_LEDS];


void setup() {
	Serial.begin(115200);  
  Serial.println();
  Serial.println("Wiegand RFID reader");
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( BRIGHTNESS );
    leds[0] = CRGB::Gold;
    FastLED.show();

    Serial.print(F("Connecting to Wifi"));
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    seconds = 0;
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
      seconds++;
      if (seconds % 2 == 0) {
        leds[0] = CRGB::Red;
      } else {
        leds[0] = CRGB::Black;
      }
      FastLED.show();      
      if (seconds>180) {
        // reboot the ESP if cannot connect to wifi
        ESP.restart();
      }
    }
    leds[0] = CRGB::Black;
    FastLED.show();

    Serial.println("");
    Serial.print(F("Connected to "));
    Serial.println(ssid);
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("Signal [RSSI]: "));
    Serial.println(WiFi.RSSI());

    // Set up the MDNS and the HTTP server
    if (mdns.begin("rfidmqtt", WiFi.localIP())) {
      Serial.println(F("MDNS responder started"));
    }  
    server.on("/", [](){                        // Dummy page
      server.send(200, "text/plain", "RFID MQTT");
    });
    server.begin();
    Serial.println(F("HTTP server started"));


     // Set up the MQTT server connection
    if (mqtt_server!="") {
      mqtt.setServer(mqtt_server, 1883);
      mqtt.setCallback(MQTTcallback);
      reconnect();
    }
      
	// default Wiegand Pin 2 and Pin 3 see image on README.md
	// for non UNO board, use wg.begin(pinD0, pinD1) where pinD0 and pinD1 
	// are the pins connected to D0 and D1 of wiegand reader respectively.
	wg.begin(WG_PIN_GREEN,WG_PIN_WHITE); // D6 D7 pins used on NodeMCU

}

void loop() {
  handleWiegand();

  // Handle MQTT connection/reconnection
  if (mqtt_server!="") {
    if (!mqtt.connected()) {
      reconnect();
    }
    mqtt.loop();
  }
  // Send status update message
  handleMQTTStatus();

  handleStatusLED();
  
}

void handleStatusLED() {
  if (LEDphase!=0) {
    // Phase 2, first color is on
    if (LEDphase == 2) {
      if (millis() - lastPhaseChange > LEDblink1) {
        if (millis() - cycleStart < LEDduration) {
            // move to the next phase
            lastPhaseChange = millis();
            SetLEDColor(LEDcolor2);
            LEDphase = 3;
        } else {
            // duration expired, turn off the led
            SetLEDColor("black");
            LEDphase = 0;
        }
      }
    }
    // Phase 3, second color is on
    if (LEDphase == 3) {
      if (millis() - lastPhaseChange > LEDblink2) {
        if (millis() - cycleStart < LEDduration) {
            // move to the next phase
            lastPhaseChange = millis();
            SetLEDColor(LEDcolor1);
            LEDphase = 2;
        } else {
            // duration expired, turn off the led
            SetLEDColor("black");
            LEDphase = 0;
        }
      }
    }

    // Phase 1, starting a new pattern
    if (LEDphase == 1) {
      lastPhaseChange = millis();
      cycleStart = millis();
      SetLEDColor(LEDcolor1);
      LEDphase = 2;
    }
  }
}

void SetLEDColor(String color) {
  
  if (color=="black") {
    leds[0] = CRGB::Black;
  }
  if (color=="yellow") {
    leds[0] = CRGB::Yellow;
  }
  if (color=="red") {
    leds[0] = CRGB::Red;
  }
  if (color=="green") {
    leds[0] = CRGB::Green;
  }
  if (color=="blue") {
    leds[0] = CRGB::Blue;
  }
  FastLED.show();
}

void handleWiegand() {
  if (millis()-lastKey > PINTIMEOUT) {
    if (pin!="") {
      pin="";
      lastKey=millis();
      Serial.println("Pin timeout");
    }
  }
  if(wg.available()) {
    unsigned long wcode = wg.getCode();
    int wtype = wg.getWiegandType();
    Serial.print("Wiegand HEX = ");
    Serial.print(wcode,HEX);
    Serial.print(", DECIMAL = ");
    Serial.print(wcode);
    Serial.print(", Type W");
    Serial.print(wtype); 

    // RFID card was scanned  
    if (wtype==26) {
      String msg;
      msg = "{\"code\":";
      msg += wcode;
      msg += ",\"type\": \"rfid\"}";
      if (millis()-lastRfid > RFIDLIMIT) {
        rfidcount++;
        mqtt.publish(topicEvent, msg.c_str());
        Serial.print("  -> MQTT sent");
        lastRfid = millis();
      } else {
        mqtt.publish(topicEvent, "{ \"type\": \"rfidratelimit\" }");
        Serial.print("  -> RATELIMITED");
      }
    }

    // Keypad was used
    if (wtype==4) {
      if (wcode==27) {
        pin+="*";
        lastKey = millis();
        Serial.print(" | PIN = ");
        Serial.print(pin);
      }
      if (wcode==13) {
        lastKey = millis();
        String msg;
        msg = "{\"code\": \"";
        msg += pin;
        msg += "\" ,\"type\": \"pin\"}";
        Serial.print(" | PIN = ");
        Serial.print(pin);
        if (millis()-lastPin > PINLIMIT) {
          pincount++;
          mqtt.publish(topicEvent, msg.c_str());
          Serial.print("  -> MQTT sent");
          pin="";
          lastPin = millis();
        } else {
          mqtt.publish(topicEvent, "{ \"type\": \"pinratelimit\" }");
          Serial.print("  -> RATELIMITED");
          pin="";
        }
      }
      if ((wcode>=0)&&(wcode<=9)) {
        pin+=wcode;
        lastKey = millis();
        Serial.print(" | PIN = ");
        Serial.print(pin);
      }
    }
    Serial.println(); 
  }
}

void handleMQTTStatus() {
  if (millis() - lastStatus >= STATUSUPDATEFRQ) {  
    lastStatus = millis();
    String mqttStat;
    mqttStat = "{\"rssi\":";
    mqttStat += WiFi.RSSI();
    mqttStat += ",\"uptime\":";
    mqttStat += millis()/1000/60;
    mqttStat += ",\"rfidcount\":";
    mqttStat += rfidcount;
    mqttStat += ",\"pincount\":";
    mqttStat += pincount;
    mqttStat += "}";
    mqtt.publish(topicStatus, mqttStat.c_str());
    Serial.print(F("Status: "));
    Serial.println(mqttStat);
  }    
}

// MQTT reconnect logic
void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(clientID, mqtt_user, mqtt_password)) {
      Serial.println(F("connected"));
      // ... and resubscribe
      mqtt.subscribe(topicRelay);
      Serial.print(F("Subscribed to "));
      Serial.println(topicRelay);
      mqtt.subscribe(topicLight);
      Serial.print(F("Subscribed to "));
      Serial.println(topicLight);
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(mqtt.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// MQTT callback function
void MQTTcallback(char* topic, byte* payload, unsigned int length) {
  // Convert the incoming byte array to a string
  char inData[120];
  Serial.print("array: ");
   for(int i = 0; i<length; i++){
     //Serial.print((char)payload[i]);Serial.print("_");
     inData[i] = (char)payload[i];
   }
   //Serial.println();

  String strTopic = String((char*)topic);
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;

  Serial.print(F("Message arrived on topic: ["));
  Serial.print(strTopic);
  Serial.print(F("], "));
  Serial.println(message);

  if (strTopic==(String)topicRelay) {
    int newvolume = atoi((char *)payload);
    if ((newvolume>=0) && (newvolume<22)) {
      Serial.print(F("New volume: "));
      Serial.println(newvolume);  
    }
  }
  if (strTopic==(String)topicLight) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(inData);  
    LEDcolor1 = root["color1"].as<String>();
    Serial.print("Color1 = ");
    Serial.print(LEDcolor1);
    LEDcolor2 = root["color2"].as<String>();
    Serial.print(", Color2 = ");
    Serial.print(LEDcolor2);
    LEDblink1 = root["blink1"];
    Serial.print(", Blink1 = ");
    Serial.print(LEDblink1);
    LEDblink2 = root["blink2"];
    Serial.print(", Blink2 = ");
    Serial.print(LEDblink2);
    LEDduration = root["duration"];
    Serial.print(", Duration = ");
    Serial.println(LEDduration);
    LEDphase = 1;
  }

}
