#ifndef AViShaMQTT_h
#define AViShaMQTT_h

#include <Arduino.h>
#include <WiFiClient.h>
#include "mqtt/MQTT.h"

// Uncomment to disable ESP32 brownout detector (use if experiencing brownout during WiFi)
// #define AVISHAMQTT_DISABLE_BROWNOUT

#if defined(ESP32) && defined(AVISHAMQTT_DISABLE_BROWNOUT)
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5

#define AVISHAMQTT_MAX_SUBS 8

class AViShaMQTT {
  public:
    AViShaMQTT(const char* ssid, const char* password, const char* mqtt_server,
            int mqtt_port = 1883, const char* mqtt_user = nullptr, const char* mqtt_pass = nullptr,
            int bufferSize = 128);
    AViShaMQTT(const char* ssid, const char* password, const char* mqtt_server,
            int mqtt_port, Client& net, const char* mqtt_user = nullptr, const char* mqtt_pass = nullptr,
            int bufferSize = 128);

    bool begin();
    void loop();
    bool isConnected();
    int state();

    bool publish(String topic, String payload);
    bool publish(String topic, String payload, bool retained, int qos);
    bool publish(const char* topic, const uint8_t* payload, unsigned int length, bool retained = false, int qos = 0);
    bool beginPublish(const char* topic, unsigned int size, bool retained = false, int qos = 0);
    size_t write(const uint8_t* buf, size_t size);
    size_t write(uint8_t b);
    bool endPublish();

    bool subscribe(const char* topic);
    bool subscribe(const char* topic, int qos);
    bool unsubscribe(const char* topic);

    const char* getIncomingTopic();
    const char* getIncomingMessage();
    void setIncomingTopic(const char* topic);
    void setIncomingMessage(const char* message);

    void setWill(const char* topic, const char* payload, bool retained = false, int qos = 0);
    void clearWill();

    void setKeepAlive(int keepAlive);
    void setCleanSession(bool cleanSession);
    void disconnect();

  private:
    const char* _ssid;
    const char* _password;
    const char* _mqtt_server;
    const char* _mqtt_user;
    const char* _mqtt_pass;
    int _mqtt_port;
    WiFiClient _internalNet;
    Client* _net;
    MQTTClient _client;
    char _incomingTopic[100];
    char _incomingMessage[100];
    int _state;
    unsigned long _lastReconnectTry;

    uint8_t* _streamBuf = nullptr;
    unsigned int _streamSize = 0;
    unsigned int _streamPos = 0;
    const char* _streamTopic = nullptr;
    bool _streamRetained = false;
    int _streamQos = 0;

    struct {
      char topic[64];
      int qos;
      bool active;
    } _subs[AVISHAMQTT_MAX_SUBS];

    bool connectWiFi(unsigned long timeout_ms = 15000);
    bool connectMQTT(unsigned long timeout_ms = 15000);
    void restoreSubscriptions();
    void onMessage(String &topic, String &payload);
};

#endif
