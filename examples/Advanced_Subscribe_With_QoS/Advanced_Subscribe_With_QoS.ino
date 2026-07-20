#include <AViShaMQTT.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";
AViShaMQTT mqtt(ssid, password, mqtt_server);

const char* topicLED = "test/topic/led";

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!mqtt.begin()) {
    Serial.print("[APP]: Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  mqtt.subscribe(topicLED, 1); // QoS 1
}

void loop() {
  mqtt.loop();

  String msg = mqtt.getIncomingMessage();
  if (msg != "") {
    digitalWrite(LED_BUILTIN, msg.toInt());
    mqtt.setIncomingMessage("");
  }
}
