# 01-Publish
This is a doc to make clear what you can achieve by using this lib.

In this lib you will learn to publish data into a broker and get data from an broker.

This doc will focus on how to send data to an broker.

## Table of content
1. First obj
2. begin()
3. loop()
4. publish()

5. full example

## First Object
First you need to instantiate an obj from class "AViShaMQTT".

AViShaMQTT's Constructor requires:
- ssid: Your WiFi Name
- ssid_password: Your WiFi password
- mqtt_server: The IP or the url of the MQTT Broker
- mqtt_port: the port of the server. (Mostly used: 1883)

- mqtt_user: The broker gave you an username
- mqtt_pass: The broker gave you an password

As example:
```cpp
AViShaMQTT myMQTT(wifi_name, wifi_password, 192.168.2.55, 1883, my_user, my_user_password);
```

## Begin()
Then you want to use the method "begin" of the object.

```cpp
myMQTT.begin();
```

> Which will connect to WiFi and connect to MQTT!

## Loop()

> [!Important]
> Call loop() at least every ~15 s to maintain connection.

```cpp
myMQTT.loop();
```

## Publish()
To send data you must have:
- In which `Topic`
- Data

### The Topic
A topic is a digital place where you want to send your data to. 

For example you come up with the name:
"myHome/inDoor/livingroom/0/temp"

and every new sensor will be a new place.

For example a sensor, which gets pressure data.

Therefore you might want:
"myHome/inDoor/livingroom/0/pres"

> [!NOTE]
> "0" is here just a number, which increments from 0 in this example.

### Data

> [!Important]
> Your data must be transformed into a String();

```cpp
String(5); // convert 5 into string
```

> [!Info]
> Do **NOT** separate the String or use in any way spaces or special characters!

### Publish Data into a topic
To send data, use the `publish` method.

It requires:
- Topic
- Data

As following:
```cpp
myMQTT.publish("this/is/my/topic/pressure", String(5));
```

## Full example
```ino

// demo code, code could be broken
// Copyright (C) 2026 seesee010
// this snippset is under CC0 License

#include <AViShaMQTT.h>

const char* ssid = "isi dengan nama wifi";
const char* password = "isi dengan password wifi";
const char* mqtt_server = "broker.emqx.io";

const int mqtt_port = 1883;

const char* mqtt_user = "user123";
const char* mqtt_pass = "12345";

// instiate object
AViShaMQTT mqtt(ssid, password, mqtt_server, mqtt_port, mqtt_user, my_user_pass);

void setup() {
    mqtt.begin();
}

void loop() {
    mqtt.loop();

    mqtt.publish("helloWorld/topic/0123/temp", String(5));
}
```

> [!Important]
> Copyright (C) 2026 seesee010. This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**, same as the AViShaMQTT code.