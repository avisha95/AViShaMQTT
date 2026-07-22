#include <AViShaMQTT.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";

AViShaMQTT mqtt(ssid, password, mqtt_server, 1883, nullptr, nullptr, 256);

const char* topicLED = "test/persistent/led";
const char* topicSensor = "test/persistent/sensor";

unsigned long timer = 0;
const unsigned long interval = 5000;
int counter = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  mqtt.setCleanSession(false);
  Serial.println("[APP]: Persistent Session enabled");

  if (!mqtt.begin()) {
    Serial.print("[APP]: Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  mqtt.subscribe(topicLED, 1);
  mqtt.subscribe(topicSensor, 1);
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= interval) {
    timer = millis();
    mqtt.publish(topicSensor, String(++counter), false, 1);
  }

  String topic = mqtt.getIncomingTopic();
  String msg = mqtt.getIncomingMessage();

  if (msg != "") {
    Serial.print("[APP]: RX Topic: ");
    Serial.print(topic);
    Serial.print(" | Payload: ");
    Serial.println(msg);

    if (topic == topicLED) {
      digitalWrite(LED_BUILTIN, msg.toInt());
    }

    mqtt.setIncomingMessage("");
    mqtt.setIncomingTopic("");
  }
}
