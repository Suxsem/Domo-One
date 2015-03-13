#include <dht.h>
#include <SimpleTimer.h>
#include <NewPing.h>

#define DISTANZA_RILEVAZIONE 200 //cm
#define DISTANZA_ATTIVAZIONE 8 //cm
#define DISTANZA_MAX 250 //cm
//#define RIPETI_DISTANZA 3
#define DIFFERENZA_LUCE 60 //todo
#define PERIODO_DI_GRAZIA 20 //s
#define SUONA_PER 20 //s

#define APssid "***"
#define APpsw "***"
#define MQTTid "DomoOne"
#define MQTTip "***"
#define MQTTport 1883
#define MQTTuser "***"
#define MQTTpsw "***"
#define MQTTalive 120
#define MQTTretry 20
#define MQTTqos 2
#define esp8266alive 40
#define esp8266serial Serial

#define rumorePin 2
#define buzzPin 4
#define alarmPin 5
#define echoPin 6
#define dhtPin 7
#define statusPin 8
#define rgbrPin 9
#define rgbgPin 10
#define rgbbPin 11
#define triggerPin 12
#define photoPin A0
#define audioPin A1
#define esp8266reset A5

dht DHT;
SimpleTimer timer;
NewPing sonar(triggerPin, echoPin, DISTANZA_MAX);

byte touchCount = 0;
boolean alarmOn = false;
boolean alarmScattato = false;
boolean alarmInserito = false;
volatile boolean rumoreRilevato = false;
boolean movimentoRilevato = false;
boolean connected = false;
byte skip = 0;
unsigned long epoch = 0;
unsigned long lastEpoch = 0;
unsigned int lastPhoto;
char lastLed[7] = "000000";

// #################### esp8266 ####################

void onMessage(String topic, String message) {
  if (topic == String(MQTTid) + "/Led/c") {
    setLed(message);
  } else if (topic == String(MQTTid) + "/Allarme/c") {
    if (message == "1" && !alarmOn) {
      abilitaAllarme(true);
    } else if (message == "0" && alarmOn) {
      disabilitaAllarme();
    } else if (message == "2") {
      if (alarmOn)
        disabilitaAllarme();
      else
        abilitaAllarme(true);      
    }
  } else if (topic == "time") {
      epoch = strtoul(&message[0], NULL, 0);
      lastEpoch = millis() / 1000;
  }
}

void onConnected() {
  digitalWrite(statusPin, HIGH);
  mqttSubscribe(String(MQTTid) + "/Led/c");
  mqttSubscribe(String(MQTTid) + "/Allarme/c");
  mqttSubscribe("time");
  mqttPublish(String(MQTTid) + "/Allarme", String(alarmOn), 1);
  mqttPublish(String(MQTTid) + "/Led", lastLed, 1);
}

void onDisconnected() {  
  digitalWrite(statusPin, LOW);
}

