---
title: 03 — Advanced Features
description: Streaming publish, persistent session, Last Will, TLS, authentication, and more
nav_order: 3
---

# 03 — Advanced Features

This page covers all advanced MQTT features available in AViShaMQTT with ready-to-use code snippets.

## Table of Contents

- [Streaming Publish (Large Payloads)](#streaming-publish-large-payloads)
- [QoS (Quality of Service)](#qos-quality-of-service)
- [Retain Messages](#retain-messages)
- [Last Will & Testament](#last-will--testament)
- [Persistent Session](#persistent-session)
- [TLS / Secure Connection](#tls--secure-connection)
- [Authentication](#authentication)
- [Binary Payloads](#binary-payloads)
- [Multi-Topic Subscribe](#multi-topic-subscribe)
- [Custom Buffer Size](#custom-buffer-size)
- [Connection State Monitoring](#connection-state-monitoring)

---

## Streaming Publish (Large Payloads)

Publish payloads larger than the internal buffer using `beginPublish()` / `write()` / `endPublish()`:

```cpp
AViShaMQTT mqtt(ssid, password, server, 1883, nullptr, nullptr, 512);

unsigned int size;
uint8_t json[512];
buildJSON(json, size);  // builds a 300+ byte JSON payload

mqtt.beginPublish("home/data/json", size, false, 1);
mqtt.write(json, size);
mqtt.endPublish();
```

> See the **Streaming_Publish** example for a complete working sketch.

---

## QoS (Quality of Service)

AViShaMQTT supports all three QoS levels for both publish and subscribe:

```cpp
mqtt.publish("sensor/temp", "25.5");                // QoS 0 — fire and forget
mqtt.publish("sensor/press", "1013", false, 1);     // QoS 1 — at least once
mqtt.publish("actuator/valve", "OFF", true, 2);     // QoS 2 — exactly once + retain

mqtt.subscribe("cmd/#", 0);  // QoS 0
mqtt.subscribe("cmd/#", 1);  // QoS 1 — acknowledged
mqtt.subscribe("cmd/#", 2);  // QoS 2 — four-step handshake
```

| QoS | Guarantee | Publishes `+PUBACK` | Use case |
|-----|-----------|:---:|----------|
| 0 | At most once | No | Sensor data (loss tolerable) |
| 1 | At least once | `+PUBACK` | Commands, state changes |
| 2 | Exactly once | `+PUBCOMP` | Critical actuation |

> **Note:** QoS guarantees delivery to the **broker**, not to subscribers. MQTT publishers and subscribers are decoupled.

---

## Retain Messages

The broker stores the last retained message and delivers it to new subscribers immediately:

```cpp
mqtt.publish("home/status/fridge", "online", true, 1);   // retained
mqtt.publish("home/sensor/temp", "25.5");                 // not retained
```

Any new subscriber to `home/status/fridge` immediately receives `"online"` without waiting for the next publish.

---

## Last Will & Testament

The broker publishes a will message if the client disconnects unexpectedly. Call **before** `begin()`:

```cpp
mqtt.setWill("home/status/fridge", "offline", true, 1);  // retained QoS 1
mqtt.begin();

// On connection, publish online status
mqtt.publish("home/status/fridge", "online", true, 1);
```

If the ESP loses power or WiFi, the broker automatically publishes `"offline"` to the topic.

Clear the will:

```cpp
mqtt.clearWill();
```

---

## Persistent Session

Keep subscriptions and queued messages across disconnections. Call **before** `begin()`:

```cpp
mqtt.setCleanSession(false);  // persistent session
mqtt.begin();
mqtt.subscribe("important/cmd", 1);  // broker remembers this
```

When the ESP reconnects after a power loss:
- Broker **automatically restores** subscriptions
- Broker **redelivers** any undelivered QoS 1/2 messages
- No need to call `subscribe()` again

> See the **Persistent_Session** example for a full working sketch.

---

## TLS / Secure Connection

Use `WiFiClientSecure` with the custom-client constructor:

```cpp
#include <AViShaMQTT.h>
#include <WiFiClientSecure.h>

WiFiClientSecure net;
AViShaMQTT mqtt(ssid, password, "broker.hivemq.com", 8883, net);

void setup() {
  net.setInsecure();  // skip certificate validation (dev only)
  mqtt.begin();
}
```

For production, load a CA certificate:

```cpp
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIID...
-----END CERTIFICATE-----
)EOF";

net.setCACert(ca_cert);
```

> See the **MQTT_With_TLS** example for a complete sketch.

---

## Authentication

Pass username and password to the constructor:

```cpp
AViShaMQTT mqtt(ssid, password, "broker.emqx.io", 1883, "myuser", "mypassword");
mqtt.begin();
```

If authentication fails, `state()` returns `MQTT_CONNECT_BAD_CREDENTIALS (4)`.

> See the **MQTT_With_Authentication** example for a complete sketch.

---

## Binary Payloads

Publish raw bytes — useful for sensor readings, protocol buffers, or encoded data:

```cpp
uint8_t sensorData[] = {0x01, 0xA3, 0xFF, 0x7B};

mqtt.publish("sensor/raw", sensorData, 4, false, 1);
```

To read binary messages from a subscriber:

```cpp
const char* raw = mqtt.getIncomingMessage();
// raw is null-terminated, length can be inferred via strlen()
```

---

## Multi-Topic Subscribe

Subscribe up to 8 topics (default). After reconnect, subscriptions auto-restore:

```cpp
mqtt.subscribe("home/led/red");
mqtt.subscribe("home/led/green", 1);
mqtt.subscribe("home/led/blue", 1);
mqtt.subscribe("home/sensor/temp", 2);

// In loop(), differentiate by topic:
String topic = mqtt.getIncomingTopic();
String msg = mqtt.getIncomingMessage();

if (msg != "") {
  if (topic == "home/led/red")     digitalWrite(PIN_RED, msg.toInt());
  if (topic == "home/led/green")   digitalWrite(PIN_GREEN, msg.toInt());
  if (topic == "home/led/blue")    digitalWrite(PIN_BLUE, msg.toInt());
  if (topic == "home/sensor/temp") Serial.println("Temp: " + msg);

  mqtt.setIncomingMessage("");
  mqtt.setIncomingTopic("");
}
```

> See the **Multi_Subscribe_Multi_LED** example for a complete sketch.

---

## Custom Buffer Size

Increase the MQTT packet buffer via the last constructor parameter:

```cpp
AViShaMQTT mqtt(ssid, password, server, 1883, nullptr, nullptr, 512);
```

Default is 128 bytes. Increase it for:
- Long topic names
- Large JSON payloads
- Streaming publish buffers

---

## Connection State Monitoring

Check connection health and handle errors gracefully:

```cpp
void loop() {
  mqtt.loop();

  if (!mqtt.isConnected()) {
    int s = mqtt.state();
    switch (s) {
      case MQTT_CONNECTION_LOST:     Serial.println("Reconnecting..."); break;
      case MQTT_CONNECTION_TIMEOUT:  Serial.println("Timeout!"); break;
      case MQTT_CONNECT_FAILED:      Serial.println("Failed!"); break;
      case MQTT_CONNECT_BAD_CREDENTIALS: Serial.println("Wrong password!"); break;
      default: Serial.printf("State: %d\n", s);
    }
  }
}
```

---

[← Previous: 02 — Subscribing](02-subscribe) | [Back to Home](.)
