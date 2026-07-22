---
title: 01 — Publishing Data
description: Learn how to connect to WiFi/MQTT and publish data to a broker
nav_order: 1
---

# 01 — Publishing Data

This tutorial covers how to publish sensor data to an MQTT broker using AViShaMQTT.

## Table of Contents

1. [Create the Object](#create-the-object)
2. [begin() — Connect WiFi + MQTT](#begin--connect-wifi--mqtt)
3. [loop() — Keep Alive](#loop--keep-alive)
4. [publish() — Send Data](#publish--send-data)
5. [Full Example](#full-example)

---

## Create the Object

Create an instance of `AViShaMQTT` with your credentials:

```cpp
AViShaMQTT myMQTT(wifi_name, wifi_password, "broker.emqx.io", 1883);
```

The constructor requires:
- **ssid** — Your WiFi network name
- **password** — Your WiFi password
- **mqtt_server** — Broker IP or hostname
- **mqtt_port** — Broker port (default: 1883)
- **mqtt_user** *(optional)* — Username for authentication
- **mqtt_pass** *(optional)* — Password for authentication

> **Note:** For brokers requiring authentication, see the `MQTT_With_Authentication` example.

---

## begin() — Connect WiFi + MQTT

Call `begin()` once in `setup()`. It handles both WiFi and MQTT connection with a 15-second timeout:

```cpp
void setup() {
  if (!myMQTT.begin()) {
    Serial.print("Failed! State: ");
    Serial.println(myMQTT.state());
    while (1); // halt on failure
  }
}
```

`begin()` returns `false` on failure. Check `state()` for the error code.

---

## loop() — Keep Alive

Call `loop()` in the main loop to maintain the connection. It auto-reconnects WiFi and MQTT if disconnected:

```cpp
void loop() {
  myMQTT.loop();
  // your code here
}
```

> **Important:** Call `loop()` at least every ~15 seconds to maintain the connection.

---

## publish() — Send Data

Use `publish()` to send data to a topic:

```cpp
myMQTT.publish("myHome/livingroom/temp", String(25.5));
```

**Choosing a topic:**

Topics are hierarchical paths. For example:

```
myHome/livingroom/0/temp   ← temperature sensor 0
myHome/livingroom/0/pres   ← pressure sensor 0
myHome/livingroom/1/temp   ← temperature sensor 1
```

**Publish overloads:**

```cpp
mqtt.publish("topic", "payload");                          // QoS 0
mqtt.publish("topic", "payload", true, 1);                 // Retain + QoS 1
mqtt.publish("topic", buffer, length, false, 2);           // Binary + QoS 2
```

**Streaming publish (large payloads):**

```cpp
mqtt.beginPublish("topic/json", totalSize, false, 1);
mqtt.write(chunk1, size1);
mqtt.write(chunk2, size2);
mqtt.endPublish();
```

---

## Full Example

```cpp
#include <AViShaMQTT.h>

const char* ssid = "your wifi";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";

AViShaMQTT mqtt(ssid, password, mqtt_server);

unsigned long timer = 0;
int counter = 0;

void setup() {
  Serial.begin(115200);

  if (!mqtt.begin()) {
    Serial.print("Failed, state: ");
    Serial.println(mqtt.state());
    while (1);
  }
}

void loop() {
  mqtt.loop();

  if (millis() - timer >= 1000) {
    timer = millis();
    mqtt.publish("hello/topic/sensor", String(++counter));
  }
}
```

---

[← Back to Home](.) | [Next: 02 — Subscribing →](02-subscribe)
