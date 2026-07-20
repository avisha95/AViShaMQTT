// Uncomment to disable brownout detector (see src/AViShaMQTT.h for details)
// #define AVISHAMQTT_DISABLE_BROWNOUT

#include <AViShaMQTT.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";
const char* mqtt_user = "username";
const char* mqtt_pass = "password";

AViShaMQTT mqtt(ssid, password, mqtt_server, 1883, mqtt_user, mqtt_pass);

const char* topicSensor = "test/topic/sensor";
unsigned long timer = 0;
const unsigned long interval = 5000;
int counter = 0;

void setup() {
  Serial.begin(115200);

  if (!mqtt.begin()) {
    Serial.print("[APP]: Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  Serial.println("[APP]: Connected with authentication");
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= interval) {
    timer = millis();
    mqtt.publish(topicSensor, String(++counter));

    if (!mqtt.isConnected()) {
      Serial.print("[APP]: Connection lost, state: ");
      Serial.println(mqtt.state());
    }
  }
}
