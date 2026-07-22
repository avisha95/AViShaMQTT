#include "AViShaMQTT.h"

AViShaMQTT::AViShaMQTT(const char* ssid, const char* password, const char* mqtt_server,
                 int mqtt_port, const char* mqtt_user, const char* mqtt_pass,
                 int bufferSize)
  : _ssid(ssid), _password(password), _mqtt_server(mqtt_server), _mqtt_port(mqtt_port),
    _mqtt_user(mqtt_user), _mqtt_pass(mqtt_pass), _net(&_internalNet),
    _client(bufferSize), _state(MQTT_DISCONNECTED), _lastReconnectTry(0) {
  for (int i = 0; i < AVISHAMQTT_MAX_SUBS; i++) {
    _subs[i].active = false;
  }
}

AViShaMQTT::AViShaMQTT(const char* ssid, const char* password, const char* mqtt_server,
                 int mqtt_port, Client& net, const char* mqtt_user, const char* mqtt_pass,
                 int bufferSize)
  : _ssid(ssid), _password(password), _mqtt_server(mqtt_server), _mqtt_port(mqtt_port),
    _mqtt_user(mqtt_user), _mqtt_pass(mqtt_pass), _net(&net),
    _client(bufferSize), _state(MQTT_DISCONNECTED), _lastReconnectTry(0) {
  for (int i = 0; i < AVISHAMQTT_MAX_SUBS; i++) {
    _subs[i].active = false;
  }
}

bool AViShaMQTT::begin() {
#if defined(ESP32) && defined(AVISHAMQTT_DISABLE_BROWNOUT)
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
#endif
  _state = MQTT_DISCONNECTED;

  if (!connectWiFi(15000)) {
    return false;
  }

  _client.begin(_mqtt_server, _mqtt_port, *_net);
  _client.onMessage([this](String & topic, String & payload) {
    this->onMessage(topic, payload);
  });

  if (!connectMQTT(15000)) {
    return false;
  }

  return true;
}

void AViShaMQTT::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (_state == MQTT_CONNECTED) {
      _state = MQTT_CONNECTION_LOST;
    }
    if (millis() - _lastReconnectTry >= 3000) {
      _lastReconnectTry = millis();
      connectWiFi(5000);
    }
    return;
  }

  _client.loop();

  if (!_client.connected()) {
    if (_state == MQTT_CONNECTED) {
      _state = MQTT_CONNECTION_LOST;
    }
    if (millis() - _lastReconnectTry >= 3000) {
      _lastReconnectTry = millis();
      if (connectMQTT(10000)) {
        restoreSubscriptions();
      }
    }
  }
}

bool AViShaMQTT::isConnected() {
  return _state == MQTT_CONNECTED && _client.connected();
}

int AViShaMQTT::state() {
  if (_state == MQTT_CONNECTED && !_client.connected()) {
    _state = MQTT_CONNECTION_LOST;
  }
  return _state;
}

bool AViShaMQTT::publish(String topic, String payload) {
  bool result = _client.publish(topic, payload);
  if (result) {
    Serial.println("[MQTT] >> PUB [QoS0] Sent Topic: " + topic + ", Payload: " + payload);
  } else {
    Serial.println("[MQTT] >> PUB [QoS0] FAILED Topic: " + topic);
  }
  return result;
}

bool AViShaMQTT::publish(String topic, String payload, bool retained, int qos) {
  bool result = _client.publish(topic, payload, retained, qos);
  if (result) {
    String ack = (qos == 0) ? "Sent" : ((qos == 1) ? "+PUBACK" : "+PUBCOMP");
    Serial.println("[MQTT] >> PUB [QoS" + String(qos) + "]" + String(retained ? " [R]" : "") + " " + ack + " Topic: " + topic + ", Payload: " + payload);
  } else {
    String fail = (qos <= 0) ? "FAILED" : ((qos == 1) ? "!PUBACK" : "!PUBCOMP");
    Serial.println("[MQTT] >> PUB [QoS" + String(qos) + "] " + fail + " Topic: " + topic);
  }
  return result;
}

