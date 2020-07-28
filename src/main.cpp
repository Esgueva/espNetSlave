/*
 platformio run --target uploadfs
*/
#include <Arduino.h>
#include <global.h>
#include <appPreferences.cpp>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <math.h>

#include <iostream>
#include <exception>

#include <network.cpp>

#include <bme280.cpp>
#include <tools.cpp>

//prototype
void bme280Init();
String readbme280();
void getReadings();
void updateDisplay();

boolean sensorsActive = false;

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    _APP_DEBUG_("INIT", "An Error has occurred while mounting SPIFFS");
    return;
  }

  _APP_DEBUG_("", "SPIFFS mounted");
#ifdef _APP_DEBUG_ON_
  /*Muestra el contenido de la memoria flash*/
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    _APP_DEBUG_("FILE:", file.name());
    file = root.openNextFile();
  }

  _APP_DEBUG_("SPIFFS totalBytes", SPIFFS.totalBytes());
  _APP_DEBUG_("SPIFFS usedBytes", SPIFFS.usedBytes());
#endif

  delay(10);
  //cleanWifiPreferences();

  appPreferences.ssid_ap = getPreferenceByName("SSID_AP", "");
  appPreferences.pass_ap = getPreferenceByName("PASS_AP", "");
  appPreferences.mac_ap  = getPreferenceByName("MAC_AP", "");
  appPreferences.chan_ap = getPreferenceByName("CHAN_AP", "");

#ifdef _APP_DEBUG_ON_

  _APP_DEBUG_("Pref: ssid_ap", appPreferences.ssid_ap);
  _APP_DEBUG_("Pref: pass_ap", appPreferences.pass_ap);
  _APP_DEBUG_("Pref: mac_ap",appPreferences.mac_ap);
  _APP_DEBUG_("Pref: chan_ap", appPreferences.chan_ap);

#endif

  //char buffer = getPreferenceBytes("broadcastMaster");

  if (appPreferences.ssid_ap == "" && appPreferences.pass_ap == "" && appPreferences.mac_ap == "" && appPreferences.chan_ap == "")
  {
    _APP_DEBUG_("INIT", "Primer arranque");
    scanNetworks();
    networkAPInit();
    serverInit();
  }
  else if (appPreferences.ssid_ap != "" && appPreferences.pass_ap != "" && appPreferences.mac_ap != "" && appPreferences.chan_ap != "")
  {
    //Inicar red en modo STATION
    _APP_DEBUG_("INIT", "Aplicando preferencias");
    sensorsActive = true;
    networkStationInit();
    espnowInit();
    broadcastInit();
    // Init BME280 sensor
    bme280Init();
  }

}

void loop()
{
  //Actualiza los valores del sensor temp,hum,pres

  if (sensorsActive)
  {
    String msg = readbme280();
     _APP_DEBUG_("SEND", msg);
    espnowSend(msg);
    //verIncomingReadings();
  }

  delay(5000);
}