# AViShaMQTT — MQTT Library for ESP32 and ESP8266

A simple and robust MQTT library for ESP32 and ESP8266 with auto-reconnect, state tracking, QoS 0/1/2, TLS support, and Last Will. Built on [arduino-mqtt](https://github.com/256dpi/arduino-mqtt) by Joël Gähwiler — **bundled, no extra dependencies**.

**Version:** 1.1.0  
**Author:** Ajang Rahmat  
**Maintainer:** Ajang Rahmat <ajangrahmat@gmail.com>  
**Category:** Communication  
**URL:** [GitHub — avisha95/AViShaMQTT](https://github.com/avisha95/AViShaMQTT)  
**Architectures:** ESP8266, ESP32  
**Includes:** AViShaMQTT.h  

### Installation

#### Arduino IDE

1. Download this repo as ZIP (`Code` → `Download ZIP`)
2. In Arduino IDE: `Sketch` → `Include Library` → `Add .ZIP Library...`
3. Select the downloaded ZIP file

Or manually copy the `AViShaMQTT` folder to `Arduino/libraries/`.

#### PlatformIO

**Via `platformio.ini`:**

```ini
lib_deps =
    https://github.com/avisha95/AViShaMQTT
```

Or if registered in the registry:

```ini
lib_deps =
    avisha95/AViShaMQTT@^1.1.0
```

**Via CLI:**

```bash
pio pkg install --library "avisha95/AViShaMQTT@^1.1.0"
```

> **Note:** arduino-mqtt is already bundled inside AViShaMQTT — no extra dependencies needed.

### Credits

AViShaMQTT uses [arduino-mqtt](https://github.com/256dpi/arduino-mqtt) (MIT License) by Joël Gähwiler as the MQTT backend. See `LICENSE-mqtt.md` for license information.

### Description

AViShaMQTT is designed to make it easy for ESP8266 and ESP32 devices to use the MQTT protocol. It supports MQTT features like publish, subscribe, Retain, QoS, and more. Developers can quickly integrate their IoT devices with any MQTT broker.

### Features

- **Publish & Subscribe:** Send and receive messages via MQTT topics.
- **Binary Publish:** Publish binary payloads (`uint8_t*`, length).
- **Retain:** Store the last message on a topic.
- **QoS (Quality of Service):** QoS 0, 1, 2.
- **Unsubscribe:** Stop subscribing to a topic.
- **Disconnect:** Manually disconnect from the broker.
- **Last Will & Testament:** Set a will message for device disconnect notification.
- **Keep Alive:** Configurable keep alive interval.
- **State Info:** Get connection error codes via `state()` (PubSubClient compatible).
- **Auto-Reconnect:** Automatic WiFi and MQTT reconnection in `loop()`.
- **Non-blocking Connect:** `begin()` returns `false` on failure (with timeout).
- **Custom Client Support:** Use any `Client` subclass (WiFiClientSecure, EthernetClient, etc.).
- **MQTT 3.1.1:** Uses [arduino-mqtt](https://github.com/256dpi/arduino-mqtt) as the backend.
- **Easy Integration:** Simple API for IoT devices.

### Connection States

| Code | Constant | Description |
|------|----------|-------------|
| -4 | `MQTT_CONNECTION_TIMEOUT` | Connection timed out |
| -3 | `MQTT_CONNECTION_LOST` | Connection lost |
| -2 | `MQTT_CONNECT_FAILED` | Connection failed |
| -1 | `MQTT_DISCONNECTED` | Disconnected |
| 0 | `MQTT_CONNECTED` | Connected |
| 1 | `MQTT_CONNECT_BAD_PROTOCOL` | Bad protocol version |
| 2 | `MQTT_CONNECT_BAD_CLIENT_ID` | Client ID rejected |
| 3 | `MQTT_CONNECT_UNAVAILABLE` | Server unavailable |
| 4 | `MQTT_CONNECT_BAD_CREDENTIALS` | Bad username/password |
| 5 | `MQTT_CONNECT_UNAUTHORIZED` | Not authorized |

### Quick Example

```cpp
#include <AViShaMQTT.h>

const char* ssid = "your wifi";
const char* password = "your wifi password";
const char* mqtt_server = "broker.emqx.io";

AViShaMQTT mqtt(ssid, password, mqtt_server);

void setup() {
  Serial.begin(115200);

  if (!mqtt.begin()) {
    Serial.print("Connection failed, state: ");
    Serial.println(mqtt.state());
    while (1) delay(1000);
  }

  mqtt.subscribe("test/topic");
}

void loop() {
  mqtt.loop();

  String msg = mqtt.getIncomingMessage();
  if (msg != "") {
    Serial.println("Received: " + msg);
    mqtt.setIncomingMessage("");
  }
}
```

### API Reference

#### Constructor

```cpp
AViShaMQTT(ssid, password, mqtt_server, mqtt_port = 1883, mqtt_user = nullptr, mqtt_pass = nullptr);
AViShaMQTT(ssid, password, mqtt_server, mqtt_port, Client& net, mqtt_user = nullptr, mqtt_pass = nullptr);
```

#### Lifecycle

| Method | Returns | Description |
|--------|---------|-------------|
| `begin()` | `bool` | Connect WiFi + MQTT with 15s timeout. Returns `false` on failure |
| `loop()` | `void` | Call in `loop()`. Handles WiFi/MQTT reconnect + message processing |
| `isConnected()` | `bool` | Check if MQTT is connected |
| `state()` | `int` | Get connection state code (-4 to 5) |
| `disconnect()` | `void` | Disconnect from MQTT broker |

#### Publish

| Method | Returns | Description |
|--------|---------|-------------|
| `publish(topic, payload)` | `bool` | Publish string payload |
| `publish(topic, payload, retained, qos)` | `bool` | Publish with retain flag and QoS |
| `publish(topic, payload, length, retained, qos)` | `bool` | Publish binary payload (`uint8_t*`) |

#### Subscribe

| Method | Returns | Description |
|--------|---------|-------------|
| `subscribe(topic)` | `bool` | Subscribe to topic (QoS 0) |
| `subscribe(topic, qos)` | `bool` | Subscribe with QoS level |
| `unsubscribe(topic)` | `bool` | Unsubscribe from topic |

#### Incoming Messages

| Method | Returns | Description |
|--------|---------|-------------|
| `getIncomingTopic()` | `const char*` | Get topic from last received message |
| `getIncomingMessage()` | `const char*` | Get payload from last received message |
| `setIncomingTopic(topic)` | `void` | Clear/override incoming topic |
| `setIncomingMessage(msg)` | `void` | Clear/override incoming message |

#### Configuration

| Method | Returns | Description |
|--------|---------|-------------|
| `setWill(topic, payload, retained, qos)` | `void` | Set Last Will message (call before `begin()`) |
| `clearWill()` | `void` | Clear Last Will |
| `setKeepAlive(seconds)` | `void` | Set MQTT keep alive interval |

### Examples

| Example | Features | |
|---------|----------|-|
| `Simple_Publish_and_Subscribe` | Basic pub/sub, 1 LED control | [View](examples/Simple_Publish_and_Subscribe) |
| `Advanced_Publish_With_Retain_and_QoS` | Retain flag + QoS 2 publish | [View](examples/Advanced_Publish_With_Retain_and_QoS) |
| `Advanced_Subscribe_With_QoS` | Subscribe with QoS 1 | [View](examples/Advanced_Subscribe_With_QoS) |
| `Advanced_Publish_Subscribe_With_Retain_and_QoS` | Combine retain + QoS for both pub/sub | [View](examples/Advanced_Publish_Subscribe_With_Retain_and_QoS) |
| `Multi_Subscribe_Multi_LED` | Subscribe 3 topics, control 3 LEDs | [View](examples/Multi_Subscribe_Multi_LED) |
| `MQTT_With_Authentication` | Username + password broker login | [View](examples/MQTT_With_Authentication) |
| `MQTT_With_TLS` | Secure MQTT via WiFiClientSecure (port 8883) | [View](examples/MQTT_With_TLS) |
| `Last_Will_And_Testament` | Will message + retained status publish | [View](examples/Last_Will_And_Testament) |

### Troubleshooting

#### ESP32 reboots with `Brownout detector was triggered`

This happens when the voltage drops below ~2.5V-2.7V, usually during WiFi TX power spikes.

**Fix your hardware first (best solution):**

| Cause | Fix |
|-------|-----|
| USB cable too long / poor quality | Use a short 20-30cm USB cable with thick wires |
| USB port can't deliver enough current | Use a 5V 2A power adapter, not a PC USB port |
| Voltage drop on breadboard jumper | Solder directly or use thicker wires |
| No capacitor on VCC | Add 100µF-470µF electrolytic capacitor near VCC/GND |

**Software workaround (last resort):**

Uncomment in `src/AViShaMQTT.h`:

```cpp
#define AVISHAMQTT_DISABLE_BROWNOUT
```

⚠️ This disables ESP32 voltage protection. The chip may behave unpredictably if voltage drops too low. Only use temporarily.

#### ESP keeps restarting (`rst:0xc (SW_CPU_RESET)`)

| Code | Meaning | Fix |
|------|---------|-----|
| `rst:0xc (SW_CPU_RESET)` | Software reset / brownout | Check power (see above) |
| `rst:0x01 (POWERON_RESET)` | Power-on reset | Normal on boot |
| `rst:0x0b (TASK_WDT_RESET)` | Task watchdog timeout | Check `loop()` blocking |
| `rst:0x10 (INT_WDT_RESET)` | Interrupt watchdog | Usually WiFi stack crash |

#### `[MQTT]: Subscribe FAILED` / `[MQTT]: Publish FAILED`

- WiFi or MQTT not connected → Check `mqtt.state()`
- Topic too long (> 128 bytes by default) → Increase MQTT buffer via `MQTTClient` constructor
- Broker rejected the message → Check topic naming rules

#### `[MQTT]: Connection denied (code: 1-5)`

| Code | Meaning | Fix |
|------|---------|-----|
| 1 | Bad protocol | Update arduino-mqtt |
| 2 | Client ID rejected | Change client ID (avoid special chars) |
| 3 | Server unavailable | Check broker address/port |
| 4 | Bad credentials | Check username/password |
| 5 | Not authorized | Check broker ACL settings |

#### WiFi connects but MQTT fails

- Is the broker reachable? Ping from PC first
- Port 1883 (TCP) or 8883 (TLS)? Check firewall
- For TLS: is `net.setInsecure()` called before `mqtt.begin()`?
- Need a specific client ID? Use a custom one

### Constructor Overloads

```cpp
// Default - uses internal WiFiClient
AViShaMQTT(ssid, password, server, port = 1883, user = nullptr, pass = nullptr);

// Custom Client - for TLS (WiFiClientSecure), Ethernet, etc.
AViShaMQTT(ssid, password, server, port, Client& net, user = nullptr, pass = nullptr);
```