bool AViShaMQTT::publish(const char* topic, const uint8_t* payload, unsigned int length, bool retained, int qos) {
  bool result = _client.publish(topic, (const char*)payload, (int)length, retained, qos);
  if (result) {
    String ack = (qos == 0) ? "Sent" : ((qos == 1) ? "+PUBACK" : "+PUBCOMP");
    Serial.println("[MQTT] >> PUB [QoS" + String(qos) + "]" + String(retained ? " [R]" : "") + " " + ack + " Topic: " + String(topic) + ", " + String(length) + " bytes");
  } else {
    String fail = (qos <= 0) ? "FAILED" : ((qos == 1) ? "!PUBACK" : "!PUBCOMP");
    Serial.println("[MQTT] >> PUB [QoS" + String(qos) + "] " + fail + " Topic: " + String(topic));
  }
  return result;
}

bool AViShaMQTT::beginPublish(const char* topic, unsigned int size, bool retained, int qos) {
  if (_streamBuf != nullptr) {
    free(_streamBuf);
  }
  _streamBuf = (uint8_t*)malloc(size + 1);
  if (_streamBuf == nullptr) {
    Serial.println("[MQTT] >> STREAM ERROR: malloc failed (" + String(size) + " bytes)");
    return false;
  }
  _streamSize = size;
  _streamPos = 0;
  _streamTopic = topic;
  _streamRetained = retained;
  _streamQos = qos;
  Serial.println("[MQTT] >> STREAM begin Topic: " + String(topic) + ", Size: " + String(size));
  return true;
}

size_t AViShaMQTT::write(uint8_t b) {
  return write(&b, 1);
}

size_t AViShaMQTT::write(const uint8_t* buf, size_t size) {
  if (_streamBuf == nullptr) return 0;
  size_t room = _streamSize - _streamPos;
  size_t n = (size < room) ? size : room;
  memcpy(_streamBuf + _streamPos, buf, n);
  _streamPos += n;
  return n;
}

bool AViShaMQTT::endPublish() {
  if (_streamBuf == nullptr) {
    Serial.println("[MQTT] >> STREAM ERROR: no active stream");
    return false;
  }
  _streamBuf[_streamPos] = '\0';
  bool result = _client.publish(_streamTopic, (const char*)_streamBuf, (int)_streamPos, _streamRetained, _streamQos);
  if (result) {
    String ack = (_streamQos == 0) ? "Sent" : ((_streamQos == 1) ? "+PUBACK" : "+PUBCOMP");
    Serial.println("[MQTT] >> PUB [QoS" + String(_streamQos) + "]" + String(_streamRetained ? " [R]" : "") + " " + ack + " Topic: " + String(_streamTopic) + ", " + String(_streamPos) + " bytes");
  } else {
    String fail = (_streamQos <= 0) ? "FAILED" : ((_streamQos == 1) ? "!PUBACK" : "!PUBCOMP");
    Serial.println("[MQTT] >> PUB [QoS" + String(_streamQos) + "] " + fail + " Topic: " + String(_streamTopic));
  }
  free(_streamBuf);
  _streamBuf = nullptr;
  _streamSize = 0;
  _streamPos = 0;
  return result;
}

bool AViShaMQTT::subscribe(const char* topic) {
  return subscribe(topic, 0);
}

bool AViShaMQTT::subscribe(const char* topic, int qos) {
  bool result = _client.subscribe(topic, qos);
  if (result) {
    for (int i = 0; i < AVISHAMQTT_MAX_SUBS; i++) {
      if (!_subs[i].active) {
        strncpy(_subs[i].topic, topic, sizeof(_subs[i].topic) - 1);
        _subs[i].topic[sizeof(_subs[i].topic) - 1] = '\0';
        _subs[i].qos = qos;
        _subs[i].active = true;
        break;
      }
    }
    Serial.println("[MQTT] << SUB [QoS" + String(qos) + "] Topic: " + String(topic));
  } else {
    Serial.println("[MQTT] << SUB FAILED Topic: " + String(topic));
  }
  return result;
}

bool AViShaMQTT::unsubscribe(const char* topic) {
  bool result = _client.unsubscribe(topic);
  if (result) {
    for (int i = 0; i < AVISHAMQTT_MAX_SUBS; i++) {
      if (_subs[i].active && strcmp(_subs[i].topic, topic) == 0) {
        _subs[i].active = false;
        break;
      }
    }
    Serial.println("[MQTT] << UNSUB Topic: " + String(topic));
  } else {
    Serial.println("[MQTT] << UNSUB FAILED Topic: " + String(topic));
  }
  return result;
}

