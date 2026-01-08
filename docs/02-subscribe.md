# 02-Subscribe
This is the doc for how to subscribe an MQTT Broker.

> [!Info]
> Before you want to read this, read at least a little bit of `01-publish.md`, because there are all the basics this doc will set as basics.

## Table of Content
1. Instance an MQTT Obj
2. subscribe into an topic
3. get the latest message.
4. Full example

## 1. Instance an MQTT Object
You need to instantiate an MQTT object from class AViShaMQTT.

Use the same code as in chapter 1 from `01-publish.md`.

Here again if you need it:

```cpp
AViShaMQTT mqtt(ssid, password, mqtt_server, mqtt_port, mqtt_user_name, user_password);
```

Where:
- ssid: Your WiFi Name
- password: Your WiFi password
- mqtt_server: The IP or the url of the MQTT Broker
- mqtt_port: the port of the server. (Mostly used: 1883)

- mqtt_user_name: The broker gave you an username
- user_pass: The broker gave you an password

> [!NOTE]
> For more information read `01-publish` Constructor.

## 2. Subscribe a topic
Use the method `subscribe`, which will subscribe to a topic you selected.

```cpp
mqtt.subscribe("your/topic/here", 1);
```

> [!Notice]
> The 1 is here because it is the priority of the topics.
>
> That means that you can subscribe more than one!

## 3. Get The latest Message
To get the freshest message use `getIncomingMessage`.

```cpp
String message = mqtt.getIncomingMessage();
```

## 4. Full example
```ino

// demo code, code could be broken
// Copyright (C) 2026 seesee010
// this snippet is under CC0 License

#include <AViShaMQTT.h>

const char* ssid = "isi dengan nama wifi";
const char* password = "isi dengan password wifi";
const char* mqtt_server = "broker.emqx.io";

const int mqtt_port = 1883;

const char* mqtt_user = "user123";
const char* mqtt_pass = "12345";

// instiate object
AViShaMQTT mqtt(ssid, password, mqtt_server, mqtt_port, mqtt_user, mqtt_pass);

String message;

void setup() {
    mqtt.begin();

    mqtt.subscribe("my/topic/here", 1);
}

void loop() {
    mqtt.loop();

    message = mqtt.getIncomingMessage();

    // ...do sth with the message...
}
```

> [!Important]
> Copyright (C) 2026 seesee010. This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**, same as the AViShaMQTT code.