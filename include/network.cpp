#if defined(ESP32)
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <esp_now.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
extern "C"
{
#include <espnow.h>
}
#endif

AsyncWebServer server(80);

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress_default[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//uint8_t broadcastAddress_default[] = { 0x30, 0xAE, 0xA4, 0xDD, 0x94, 0xC0 }; //Original
//uint8_t broadcastAddress[] = { 0xFC, 0xF5, 0xC4, 0x00, 0x31, 0xF5 }; //RODRI

esp_now_peer_info_t peerInfo;

// //Prorotype
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

String getPreferenceByName(String name, String defaultValue);
void setPreferenceByName(String name, String value);

// // Variable to store if sending data was successful
//String success;

bool msgRequest = false;
// si pasado este tiempo no recive respuesta reinica el dispositivo
unsigned long previousMillismsgRequestTimeOut = 0;
unsigned long espNowTimeOut = 0;
unsigned int msgTimeOut = 100;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{

   Serial.println("----------");
  Serial.println(status);
      Serial.println("----------");

  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    _APP_DEBUG_("OnDataSent","Delivery Success :)");
  }
  else
  {
    _APP_DEBUG_("OnDataSent", "Delivery Fail :(");
  }
}

// Callback when data is received

/*

{
  "gpio": 2,
  "value": "on/off"
}


 int system(const char *command);
{
  "command": ESP.restart(),
}

{
  "function":  cleanWifiPreferences(),
}


*/

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int incomingData_len)
{
  
  Serial.print("Bytes received: ");
  Serial.println(incomingData_len);

  Serial.println("RECV");
  char buffer[incomingData_len];
  memcpy(&buffer, incomingData, incomingData_len);

  Serial.println(buffer);


  int pos;

  pos = String(buffer).indexOf("msgRecv");

  //Comprueba si el mensaje fue entrgado 
  if(pos != -1){
      Serial.println("msgRecv");
      Serial.println(pos);
      msgRequest = false;
  }

  pos = String(buffer).indexOf("gpio");

  Serial.print("POS :: ");
  Serial.println(pos);

  if (pos != -1)
  {
    StaticJsonDocument<250> doc;
    DeserializationError err = deserializeJson(doc, buffer);
    int gpio = doc["gpio"];
    Serial.print("GPIO :::");
    Serial.println(gpio);
    int value = doc["value"];
    Serial.print("VALUE::::::: ");
    Serial.println(value);

    boolean pinStatus = digitalRead(gpio);
    Serial.println("-----------------");
    Serial.println(pinStatus);

    if(value == 1){
      Serial.println("high");
        digitalWrite(gpio, HIGH );
    }else{
       digitalWrite(gpio, LOW );
       Serial.println("low");
    }


    //int timer = doc["timer"]; // si es 0, es inifito
    // pinMode(gpio, OUTPUT);
    //  digitalWrite(gpio, value);

    // if(timer == 0){
    //     digitalWrite(gpio, value);
    //     return;
    // }

    //temporizador crear un hilo

    return;
  }

  pos = String(buffer).indexOf("command");

  if (pos != -1)
  {
    StaticJsonDocument<250> doc;
    DeserializationError err = deserializeJson(doc, buffer);
    String command = doc["command"];
    String value = doc["value"];
    Serial.print("COMMAND ::::::: ");
    Serial.println(command);
    Serial.print("Value ::::::: ");
    Serial.println(value);
    //system(command);
    return;
  }

  pos = String(buffer).indexOf("function");

  enum Functions
  {
    CLEAN_WIFI_PREFERENCES,
    RESTART
  };

  if (pos != -1)
  {
    StaticJsonDocument<250> doc;
    DeserializationError err = deserializeJson(doc, buffer);
    Functions functionToCall = doc["value"];
    Serial.print("FUNCTION ::::::: ");
    // Serial.println(functionToCall.c_str());
    switch (functionToCall)
    {
    case CLEAN_WIFI_PREFERENCES:
      cleanWifiPreferences();
      break;
    case RESTART:
      ESP.restart();
      break;
    default:
      break;
    }
    return;
  }
}

void espnowInit()
{

  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    _APP_DEBUG_("espnowInit()", "Error initializing ESP-NOW");
    ESP.restart();
    return;
  }

  _APP_DEBUG_("ESPNow", "Init Success!");

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

