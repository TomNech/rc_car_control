#include "RC_Car.h"
//created TomNech
// ========== KONFIGURACE AP ==========
const char* ssidAP     = "ESP32_RCAUTO"; 
const char* passwordAP = "12345678";        

// ========== KONFIGURACE LED PÁSKU ==========
const int LED_PIN   = 2;   
const int LED_COUNT = 24;  

// Globální proměnné (definice)
int mainMode   = 0;
int subMode    = 0;
int brightness = 50;

// Vytvoření objektů
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WebServer server(80);

// -----------------------------------------------------------------
//   FUNKCE PRO WEB SERVER
// -----------------------------------------------------------------

void handleRoot() {
  // Příprava "disable" tříd podle mainMode
  String disableM0 = (mainMode == 0) ? " disabled" : "";
  String disableM1 = (mainMode == 1) ? " disabled" : "";
  String disableM2 = (mainMode == 2) ? " disabled" : "";

  // Začneme HTML. V hlavičce místo <style> použijeme <link rel="stylesheet" href="/style.css">
  String html = R"====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 control</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <div class="container">
    <h3>RC CAR (ESP32)</h3>
    <p class="info">
      Actual mainMode:
)====";

  // Vložíme dynamicky mainMode, subMode
  html += String(mainMode);
  html += ", subMode: ";
  html += String(subMode);
  html += "</p>";

  // Tlačítka

  html += "<a href='/set?mainMode=0&subMode=0' class='button" + disableM0 + "'>DISABLE 1 (OFF)</a>";
  html += "<a href='/set?mainMode=1&subMode=1' class='button" + disableM2 + "'>WHITE LIGHT (MIN)</a>";
  html += "<a href='/set?mainMode=1&subMode=2' class='button" + disableM2 + "'>WHITE LIGHT (MAX)</a>";
  html += "<a href='/set?mainMode=2&subMode=1' class='button" + disableM1 + "'>BEACONS MODE1</a>";
  html += "<a href='/set?mainMode=2&subMode=2' class='button" + disableM1 + "'>BEACONS MODE2</a>";
  html += "<a href='/set?mainMode=2&subMode=3' class='button" + disableM1 + "'>BEACONS MODE3</a>";
  html += "<a href='/set?mainMode=2&subMode=4' class='button" + disableM1 + "'>BEACONS MODE4</a>";
  html += "<a href='/set?mainMode=2&subMode=5' class='button" + disableM1 + "'>BEACONS MODE5</a>";

  // Posuvník
  html += "<h2>Brightness SET (0-100%)</h2>";
  html += "<p>" + String(brightness) + "</p>";

  // Okamžitý GET požadavek (oninput) na /sliderset
  html += "<input class='slider' type='range' min='0' max='255' value='" + String(brightness) + "' ";
  html += "oninput=\"window.location='/sliderset?brightness=' + this.value\" />";

  // Uzavření container, body, html
  html += R"====(
  </div>
</body>
</html>
)====";

  server.send(200, "text/html", html);
}

void handleStyle() {
  // Vracíme CSS jako text
  String css = R"====(
body {
  margin: 0;              /* Zruší výchozí okraje prohlížeče */
  padding: 20px;
  background-color: #f0f0f0;
  font-family: Arial, sans-serif;
}
.container {
  width: 100%;
  max-width: 400px;       /* Container max 400 px, zbytek je prázdný */
  margin: 0 auto;         /* Horizontální zarovnání na střed */
  text-align: center;     /* Text uvnitř na střed */
  background-color: #fff;
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 0 10px rgba(0,0,0,0.2);
}
h3 {
  margin-top: 0;
  color: #333;
}
.info {
  background-color: #b3e5fc;
  padding: 10px;
  border-radius: 5px;
  margin: 10px 0;
  color: #000;
}
/* Odkaz = tlačítko */
.button {
  display: block;         /* Jeden blok pod druhým */
  width: auto;            /* Šířka tlačítka - lze i 100% */
  margin: 5px 0;
  padding: 12px;
  background-color: #039be5;
  color: #fff;
  font-size: 16px;
  text-decoration: none;  /* Vypne podtržení */
  text-align: center;     /* Text na střed uvnitř tlačítka */
  border-radius: 4px;
  cursor: pointer;
}
.button:hover {
  background-color: #0277bd;
}
.button.disabled {
  background-color: #ccc;
  pointer-events: none;   /* Nepůjde kliknout */
  cursor: not-allowed;
}
.slider {
  width: 100%;
  margin: 20px 0;
}
)====";

  server.send(200, "text/css", css);
}


