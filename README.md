# AViShaMQTT

AViShaMQTT is a small wrapper library that simplifies the usage of the
`MQTT.h` library by Joël Gähwiler on ESP8266 and ESP32 devices.

> [!Note]  
> This repository is a **documentation-focused fork** of the original project by
> Ajang Rahmat.  
> The code itself is largely unchanged; the goal of this fork is improved 
documentation and clarity.

Original project:
https://github.com/avisha95/AViShaMQTT

---

## Requirements

This library depends on the following external library:

- **MQTT** by Joël Gähwiler  
  https://github.com/256dpi/arduino-mqtt

You must install it before using AViShaMQTT.


## Supported Platforms

- ESP8266
- ESP32


## Description

AViShaMQTT simplifies common MQTT tasks such as connecting to a broker,
publishing messages, and subscribing to topics on ESP-based devices.
It acts as a thin abstraction layer on top of `MQTT.h`.


## Features

- Publish and subscribe to MQTT topics
- Retained messages
- QoS support (depending on underlying MQTT library)

## License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.


Original Author: Ajang Rahmat  
Documentation & maintenance improvements: seesee010
