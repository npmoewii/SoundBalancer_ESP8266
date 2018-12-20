// #include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MicroGear.h>

#define D0 16
#define LED D0

#define APPID ""
#define KEY ""
#define SECRET ""
#define ALIAS ""
#define TARGETWEB ""

const char *ssid = "";
const char *password = "";

WiFiClient client;
MicroGear microgear(client);

int timer = 0;
char dataToNetpie[50], vol[50], mode = '0';
String rawSTM, rawSerial;

void connectWifi() {
  WiFi.begin(ssid, password);
  // Serial.print("Connecting wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    // Serial.print(".");
  }
  // Serial.println("WiFi connected");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
}

// Event handler for microgear >>>>>>>
void onMsghandler(char *topic, uint8_t *msg, unsigned int msglen) {
  // Serial.print("Incoming message -->");
  msg[msglen] = '\0';
  // Serial.println((char *)msg);
  mode = msg[0];
}

void onConnected(char *attribute, uint8_t *msg, unsigned int msglen) {
  // Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}
// <<<<< End Event handler for microgear

void setup() {
  // Serial.println("Initialize..");

  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(10);

  connectWifi();

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);
  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
}

void loop() {
  if (Serial.available()) {
    rawSTM = Serial.readString();
    if (rawSTM.length()) {
      sprintf(vol, "%s", rawSTM.c_str());
    }
  }

  if (mode != '0') {
    Serial.printf("%c", mode);
    mode = '0';
  }

  if (microgear.connected()) {
    microgear.loop();
    if (timer >= 200) {
      sprintf(dataToNetpie, "%s", vol);
      microgear.chat(TARGETWEB, dataToNetpie);
      timer = 0;
    } else {
      timer += 100;
    }
  } else {
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    } else {
      timer += 100;
    }
  }

  delay(100);
}