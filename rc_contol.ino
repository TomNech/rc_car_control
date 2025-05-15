#include "RC_Car.h" 

void setup() {
  Serial.begin(115200);

  // Inicializace LED pásku
  strip.begin();
  strip.clear();
  strip.setBrightness(brightness);
  strip.show();

  // Nastavení režimu AP (Access Point)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidAP, passwordAP);

  // Získáme IP a spustíme server
  server.on("/", handleRoot);         // HTML
  server.on("/style.css", handleStyle);   // CSS
  server.on("/sliderset", handleSliderSet); // atd.
  server.on("/set", handleSet);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop() {
  // Obsluhujeme HTTP klienty
  server.handleClient();

  // Ovládáme LED podle mainMode a subMode
  if (mainMode == 0) {
    turnOff();
  } else if (mainMode == 1) {
    lightMode();
  } else if (mainMode == 2) {
    beaconMode();
  }

  delay(10);
}
