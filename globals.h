//NodeMCU pins
#define WG_PIN_GREEN 12  // D6
#define WG_PIN_WHITE 13  // D7
#define LED_PIN 14        // Neopixel led D5
#define STATUSLED_PIN 4  // D1
#define RELAY1 16
#define RELAY2 5
#define RELAY3 0
#define RELAY4 2
#define ANALOGINPUT A0
#define IBUTTONPIN 4

#define NUM_LEDS 1
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB


unsigned long lastStatus, lastBME, seconds, lastKey, lastPin, lastRfid, lastiButtonTime, lastAnalog, lastPulse1, lastPulse2;
String mqttStat = "";
unsigned long rfidcount = 0;
unsigned long pincount = 0;
unsigned long ibuttoncount = 0;
String pin = "";
String LEDcolor1, LEDcolor2, lastiButton;
unsigned long LEDblink1, LEDblink2, LEDduration, lastPhaseChange, cycleStart;
int LEDphase = 0;
byte buffer[8];
bool lastAnalogState, pulse1State, pulse2State;