const char* AViShaMQTT::getIncomingTopic() {
  return _incomingTopic;
}

const char* AViShaMQTT::getIncomingMessage() {
  return _incomingMessage;
}

void AViShaMQTT::setIncomingTopic(const char* topic) {
  strncpy(_incomingTopic, topic, sizeof(_incomingTopic) - 1);
  _incomingTopic[sizeof(_incomingTopic) - 1] = '\0';
}

void AViShaMQTT::setIncomingMessage(const char* message) {
  strncpy(_incomingMessage, message, sizeof(_incomingMessage) - 1);
  _incomingMessage[sizeof(_incomingMessage) - 1] = '\0';
}

void AViShaMQTT::setWill(const char* topic, const char* payload, bool retained, int qos) {
  _client.setWill(topic, payload, retained, qos);
  Serial.println("[MQTT] << WILL [QoS" + String(qos) + "]" + String(retained ? " [R]" : "") + " Topic: " + String(topic));
}

void AViShaMQTT::clearWill() {
  _client.clearWill();
  Serial.println("[MQTT] << WILL cleared");
}

void AViShaMQTT::setKeepAlive(int keepAlive) {
  _client.setKeepAlive(keepAlive);
}

void AViShaMQTT::setCleanSession(bool cleanSession) {
  _client.setCleanSession(cleanSession);
  Serial.println("[MQTT] Clean Session: " + String(cleanSession ? "true" : "false (persistent)"));
}

void AViShaMQTT::disconnect() {
  _client.disconnect();
  _state = MQTT_DISCONNECTED;
  Serial.println("[MQTT] !! Disconnected");
}

bool AViShaMQTT::connectWiFi(unsigned long timeout_ms) {
  Serial.print("[WIFI]: Connecting...");

  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout_ms) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK");
    return true;
  }

  Serial.println(" FAILED");
  _state = MQTT_CONNECT_FAILED;
  return false;
}

bool AViShaMQTT::connectMQTT(unsigned long timeout_ms) {
  String clientId = "AViShaMQTT-" + String(random(0xffff), HEX);
  unsigned long start = millis();

  Serial.print("[MQTT]: Connecting...");

  while (millis() - start < timeout_ms) {
    bool connected = false;

    if (_mqtt_user && _mqtt_pass) {
      connected = _client.connect(clientId.c_str(), _mqtt_user, _mqtt_pass);
    } else {
      connected = _client.connect(clientId.c_str());
    }

    if (connected) {
      _state = MQTT_CONNECTED;
      Serial.println(" OK");
      return true;
    }

    int rc = (int)_client.returnCode();
    if (rc >= 1 && rc <= 5) {
      switch (rc) {
        case 1: _state = MQTT_CONNECT_BAD_PROTOCOL; break;
        case 2: _state = MQTT_CONNECT_BAD_CLIENT_ID; break;
        case 3: _state = MQTT_CONNECT_UNAVAILABLE; break;
        case 4: _state = MQTT_CONNECT_BAD_CREDENTIALS; break;
        case 5: _state = MQTT_CONNECT_UNAUTHORIZED; break;
      }
      Serial.println(" DENIED (code: " + String(rc) + ")");
      return false;
    }

    delay(500);
    Serial.print(".");
  }

  _state = MQTT_CONNECTION_TIMEOUT;
  Serial.println(" TIMEOUT");
  return false;
}

void AViShaMQTT::restoreSubscriptions() {
  for (int i = 0; i < AVISHAMQTT_MAX_SUBS; i++) {
    if (_subs[i].active) {
      _client.subscribe(_subs[i].topic, _subs[i].qos);
      Serial.println("[MQTT] << SUB restore [QoS" + String(_subs[i].qos) + "] Topic: " + String(_subs[i].topic));
    }
  }
}

void AViShaMQTT::onMessage(String &topic, String &payload) {
  setIncomingTopic(topic.c_str());
  setIncomingMessage(payload.c_str());
  Serial.println("[MQTT] << RX Topic: " + topic + ", Payload: " + payload);
}
