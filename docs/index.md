---
title: AViShaMQTT — Arduino MQTT Library for ESP32 & ESP8266
description: Combines arduino-mqtt + PubSubClient — auto-reconnect, QoS 0/1/2, streaming publish, TLS
---

## Overview

AViShaMQTT combines **[arduino-mqtt](https://github.com/256dpi/arduino-mqtt)** (QoS 2, Retain, Will, Persistent Session) with **[PubSubClient](https://github.com/knolleary/pubsubclient)** (streaming publish, custom buffer) into one Arduino MQTT library for ESP32 and ESP8266 — **zero extra dependencies**.

### Quick Start

```cpp
#include <AViShaMQTT.h>

AViShaMQTT mqtt("wifi_ssid", "wifi_pass", "broker.emqx.io");

void setup() {
  mqtt.begin();
  mqtt.subscribe("test/topic");
}

void loop() {
  mqtt.loop();
  String msg = mqtt.getIncomingMessage();
  if (msg != "") {
    Serial.println(msg);
    mqtt.setIncomingMessage("");
  }
}
```

### Why AViShaMQTT?

| Feature | PubSubClient | arduino-mqtt | AViShaMQTT |
|---------|:---:|:---:|:---:|
| QoS 0 / 1 / 2 | 0 / 1 / — | 0 / 1 / 2 | 0 / 1 / 2 |
| Retain messages | — | ✓ | ✓ |
| Last Will | — | ✓ | ✓ |
| Persistent session | — | ✓ | ✓ |
| Streaming publish | ✓ | — | ✓ |
| Custom buffer size | ✓ | — | ✓ |
| Auto-reconnect WiFi+MQTT | — | — | ✓ |
| Subscription restore | — | — | ✓ |
| TLS | Manual | Manual | Built-in |
| Platforms | All | All | ESP8266, ESP32 |

---

## Tutorials

- **[01 — Publishing Data](01-publish)** — Learn how to connect to WiFi + MQTT and publish sensor data
- **[02 — Subscribing to Topics](02-subscribe)** — Learn how to subscribe and read incoming messages
- **[03 — Advanced Features](03-advanced)** — Streaming publish, QoS, Retain, Will, Persistent Session, TLS, Auth

---

## API Reference

### Constructor

```cpp
AViShaMQTT(ssid, password, server, port = 1883, user = nullptr, pass = nullptr, bufferSize = 128);
AViShaMQTT(ssid, password, server, port, Client& net, user = nullptr, pass = nullptr, bufferSize = 128);
```

### Lifecycle

| Method | Returns | Description |
|--------|---------|-------------|
| `begin()` | `bool` | Connect WiFi + MQTT (15s timeout) |
| `loop()` | `void` | Auto-reconnect + message processing |
| `isConnected()` | `bool` | Check connection status |
| `state()` | `int` | Connection state (-4 to 5) |
| `disconnect()` | `void` | Disconnect from broker |

### Publish

| Method | Returns | Description |
|--------|---------|-------------|
| `publish(topic, payload)` | `bool` | Publish string |
| `publish(topic, payload, retained, qos)` | `bool` | Publish with retain + QoS |
| `publish(topic, payload, length, retained, qos)` | `bool` | Publish binary (`uint8_t*`) |
| `beginPublish(topic, size, retained, qos)` | `bool` | Start streaming publish |
| `write(buf, size)` | `size_t` | Write chunk |
| `endPublish()` | `bool` | Finish and send |

### Subscribe

| Method | Returns | Description |
|--------|---------|-------------|
| `subscribe(topic)` | `bool` | Subscribe (QoS 0) |
| `subscribe(topic, qos)` | `bool` | Subscribe with QoS |
| `unsubscribe(topic)` | `bool` | Unsubscribe |

### Incoming Messages

| Method | Returns | Description |
|--------|---------|-------------|
| `getIncomingTopic()` | `const char*` | Last received topic |
| `getIncomingMessage()` | `const char*` | Last received payload |
| `setIncomingTopic(topic)` | `void` | Clear/override topic |
| `setIncomingMessage(msg)` | `void` | Clear/override message |

### Configuration

| Method | Returns | Description |
|--------|---------|-------------|
| `setWill(topic, payload, retained, qos)` | `void` | Last Will message |
| `clearWill()` | `void` | Clear Will |
| `setKeepAlive(seconds)` | `void` | Keep alive interval |
| `setCleanSession(clean)` | `void` | Persistent session on/off |

---

## Connection States

| Code | Constant | Description |
|------|----------|-------------|
| -4 | `MQTT_CONNECTION_TIMEOUT` | Connection timed out |
| -3 | `MQTT_CONNECTION_LOST` | Connection lost |
| -2 | `MQTT_CONNECT_FAILED` | Connection failed |
| -1 | `MQTT_DISCONNECTED` | Disconnected |
| 0 | `MQTT_CONNECTED` | Connected |
| 1 | `MQTT_CONNECT_BAD_PROTOCOL` | Bad protocol |
| 2 | `MQTT_CONNECT_BAD_CLIENT_ID` | Client ID rejected |
| 3 | `MQTT_CONNECT_UNAVAILABLE` | Server unavailable |
| 4 | `MQTT_CONNECT_BAD_CREDENTIALS` | Bad credentials |
| 5 | `MQTT_CONNECT_UNAUTHORIZED` | Not authorized |

---

## Examples

| Example | Features |
|---------|----------|
| `Simple_Publish_and_Subscribe` | Basic pub/sub, 1 LED |
| `Advanced_Publish_With_Retain_and_QoS` | Retain + QoS 2 |
| `Advanced_Subscribe_With_QoS` | Subscribe QoS 1 |
| `Advanced_Publish_Subscribe_With_Retain_and_QoS` | Retain + QoS pub/sub |
| `Multi_Subscribe_Multi_LED` | 3 topics, 3 LEDs |
| `MQTT_With_Authentication` | Username + password |
| `MQTT_With_TLS` | WiFiClientSecure port 8883 |
| `Last_Will_And_Testament` | Will + retained status |
| `Persistent_Session` | `setCleanSession(false)` |
| `Streaming_Publish` | Large JSON via beginPublish/write/endPublish |

---

## Installation

### Arduino IDE

1. Download ZIP → `Sketch` → `Include Library` → `Add .ZIP Library...`

### PlatformIO

```ini
lib_deps = avisha95/AViShaMQTT@^1.2.0
```

---

**[View on GitHub](https://github.com/avisha95/AViShaMQTT)** | Version 1.2.0 — Ajang Rahmat
