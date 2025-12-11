#include <NinjaIoT.h>

NinjaIoT iot;

void setup() {
  Serial.begin(115200);
  iot.connect("wifi-name", "wifi-pass", "UIDofIoTPlatform");   //link: https://iot.roboninja.in/
}

void loop() {
  iot.ReadAll(); // Read all values from the cloud

  // Read button state on D0 and upload it
  iot.SyncIN("D0");

  delay(50);  // wait 50 milliseconds
}