#define buffer_l 50
#define replyTimeout 5000
char in_buffer[buffer_l + 1];
char cb[1];
boolean success;
boolean messageQueued = false;
unsigned long lastAliveCheck = 0;
void checkComm() {
    if (millis() - lastAliveCheck > esp8266alive * 2000UL || lastAliveCheck == 0) {
        pinMode(esp8266reset, OUTPUT);
        delay(50);
        pinMode(esp8266reset, INPUT);
        lastAliveCheck = millis();
        connected = false;        
    }
    if (esp8266serial.find("[(")) {
        esp8266serial.readBytes(cb, 1);
        if (cb[0] == 'r') {
            //ready
            if (connected) {
                connected = false;
                onDisconnected();
            }
            lastAliveCheck = millis();            
            esp8266serial.println("startAlive(" + String(esp8266alive) + ")");
            esp8266serial.flush();
            esp8266serial.println("connectAP(\"" + String(APssid) + "\", \"" + String(APpsw) + "\")");
            esp8266serial.flush();
        } else if (cb[0] == 'a') {
            lastAliveCheck = millis();
            checkComm();
        } else if (cb[0] == 'w') {
            //wifi connected
            esp8266serial.println("mqttInit(\"" + String(MQTTid) + "\", \"" + String(MQTTip) + "\", " + MQTTport + ", \"" + String(MQTTuser)
                            + "\", \"" + String(MQTTpsw) + "\", " + MQTTalive + ", " + MQTTretry + ")");
            esp8266serial.flush();
        } else if (cb[0] == 'c') {
            //mqtt connected
            connected = true;
            onConnected();
        } else if (cb[0] == 'd') {
            //disconnected
            connected = false;
            onDisconnected();
        } else if (cb[0] == 'm') {
            //new message
            if (messageQueued)
                return;
            if (!success)
                messageQueued = true;
            memset(in_buffer, 0, sizeof(in_buffer));
            esp8266serial.readBytesUntil('|', in_buffer, buffer_l);
            String topic = String(in_buffer);
            memset(in_buffer, 0, sizeof(in_buffer));
            esp8266serial.readBytesUntil('|', in_buffer, buffer_l);
            String message = String(in_buffer);
            waitForSuccess();
            onMessage(topic, message);
            messageQueued = false;
        } else if (cb[0] == 'p' || cb[0] == 's') {
            success = true;
        }
    }
}
void waitForSuccess() {
    unsigned long started = millis();
    while (!success) {
        if (!connected || millis() - started > replyTimeout) {
            success = true;
            break;
        }
        checkComm();
    }
}
void mqttPublish(String topic, String message, byte retain) {
    if (!connected)
        return;
    success = false;
    esp8266serial.println("mqttPublish(\"" + topic + "\", \"" + message + "\",  " + MQTTqos + ", " + retain + ")");                
    esp8266serial.flush();
    waitForSuccess();
}
void mqttSubscribe(String topic) {
    if (!connected)
        return;
    success = false;
    esp8266serial.println("mqttSubscribe(\"" + String(topic) + "\", " + MQTTqos + ")");
    esp8266serial.flush();
    waitForSuccess();
}

// #################### allarme ####################

void abilitaAllarme(boolean now) {
  timer.deleteTimers();
  alarmOn = true;
  if (now)
    attivaAllarme();
  else
    timer.setTimeout(PERIODO_DI_GRAZIA * 1000UL, attivaAllarme);
  digitalWrite(alarmPin, HIGH);
  setLed("000000");
  mqttPublish(String(MQTTid) + "/Allarme", "1", 1);
}

void disabilitaAllarme() {
  timer.deleteTimers();
  alarmOn = false;
  alarmScattato = false;
  alarmInserito = false;
  digitalWrite(alarmPin, LOW);
  mqttPublish(String(MQTTid) + "/Allarme", "0", 1);
}

void attivaAllarme() {
  alarmInserito = true;
}

void silenziaAllarme() {
  alarmScattato = false;
}


// #################### led, beep, time e interrupt ####################

#define R 31.8975139158 //https://diarmuid.ie/blog/post/pwm-exponential-led-fading-on-arduino-or-other-platforms
void setLed(String mRgb) {
  // Convert it to long 
  long number = strtol(&mRgb[0], NULL, 16);  
  // Split them up into r, g, b values
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  analogWrite(rgbrPin, pow (2, (r / R)) - 1);
  analogWrite(rgbgPin, pow (2, (g / R)) - 1);
  analogWrite(rgbbPin, pow (2, (b / R)) - 1);
  delay(100);
  lastPhoto = analogRead(photoPin);  
  mRgb.toCharArray(lastLed, 7);
  mqttPublish(String(MQTTid) + "/Led", mRgb, 1);  
}

void faiBeep(unsigned int millis) {
  detachInterrupt(0);
  digitalWrite(buzzPin, HIGH);
  delay(millis);
  digitalWrite(buzzPin, LOW);
  delay(20);
  attachInterrupt(0, rilevaRumore, FALLING);
}

unsigned long tempo() {
  return epoch + (millis() / 1000 - lastEpoch);
}

void rilevaRumore() {
  rumoreRilevato = true;
}

// #################### setup e loop ####################

