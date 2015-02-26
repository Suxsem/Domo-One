# **DomoOne**

###An Arduino based device for home surveillance and environment sensing

# **USAGE**

- Setup an MQTT broker (https://github.com/Suxsem/Domo-Openwrt)
- Edit username and password in the Arduino sketch
- Upload the Arduino sketch on your Arduino
- Make the circuit (you can find some hint in circuit folder)
- Connect to a good power supply (7.5v - 9v 1A)
- Deply the Domo app (https://github.com/Suxsem/Domo-Android) on your Android smartphone
- Enjoy Domo

###Info
This project uses the following libraries:
- MQTTDUINO for MQTT communication (INCLUDED IN THE SKETCH!). More info at https://github.com/Suxsem/Mqttduino
- DHT for DHTxx sensor (standard version in library folder). More info at http://playground.arduino.cc/Main/DHTLib
- SimpleTimer for timed events (CUSTOM VERSION in library folder). More info at https://github.com/jfturcot/SimpleTimer