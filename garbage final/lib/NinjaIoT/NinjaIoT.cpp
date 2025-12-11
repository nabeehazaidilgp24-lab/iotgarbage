#include "NinjaIoT.h"

void NinjaIoT::connect(const char* ssid, const char* password, const String& uid) {
  UID = uid;
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  client.setInsecure(); // Bypass SSL
  cachedJson = "";      // Clear cached JSON
}

String NinjaIoT::ReadAll() {
  if (WiFi.status() != WL_CONNECTED) return "";

  HTTPClient https;
  String url = "https://iot.roboninja.in/index.php?action=read_all&UID=" + UID;
  https.begin(client, url);
  int httpCode = https.GET();
  String payload = "";

  if (httpCode > 0) {
    payload = https.getString();
    Serial.println("ReadAll: " + payload);
  } else {
    Serial.println("HTTP Error: " + https.errorToString(httpCode));
  }
  https.end();

  delay(50);  // Rate limit to avoid flooding
  cachedJson = payload;  // Cache the JSON for parsing later
  return payload;
}

String NinjaIoT::parseJsonValue(const String& json, const String& key) {
  int keyIndex = json.indexOf("\"" + key + "\"");
  if (keyIndex == -1) return "";

  int colonIndex = json.indexOf(':', keyIndex);
  if (colonIndex == -1) return "";

  int firstQuote = json.indexOf('\"', colonIndex);
  if (firstQuote == -1) return "";

  int secondQuote = json.indexOf('\"', firstQuote + 1);
  if (secondQuote == -1) return "";

  return json.substring(firstQuote + 1, secondQuote);
}

String NinjaIoT::readField(const String& field) {
  // Use cached JSON to read values
  if (cachedJson == "") {
    ReadAll();  // fetch and cache JSON if empty
  }
  return parseJsonValue(cachedJson, field);
}

void NinjaIoT::SyncOut(const String& field) {
  int pin = getGPIO(field);
  if (pin == -1) {
    Serial.println("SyncOut: Invalid pin name.");
    return;
  }

  pinMode(pin, OUTPUT);
  String value = readField(field);
  digitalWrite(pin, value == "1" ? HIGH : LOW);
  Serial.println("SyncOut: Set " + field + " → " + value);
}

void NinjaIoT::SyncIN(const String& field) {
  int pin = getGPIO(field);
  if (pin == -1) {
    Serial.println("SyncIN: Invalid pin name.");
    return;
  }

  pinMode(pin, INPUT_PULLUP);
  int value = digitalRead(pin);
  writeField(field, String(value));
  Serial.println("SyncIN: Sent " + field + " → " + String(value));
}

void NinjaIoT::SyncPWM(const String& field) {
  int pin = getGPIO(field);
  if (pin == -1) {
    Serial.println("SyncPWM: Invalid pin name.");
    return;
  }

  pinMode(pin, OUTPUT);
  String valueStr = readField(field);
  int pwmValue = valueStr.toInt();

  if (pwmValue < 0) pwmValue = 0;
  if (pwmValue > 1023) pwmValue = 1023;

  analogWrite(pin, pwmValue);
  Serial.println("SyncPWM: PWM " + field + " = " + String(pwmValue));
}

String NinjaIoT::SyncVar(const String& variable) {
  // Return variable value from cached JSON
  if (cachedJson == "") {
    ReadAll();
  }
  String val = parseJsonValue(cachedJson, variable);
  Serial.println("SyncVar: " + variable + " = " + val);
  return val;
}

void NinjaIoT::WriteVar(const String& variable, const String& value) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient https;
  String url = "https://iot.roboninja.in/index.php?action=write&UID=" + UID + "&" + variable + "=" + value;
  https.begin(client, url);
  int httpCode = https.GET();
  https.end();

  if (httpCode > 0) {
    Serial.println("WriteVar OK: " + variable + "=" + value);
  } else {
    Serial.println("WriteVar failed: " + https.errorToString(httpCode));
  }

  delay(50);  // Rate limit
}

void NinjaIoT::WriteVar(const String& variable, int value) {
  WriteVar(variable, String(value));
}

void NinjaIoT::WriteVar(const String& variable, float value) {
  WriteVar(variable, String(value, 2));
}

void NinjaIoT::writeField(const String& field, const String& value) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient https;
  String url = "https://iot.roboninja.in/index.php?action=write&UID=" + UID + "&" + field + "=" + value;
  https.begin(client, url);
  int httpCode = https.GET();
  https.end();

  if (httpCode > 0) {
    Serial.println("Write OK: " + field + " = " + value);
  } else {
    Serial.println("Write failed: " + https.errorToString(httpCode));
  }

  delay(50);
}

int NinjaIoT::getGPIO(const String& field) {
  if (field == "D0") return 16;
  if (field == "D1") return 5;
  if (field == "D2") return 4;
  if (field == "D3") return 0;
  if (field == "D4") return 2;
  if (field == "D5") return 14;
  if (field == "D6") return 12;
  if (field == "D7") return 13;
  if (field == "D8") return 15;
  if (field == "A0") return A0;
  return -1;
}