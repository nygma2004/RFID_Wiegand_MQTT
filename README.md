# RFID Access Control
This ESP8266 sketch communicates with a Weigand RFID reader and keypad. Ket features:
- Connect ot local MQTT server
- Send RFID codes over to MQTT
- Collect keypad presses and until # is pressed and send as a pin code over MQTT
- Block RFID readings and pin code entries too close to each other
- Neopixel LED provides visual feedback which is controller over MQTT
## Hardware
This sketch uses a ESP8266, NodeMCU or Wemos D1 Mini.
- Weigand reader: https://www.aliexpress.com/item/4001034161299.html
- Neopixel (WS2812b) LED
## Wiring
| ESP GPIO | ESP PIN | Weigand Reader  | NeoPixel |
|----------|---------|-----------------|----------|
| 12       | D6      | green (D0)      |          |
| 13       | D7      | white (D1)      |          | 
|          | GND     | black           | GND      |
|          | 5V      |                 | 5V       |
| 14       | D5      |                 | DataIn   |     

Besides this the Weigand reader also need a +12V DC supply connected to the red wire.
## Security Considerations
Keep in mind that the RFID readers that I got for this RFID reader and rewriteable RFID tags. So this does not prevent somebody getting hold of your tags and making a copy. Therefore I would recommend to include additional security measure in your overall solution. I am definitely going to add a camera watching the area where the reader is, so I can get an image when somebody uses the reader. And definitely control devices that would not anyone to get into the house (e.g. open the passanger gate, or car gate, but not disable the alarm or open the garage door). But this is up to you, just keep the security limitations in mind.