void handleSet() {
  if (server.hasArg("mainMode")) {
    mainMode = server.arg("mainMode").toInt();
  }
  if (server.hasArg("subMode")) {
    subMode = server.arg("subMode").toInt();
  }
  // Přesměrování
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSliderSet() {
  if (server.hasArg("brightness")) {
    int value = server.arg("brightness").toInt();
    if (value < 0)   value = 0;
    if (value > 255) value = 255;
    brightness = value;

    strip.setBrightness(brightness);
    strip.show();
  }
  // Přesměrování zpět na "/"
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

// -----------------------------------------------------------------
//   REŽIMY LED
// -----------------------------------------------------------------

void turnOff() {
  strip.clear();
  strip.show();
}

void lightMode() {
  if (subMode == 0) {
    turnOff();
  } else if (subMode == 1) {
    strip.fill(strip.Color(255, 255, 255), 0, LED_COUNT); 
    strip.setBrightness(brightness);
    strip.show();
  } else if (subMode == 2) {
    strip.fill(strip.Color(255, 255, 255), 0, LED_COUNT);
    strip.setBrightness(brightness);
    strip.show();
  }
}

void beaconMode() {
  if (subMode == 0) {
    turnOff();
  } else if (subMode == 1) {
    beaconEffect(255, 0, 0, 12, 70);
    delay(30);
    beaconEffect(0, 0, 255, 12, 70, 12);
    delay(30);
  } else if (subMode == 2) {
    beaconEffect(255, 145, 5, 12, 70);
    delay(30);
    beaconEffect(255, 145, 5, 12, 70, 12);
    delay(30);
  } else if (subMode == 3) {
    beaconEffectDualOrange(255, 145, 5, 5, 70);
    delay(30);
  } else if (subMode == 4) {
    beaconEffectDualRed(255, 0, 0, 5, 70);
    delay(30);
  } else if (subMode == 5) {
    beaconEffectPink(255, 105, 180, 5, 70);
    delay(30);
  }
}

// -----------------------------------------------------------------
//   EFEKTY
// -----------------------------------------------------------------

void beaconEffect(int red, int green, int blue, int count, int delayTime, int start) {
  for (int i = 0; i < 2; i++) {
    for (int j = start; j < start + count; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    strip.show();
    delay(delayTime);
    for (int j = start; j < start + count; j++) {
      strip.setPixelColor(j, 0);
    }
    strip.show();
    delay(delayTime);
  }
}

void beaconEffectDualOrange(int red, int green, int blue, int count, int delayTime) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < count; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    for (int j = LED_COUNT - count; j < LED_COUNT; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    strip.show();
    delay(delayTime);

    for (int j = 0; j < count; j++) {
      strip.setPixelColor(j, 0);
    }
    for (int j = LED_COUNT - count; j < LED_COUNT; j++) {
      strip.setPixelColor(j, 0);
    }
    strip.show();
    delay(delayTime);
  }
  
  for (int i = 0; i < 2; i++) {
    for (int j = 7; j < 17; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    strip.show();
    delay(delayTime);
    for (int j = 7; j < 17; j++) {
      strip.setPixelColor(j, 0);
    }
    strip.show();
    delay(delayTime);
  }
}

void beaconEffectDualRed(int red, int green, int blue, int count, int delayTime) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < count; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    for (int j = LED_COUNT - count; j < LED_COUNT; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    strip.show();
    delay(delayTime);

    for (int j = 0; j < count; j++) {
      strip.setPixelColor(j, 0);
    }
    for (int j = LED_COUNT - count; j < LED_COUNT; j++) {
      strip.setPixelColor(j, 0);
    }
    strip.show();
    delay(delayTime);
  }

  for (int i = 0; i < 2; i++) {
    for (int j = 7; j < 17; j++) {
      strip.setPixelColor(j, strip.Color(0, 0, 255));
    }
    strip.show();
    delay(delayTime);
    for (int j = 7; j < 17; j++) {
      strip.setPixelColor(j, 0);
    }
    strip.show();
    delay(delayTime);
  }

}

void beaconEffectPink(int red, int green, int blue, int count, int delayTime) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < count; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    for (int j = LED_COUNT - count; j < LED_COUNT; j++) {
      strip.setPixelColor(j, strip.Color(red, green, blue));
    }
    strip.show();
    delay(delayTime);
    }
}