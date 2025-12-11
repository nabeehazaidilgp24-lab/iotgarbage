#include <Arduino.h>
#include <WiFi.h>  
#include <NinjaIoT.h>

#define TRIG_PIN D3
#define ECHO_PIN D2   

NinjaIoT iot;

const char* ssid = "";
const char* password = "";
const String UID = "";

void setup() {
    // Pin modes
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    digitalWrite(TRIG_PIN, LOW);

    
    iot.connect(ssid, password, UID);
}

void loop() {
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    int echoState = digitalRead(ECHO_PIN);

    iot.WriteVar("D2", String(echoState));
    iot.SyncOut("D2");

    delay(200);
}