void setup() {
  esp8266serial.begin(9600);
  esp8266serial.setTimeout(500);
  
  while(!connected)
    checkComm();    

  pinMode(buzzPin, OUTPUT);
  pinMode(statusPin, OUTPUT);
  pinMode(alarmPin, OUTPUT);  
  pinMode(rgbrPin, OUTPUT);
  pinMode(rgbgPin, OUTPUT);
  pinMode(rgbbPin, OUTPUT);
  
  lastPhoto = analogRead(photoPin);
  faiBeep(10); //contiene attachInterrupt per rilevare il rumore
}

void loop() {
  
  timer.run();
  
  do
    checkComm();  //could take up to 0.5s
  while(!connected);
   
  //misuro la distanza
  //  unsigned int dist = sonar.convert_cm(sonar.ping_median(RIPETI_DISTANZA));
  unsigned int dist = sonar.ping_cm();

  if (dist == 0)
    dist = 999;
    
  //incremento touchCount se rilevo un oggetto (mano?) a meno di 20cm, altrimenti resetto    
  if (dist < DISTANZA_ATTIVAZIONE)
    touchCount++;
  else
    touchCount = 0;
    
  //se è l'oggetto è rimasto vicino per ? loop e l'allarme non è inserito lo inserisco, se è inserito lo disinserisco
  if (touchCount >= 2 && !alarmOn) { //todo trovare touchCount
    touchCount = 0;
    faiBeep(50);    
    abilitaAllarme(false);
  } else if (touchCount >= 2 && alarmOn) {
    touchCount = 0;
    faiBeep(50);
    delay(100);
    faiBeep(50);
    disabilitaAllarme();
  }
  
  //lampeggia durante periodo di grazia
  if (alarmOn && !alarmInserito) {
    digitalWrite(alarmPin, LOW);
    delay(200);
    digitalWrite(alarmPin, HIGH);
  }
  
  unsigned int photo = analogRead(photoPin);
  unsigned int audio = analogRead(audioPin);

  int absTemp = photo - lastPhoto; //abs() non può contenere operazioni all'interno
  boolean mRumoreRilevato = rumoreRilevato;    
  rumoreRilevato = false;
  
  if ((dist < DISTANZA_RILEVAZIONE
      || abs(absTemp) > DIFFERENZA_LUCE
      || mRumoreRilevato)
      && !movimentoRilevato) {
    movimentoRilevato = true;
    mqttPublish(String(MQTTid) + "/Movimento", String(tempo()), 1);
    if (alarmInserito && !alarmScattato) {
      if (connected) {
        mqttPublish(String(MQTTid) + "/Scattato", "1", 0);
        if (dist < DISTANZA_RILEVAZIONE) //todo remove
          mqttPublish(String(MQTTid) + "/Motivo", "distanza: " + String(dist), 0);
        if (abs(absTemp) > DIFFERENZA_LUCE)
          mqttPublish(String(MQTTid) + "/Motivo", "luce", 0);
        if (mRumoreRilevato)
          mqttPublish(String(MQTTid) + "/Motivo", "rumore", 0);          
      } //todo fine remove
      alarmScattato = true;
      timer.setTimeout(SUONA_PER * 1000UL, silenziaAllarme);
    }
  } else if (dist >= DISTANZA_RILEVAZIONE
      && abs(absTemp) <= DIFFERENZA_LUCE
      && !mRumoreRilevato
      && movimentoRilevato) {
    movimentoRilevato = false;
  }
  
  lastPhoto = photo;
  
  if (alarmScattato)
    faiBeep(500);
    delay(500); //todo togliere commento
  
  if (skip % 10 == 0 && connected) { //todo trovare skip giusto
    DHT.read22(dhtPin);
    mqttPublish(String(MQTTid) + "/Temperatura", String(DHT.temperature), 1);
    mqttPublish(String(MQTTid) + "/Umidità", String(DHT.humidity ), 1);
    mqttPublish(String(MQTTid) + "/Distanza", String(dist), 1);
    mqttPublish(String(MQTTid) + "/Rumore", String(audio), 1);
    mqttPublish(String(MQTTid) + "/Luce", String(photo), 1);
  }
  
  skip++; 
}