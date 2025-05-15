#ifndef RC_CAR_H
#define RC_CAR_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// ========== KONFIGURACE AP ==========
extern const char* ssidAP;
extern const char* passwordAP;

// ========== KONFIGURACE LED PÁSKU ==========
extern const int LED_PIN;    
extern const int LED_COUNT;

// Globální proměnné režimů
extern int mainMode;   
extern int subMode;    
extern int brightness; 

// Vytvoření instance stripu a web serveru
extern Adafruit_NeoPixel strip;
extern WebServer server;

// Prototypy funkcí (web handler)
void handleRoot();
void handleSet();
void handleStyle();
void handleSliderSet();
void handleNotFound();

// Režimy LED
void turnOff();
void lightMode();
void beaconMode();

// Pomocné funkce pro efekty majáků
void beaconEffect(int red, int green, int blue, int count, int delayTime, int start = 0);
void beaconEffectDualOrange(int red, int green, int blue, int count, int delayTime);
void beaconEffectDualRed(int red, int green, int blue, int count, int delayTime);
void beaconEffectPink(int red, int green, int blue, int count, int delayTime);
#endif
