#include <AViShaMQTT.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";
AViShaMQTT mqtt(ssid, password, mqtt_server);

const char* topicSensor = "home/sensor/temp";
const char* topicLEDred = "home/led/red";
const char* topicLEDgreen = "home/led/green";
const char* topicLEDblue = "home/led/blue";

const int pinLEDred = 2;
const int pinLEDgreen = 4;
const int pinLEDblue = 5;

unsigned long timer = 0;
const unsigned long interval = 5000;
int counter = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pinLEDred, OUTPUT);
  pinMode(pinLEDgreen, OUTPUT);
  pinMode(pinLEDblue, OUTPUT);

  digitalWrite(pinLEDred, LOW);
  digitalWrite(pinLEDgreen, LOW);
  digitalWrite(pinLEDblue, LOW);

  if (!mqtt.begin()) {
    Serial.print("[APP]: Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  mqtt.subscribe(topicLEDred);
  mqtt.subscribe(topicLEDgreen, 1);
  mqtt.subscribe(topicLEDblue, 1);
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= interval) {
    timer = millis();
    mqtt.publish(topicSensor, String(++counter));
  }

  String topic = mqtt.getIncomingTopic();
  String msg = mqtt.getIncomingMessage();

  if (msg != "") {
    int value = msg.toInt();

    if (topic == topicLEDred) {
      digitalWrite(pinLEDred, value);
      Serial.print("[APP]: Red LED -> ");
    } else if (topic == topicLEDgreen) {
      digitalWrite(pinLEDgreen, value);
      Serial.print("[APP]: Green LED -> ");
    } else if (topic == topicLEDblue) {
      digitalWrite(pinLEDblue, value);
      Serial.print("[APP]: Blue LED -> ");
    }

    Serial.println(value == 1 ? "ON" : "OFF");
    mqtt.setIncomingMessage("");
    mqtt.setIncomingTopic("");
  }
}