/*
  Covierte una ip o mac en valores tipo byte
  recibe un parametro que sirve de separador -> sep
  bytes almacena el dato tras la conversion
*/
void parseBytes(const char* str, char sep, uint8_t* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}


void broadcastInit()
{
  _APP_DEBUG_("Esp NOW", "broadcastInit()");
  _APP_DEBUG_("MAC", appPreferences.mac_ap);
  _APP_DEBUG_("CHAN", appPreferences.chan_ap);
  _APP_DEBUG_("wifi ch", WiFi.channel());

  //const char* macStr =  "24:62:AB:F3:08:D4";
  const char* macStr =  appPreferences.mac_ap.c_str();
  //byte mac[6];

  uint8_t mac[6];

  parseBytes(macStr, ':', mac, 6, 16);


  memcpy(&peerInfo.peer_addr, &mac, 6);

  peerInfo.channel = appPreferences.chan_ap.toInt();
  peerInfo.encrypt = false;
  peerInfo.ifidx = ESP_IF_WIFI_STA; //ESP_IF_WIFI_AP 

  // Add peer
  if (esp_now_add_peer(&peerInfo) == ESP_OK)
  {
     _APP_DEBUG_("broadcastInit()", "Added Master Node!");
  }else{
     _APP_DEBUG_("broadcastInit()", "Master Node could not be added...");
       bool exists = esp_now_is_peer_exist(mac);

        if (exists){
          _APP_DEBUG_("broadcastInit()", "Master Node Exist");
        }
  }
}

void espnowSend(String msg)
{

  esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t *)msg.c_str(), msg.length() + 1);

  if (result == ESP_OK)
  {
    _APP_DEBUG_("espnowSend", "Sent with success");
    espNowTimeOut = millis();
    msgRequest = true; //Esta pendiente de recibir confirmacin, si no recibe 1)No esta autorizado 2)No fue autorizado, y necesita validar
  }
  else
  {
    _APP_DEBUG_("espnowSend", "Error sending the data");
  }
}

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
  _APP_DEBUG_("WiFiStationConnected", event);
  _APP_DEBUG_("INIT", "Connected to Access Point ;)");
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  // Handling function code
  _APP_DEBUG_("WiFiStationDisconnected", event);
  _APP_DEBUG_("INIT", "WiFi.disconnect()");
}

//ESP 32
void networkStationInit()
{
  _APP_DEBUG_("INIT", "networkStationInit()");
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  _APP_DEBUG_("SSID ESP NOW", appPreferences.ssid_ap);
  _APP_DEBUG_("PASS", appPreferences.pass_ap);

  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

  WiFi.begin(appPreferences.ssid_ap.c_str(), appPreferences.pass_ap.c_str(), appPreferences.chan_ap.toInt());
  _APP_DEBUG_("INIT", "Connecting to Access Point");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  
}

/*
  Inicia la configuracion WEB  del dispositivo
*/
void networkAPInit()
{
  _APP_DEBUG_("NETWORK INIT", "WIFI MODE AP");
  // Set device as a Wi-Fi Ap to setup
  WiFi.mode(WIFI_AP);
  //Nombre de red, password, canal, visibilidad
  WiFi.softAP(WiFi.macAddress().c_str(), WIFI_PASSWORD_AP_DEFAULT, CHAN_AP_DEFAULT, false, 4);
  _APP_DEBUG_("AP IP", WiFi.softAPIP());
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
      inputParam = "SSID";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }

    if (request->hasParam("pwd", true))
    {
      setPreferenceByName("PASS_AP", request->getParam("pwd", true)->value());
      inputMessage = getPreferenceByName("PASS_AP", "");
      inputParam = "PWD";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }

    if (request->hasParam("mac", true))
    {
      setPreferenceByName("MAC_AP", request->getParam("mac", true)->value());
      inputMessage = getPreferenceByName("MAC_AP", "");
      inputParam = "MAC";
      _APP_DEBUG_("post", inputParam + " " + inputMessage);
    }

    if (request->hasParam("channel", true))
    {
      setPreferenceByName("CHAN_AP", request->getParam("channel", true)->value());
      inputMessage = getPreferenceByName("CHAN_AP", "");
      inputParam = "CHANNEL";
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