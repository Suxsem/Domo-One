WARNING: all connections to the Arduino pins are made with jumper cables soldered at the back of the pcb!

C1, C2, C5 = 100uF
C3 = 0.1uF
C4 = 1uF

L1, L2, L3 = 47uH

R1 = 470omh
R2 = 220omh
R3 = 470omh trimmer (adjust it until you get about 5.1v on left pin of "sel" pin header 
R4 = 100omh
R5 = 1Komh

U1 = LM317T voltage regulator (or equivalent)

alarm = red led (must be connected to pin 5 of Arduino)
status = green led (must be connected to pin 8 of Arduino)

sel = male pin header (put a jumper on left and middle pins to enable the voltage regulator or feed external 5v on middle pin and ground to the right pin to avoid the voltage regulator)

pwr = male pin haeder (feed 8-9v on bottom pin and ground to the upper pin)

buzz = female pin header for buzzer (bottom pin = ground, upper pin must be connected to pin 4 of Arduino)

sound = female pin header for sound sensor (like lm393; left pin = analog output, must be connected to pin A1 of Arduino, middle-left pin = gnd, middle-right pin = vcc, right pin = digital output, must be connected to pin 2 of Arduino)

ultra = female pin header for ultrasonic sensor (like hc-sr04; left pin = vcc, middle-left pin = trigger, must be connected to pin 12 of Arduino, middle-right pin = echo, must be connected to pin 6 of Arduino, right pin = gnd)

wifi = female pin header for esp8266 wifi module 3.3to5v adapter (upper pin = vcc, middle-upper pin = gnd, middle pin = tx, must be connected to pin 0 of Arduino, middle-bottom pin = rx, must be connected to pin 1 of Arduino, bottom pin = reset, must be connected to pin A5 of Arduino

top1 = female pin header for the domoone-2 pcb (upper pin must be connected to pin A0 of Arduino, middle-upper pin must be connected to pin 9 of Arduino, middle-bottom pin must be connected to pin 10 of Arduino, bottom pin must be connected to pin 11 of Arduino)

top2 = female pin header for the domoone-2 pcb (middle pin must be connected to pin 7 of Arduino)