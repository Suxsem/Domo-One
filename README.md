# **DomoOne**

###An Arduino based device for home surveillance and environment sensing

# **USAGE**

- Setup an MQTT broker (https://github.com/Suxsem/Domo-Openwrt)
- Edit constants in the Arduino sketch
- Upload the Arduino sketch on your Arduino
- Make the circuit (you can find some hints in the "circuit" folder)
- Connect to a good power supply (7.5v - 9v 1A)
- Deploy the Domo app (https://github.com/Suxsem/Domo-Android) on your Android smartphone
- Enjoy Domo

# COMPONENTS
- Arduino uno
- esp8266 wifi board
- homemade esp8266 3.3to5v adapter (https://github.com/Suxsem/Mqttduino > electrical_connections)
- hc-sr04 ultrasonic sensor
- dht temperature and humidity sensor
- lm393 based sound sensor with analog and digital output
- buzzer
- photoresistor
- leds (green, red, 3x rgb)
- lm317t voltage regulator
- spare electrical components (resistors, trimmer, capacitors, inductors and so on)

# WARNING!

- The esp8266 wifi board makes a lot of RF interferences. Be sure to shield it with a conductive plate connected to ground placed under the esp8266 board

###Info
- A cad model of the box that contains the circuit ready for 3d printing can be found in the "box" folder
- This project uses the following libraries:
  - MQTTDUINO for MQTT communication (INCLUDED IN THE SKETCH!). More info at https://github.com/Suxsem/Mqttduino
  - DHT for DHTxx sensor (standard version in "library" folder). More info at http://playground.arduino.cc/Main/DHTLib
  - SimpleTimer for timed events (CUSTOM VERSION in "library" folder). More info at https://github.com/jfturcot/SimpleTimer
  - NewPing for hc-sr04 sensor (standard version in "library" folder). More info at https://code.google.com/p/arduino-new-ping/