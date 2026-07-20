// Uncomment to disable brownout detector (see src/AViShaMQTT.h for details)
// #define AVISHAMQTT_DISABLE_BROWNOUT

#include <AViShaMQTT.h>

#if defined(ESP32)
#include <WiFiClientSecure.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#endif

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 8883;

WiFiClientSecure net;
AViShaMQTT mqtt(ssid, password, mqtt_server, mqtt_port, net);

const char* topicSensor = "test/topic/sensor";
unsigned long timer = 0;
const unsigned long interval = 5000;
int counter = 0;

void setup() {
  Serial.begin(115200);

#if defined(ESP8266)
  net.setInsecure();
#elif defined(ESP32)
  net.setInsecure();
#endif

  if (!mqtt.begin()) {
    Serial.print("[APP]: Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  Serial.println("[APP]: Connected with TLS");
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= interval) {
    timer = millis();
    mqtt.publish(topicSensor, String(++counter));
  }
}
