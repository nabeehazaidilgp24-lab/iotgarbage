#ifndef NinjaIoT_h
#define NinjaIoT_h

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>


class NinjaIoT {
  public:
    void connect(const char* ssid, const char* password, const String& uid);
    String ReadAll();
    void SyncOut(const String& field);
    void SyncIN(const String& field);
    void SyncPWM(const String& field);
    String SyncVar(const String& variable);

    void WriteVar(const String& variable, const String& value);
    void WriteVar(const String& variable, int value);
    void WriteVar(const String& variable, float value);

  private:
    String UID;
    WiFiClientSecure client;
    String cachedJson;

    int getGPIO(const String& field);
    void writeField(const String& field, const String& value);

    String parseJsonValue(const String& json, const String& key);
    String readField(const String& field);
};

#endif