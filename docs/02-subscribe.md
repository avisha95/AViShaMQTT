---
title: 02 — Subscribing to Topics
description: Learn how to subscribe to MQTT topics and read incoming messages
nav_order: 2
---

# 02 — Subscribing to Topics

This tutorial covers how to subscribe to MQTT topics and process incoming messages.

> **Prerequisite:** Read [01 — Publishing Data](01-publish) first for the basics.

## Table of Contents

1. [Create the Object](#create-the-object)
2. [Subscribe to a Topic](#subscribe-to-a-topic)
3. [Get the Latest Message](#get-the-latest-message)
4. [Full Example](#full-example)

---

## Create the Object

Same as in the publish tutorial:

```cpp
AViShaMQTT mqtt("wifi_name", "wifi_pass", "broker.emqx.io", 1883);
mqtt.begin();
```

---

## Subscribe to a Topic

Use `subscribe()` to listen for messages on a topic:

```cpp
mqtt.subscribe("home/led/red");       // QoS 0
mqtt.subscribe("home/led/green", 1);  // QoS 1
mqtt.subscribe("home/led/blue", 2);   // QoS 2
```

The QoS number determines delivery guarantees:
- **0** — At most once (fire and forget)
- **1** — At least once (acknowledged)
- **2** — Exactly once (four-step handshake)

You can subscribe to multiple topics — up to 8 by default.

---

## Get the Latest Message

In `loop()`, use `getIncomingMessage()` and `getIncomingTopic()` to read messages:

```cpp
void loop() {
  mqtt.loop();

  String topic = mqtt.getIncomingTopic();
  String message = mqtt.getIncomingMessage();

  if (message != "") {
    Serial.printf("Received [%s]: %s\n", topic.c_str(), message.c_str());

    if (topic == "home/led/red") {
      digitalWrite(LED_RED, message.toInt());
    }

    // Clear buffer to avoid re-processing
    mqtt.setIncomingMessage("");
    mqtt.setIncomingTopic("");
  }
}
```

> **Important:** Always clear the incoming message buffer with `setIncomingMessage("")` to avoid processing the same message repeatedly.

---

## Full Example

```cpp
#include <AViShaMQTT.h>

const char* ssid = "your wifi";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";

AViShaMQTT mqtt(ssid, password, mqtt_server);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!mqtt.begin()) {
    Serial.print("Failed, state: ");
    Serial.println(mqtt.state());
    while (1);
  }

  mqtt.subscribe("home/led/builtin", 1);
}

void loop() {
  mqtt.loop();

  String topic = mqtt.getIncomingTopic();
  String msg = mqtt.getIncomingMessage();

  if (msg != "") {
    Serial.printf("RX [%s]: %s\n", topic.c_str(), msg.c_str());

    if (topic == "home/led/builtin") {
      digitalWrite(LED_BUILTIN, msg.toInt());
    }

    mqtt.setIncomingMessage("");
    mqtt.setIncomingTopic("");
  }
}
```

---

[← Previous: 01 — Publishing](01-publish) | [Next: 03 — Advanced →](03-advanced)
