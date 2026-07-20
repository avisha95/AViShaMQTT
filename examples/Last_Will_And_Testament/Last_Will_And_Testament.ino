#include <AViShaMQTT.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";
AViShaMQTT mqtt(ssid, password, mqtt_server);

const char* topicSensor = "home/sensor/temp";
const char* topicStatus = "home/status/fridge";
const char* topicWill   = "home/status/fridge";

const unsigned long interval = 5000;
unsigned long timer = 0;
int counter = 0;

void setup() {
  Serial.begin(115200);

  mqtt.setWill(topicWill, "offline", true, 1);

  if (!mqtt.begin()) {
    Serial.print("[APP]: Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  mqtt.publish(topicStatus, "online", true, 1);
  mqtt.subscribe(topicSensor);
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= interval) {
    timer = millis();
    mqtt.publish(topicSensor, String(++counter));
  }

  String msg = mqtt.getIncomingMessage();
  if (msg != "") {
    Serial.print("[APP]: Received -> ");
    Serial.println(msg);
    mqtt.setIncomingMessage("");
  }
}
