#include <AViShaMQTT.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";

AViShaMQTT mqtt(ssid, password, mqtt_server, 1883, nullptr, nullptr, 512);

const char* topicJSON = "home/sensor/json";
static uint8_t payload[512];

unsigned long timer = 0;
const unsigned long interval = 5000;

void buildLargeJSON(uint8_t* buf, unsigned int& size) {
  const char* sensors[] = {"temp", "humid", "press", "light", "co2"};
  unsigned int len = 0;
  len += snprintf((char*)buf + len, 512 - len, "{");
  len += snprintf((char*)buf + len, 512 - len, "\"device\":\"ESP32\",");
  len += snprintf((char*)buf + len, 512 - len, "\"uptime\":%lu,", millis());
  len += snprintf((char*)buf + len, 512 - len, "\"sensors\":[");
  for (int i = 0; i < 5; i++) {
    len += snprintf((char*)buf + len, 512 - len,
      "%s{\"name\":\"%s\",\"value\":%.1f}",
      i > 0 ? "," : "", sensors[i], random(200, 350) / 10.0);
  }
  len += snprintf((char*)buf + len, 512 - len, "]}");
  size = len;
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  if (!mqtt.begin()) {
    Serial.print("[APP]: Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  Serial.println("[APP]: Ready - streaming publish with 512 byte buffer");
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= interval) {
    timer = millis();

    unsigned int size;
    buildLargeJSON(payload, size);

    if (!mqtt.beginPublish(topicJSON, size, false, 1)) {
      Serial.println("[APP]: Stream start failed");
      return;
    }

    size_t written = mqtt.write(payload, size);
    if (written != size) {
      Serial.printf("[APP]: Stream write mismatch: %d/%d\n", written, size);
      return;
    }

    if (!mqtt.endPublish()) {
      Serial.println("[APP]: Stream end failed");
    }
  }
}
