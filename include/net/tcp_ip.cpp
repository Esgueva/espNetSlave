#if defined(ESP32)
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include "esp_wifi.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <models/net/wifi.h>

String scanNetworks();

String getPreferenceByName(String name, String defaultValue);
void setPreferenceByName(String name, String value);

AsyncWebServer server(80);

WifiPoint device;


/*
* Busca la redes y devulve un string JSON
*/
String scanNetworks()
{
  _APP_DEBUG_("", "scanNetworks()");

  int8_t scanResults = WiFi.scanNetworks();

  String json;

  if (scanResults == 0)
  {
    _APP_DEBUG_("Scan Networks", "No WiFi devices in AP Mode found");
  }
  else
  {
    Serial.print("Found ");
    Serial.print(scanResults);
    Serial.println(" devices ");

    DynamicJsonDocument doc(2048);
    JsonArray apList = doc.to<JsonArray>();

    for (int i = 0; i < scanResults; i++)
    {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);
      byte CHANNEL = WiFi.channel(i);

      JsonObject ap = apList.createNestedObject();
      ap["ssid"] = SSID;
      ap["mac"] = BSSIDstr;
      ap["rssi"] = RSSI;
      ap["ch"] = CHANNEL;

      if (PRINTSCANRESULTS)
      {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(SSID);
        Serial.print(" [");
        Serial.print(BSSIDstr);
        Serial.print("]");
        Serial.print(" (");
        Serial.print(RSSI);
        Serial.print(")");
        Serial.print(" Ch ");
        Serial.print(CHANNEL);
        Serial.println("");
      }

      delay(10);

    } //for

    // clean up ram
    WiFi.scanDelete();

    serializeJson(doc, json);
    Serial.println(json);
  }

  return json;
}


void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  // Handling function code
  _APP_DEBUG_(F("WiFiStationConnected"), event);
  _APP_DEBUG_(F("INIT"), F("Connected to Access Point OK"));
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  // Handling function code
  _APP_DEBUG_(F("WiFiStationDisconnected"), event);
  _APP_DEBUG_(F("INIT"), "WiFi.disconnect()");
}

//ESP 32
void networkStationInit()
{
  _APP_DEBUG_(F("NetWork Init"), F("Station Mode"));
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  _APP_DEBUG_(F("SSID AP to auth"), device.ssid_ap);
  _APP_DEBUG_(F("PASS"), device.pass_ap);

  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

  WiFi.begin(device.ssid_ap.c_str(), device.pass_ap.c_str(), device.chan_ap.toInt());
  _APP_DEBUG_(F("Connecting to Access Point"), "");

  while (WiFi.status() != WL_CONNECTED)
  {
    #if defined(_APP_DEBUG_ON_)
        Serial.print(F("."));
    #endif
    wait(100);
  }
}

/*
  Inicia la configuracion WEB  del dispositivo
*/
void networkAPInit()
{
  _APP_DEBUG_(F("NETWORK INIT"), ("WIFI MODE AP for steup"));
  // Set device as a Wi-Fi Ap to setup
  WiFi.mode(WIFI_AP);
  //Nombre de red, password, canal, visibilidad
  WiFi.softAP(WiFi.macAddress().c_str(), WIFI_PASSWORD_AP_DEFAULT, CHAN_AP_DEFAULT, false, 4);
  _APP_DEBUG_(F("Soft AP IP"), WiFi.softAPIP());
}



void serverInit()
{
  // Route for root / web page
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html");
  });

  // Route to load style.css file
  server.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/bootstrap.min.css", "text/css");
  });

  server.on("/css/bootstrap-select.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/bootstrap-select.min.css", "text/css");
  });

  // Route to load style.css file
  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/style.css", "text/css");
  });

  server.on("/js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/jquery.min.js", "text/js");
  });

  server.on("/js/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/bootstrap.min.js", "text/js");
  });

  server.on("/js/popper.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/popper.min.js", "text/js");
  });

  server.on("/js/bootstrap-select.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/bootstrap-select.min.js", "text/js");
  });

  /*
      PETICONES 
  */

  // Obtiene la lista de redes wifi
  server.on("/apList", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", scanNetworks().c_str());
  });

  // server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SPIFFS, "/favicon.ico", "image/ico");
  // });

  // server.on("/logo", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SPIFFS, "/logo.png", "image/png");
  // });

  server.on("/getFormSetup", HTTP_POST, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    if (request->hasParam("apList", true))
    {
      setPreferenceByName("SSID_AP", request->getParam("apList", true)->value());
      inputMessage = getPreferenceByName("SSID_AP", "");
      inputParam = "SSID_AP";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }

    if (request->hasParam("pwd", true))
    {
      setPreferenceByName("PASS_AP", request->getParam("pwd", true)->value());
      inputMessage = getPreferenceByName("PASS_AP", "");
      inputParam = "PASS_AP";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }

    if (request->hasParam("mac", true))
    {
      setPreferenceByName("MAC_AP", request->getParam("mac", true)->value());
      inputMessage = getPreferenceByName("MAC_AP", "");
      inputParam = "MAC_AP";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }

    if (request->hasParam("channel", true))
    {
      setPreferenceByName("CHAN_AP", request->getParam("channel", true)->value());
      inputMessage = getPreferenceByName("CHAN_AP", "");
      inputParam = "CHAN_AP";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }

    if (request->hasParam("deviceName", true))
    {
      setPreferenceByName("DEVICE_NAME", request->getParam("deviceName", true)->value());
      inputMessage = getPreferenceByName("DEVICE_NAME", "");
      inputParam = "DEVICE_NAME";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }
    /*
    if (request->hasParam("uri", true))
    {
      setPreferenceByName(WEBSOCKET_URI, request->getParam("uri", true)->value());
      inputMessage = getPreferenceByName(WEBSOCKET_URI,"");
      inputParam = "URI";
       _APP_DEBUG_(inputParam + " " + inputMessage);
    }
    */

    //ESP.restart();

    request->send(200, "text/html", "HTTP FORM POST sent to Slave"
                                    "<body class='bg-request'>"
                                    "<link rel='stylesheet' type='text/css' href='css/style.css'>"
                                    "<br><a  href='/setup\'>Return to Home Page</a>"
                                    "<img class='' src='logo' alt='Developers United' />"
                                    "</body>");
  });

  server.begin();
}

void initComunications(){

  if (device.ssid_ap == "" && device.pass_ap == "" && device.mac_ap == "" && device.chan_ap == "")
  {
    _APP_DEBUG_("INIT", "Primer arranque");

    #if defined(_APP_DEBUG_ON_)
      scanNetworks();
    #endif 

    networkAPInit();
    serverInit();
  }
  else if (device.ssid_ap != "" && device.pass_ap != "" && device.mac_ap != "" && device.chan_ap != "")
  {

    //Inicar red en modo STATION
    _APP_DEBUG_("INIT", "Aplicando preferencias");
    networkStationInit();
    espnowInit();
    broadcastInit();
    // Init BME280 sensor
    sensorsBme280Active = bme280Init();
  }
}