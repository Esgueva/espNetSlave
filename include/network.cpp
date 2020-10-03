#if defined(ESP32)
// #include <ESPAsyncWebServer.h>
// #include <AsyncTCP.h>
// #include <WiFi.h>
#include <esp_now.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
extern "C"
{
#include <espnow.h>
}
#endif

//AsyncWebServer server(80);

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress_default[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//uint8_t broadcastAddress_default[] = { 0x30, 0xAE, 0xA4, 0xDD, 0x94, 0xC0 }; //Original
//uint8_t broadcastAddress[] = { 0xFC, 0xF5, 0xC4, 0x00, 0x31, 0xF5 }; //RODRI

esp_now_peer_info_t peerInfo;

// Prototype
void espnowSend(String json);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

// // Variable to store if sending data was successful
//String success;

bool msgRequest = false;
// si pasado este tiempo no recive respuesta reinica el dispositivo
unsigned long previousMillismsgRequestTimeOut = 0;
unsigned long espNowTimeOut = 0;
unsigned int msgTimeOut = 3000;

void managementIncomigData(const String &incomingData)
{
  StaticJsonDocument<250> doc;
  DeserializationError err = deserializeJson(doc, incomingData);
  _APP_DEBUG_(F("managementIncomigData Deserialization"), err.c_str());

  if (String(incomingData).indexOf("gpio") != -1)
  {
    int gpio = doc["gpio"];
    bool value = doc["value"];

    _APP_DEBUG_VALUE_(F("gpio"), gpio, value);

    value == 1 ? digitalWrite(gpio, HIGH) : digitalWrite(gpio, LOW);
  }
  else if (String(incomingData).indexOf("function") != -1)
  {
    String function = doc["function"];
    Functions functionToCall = convertFunction(function.c_str());

    switch (functionToCall)
    {

      case ERR_FUNCTION_NOT_EXIST:
        _APP_DEBUG_(F("ERR_FUNCTION_NOT_EXIST"), "");
        break;
      case CLEAN_WIFI_PREFERENCES:
        cleanWifiPreferences();
        break;
      case RESTART:
        ESP.restart();
        break;
      case GPIO_STATUS:
        _APP_DEBUG_(F("FUCTION GPIO_STATUS"), F("READ CHANNELS"));
        StaticJsonDocument<250> gpio;
        String json;
        String aux = String(digitalRead(relayCH1)) + " " + String(digitalRead(relayCH2));
        gpio["DEVICE_NAME"] = device.deviceName;
        gpio["GPIO_STATUS"] = aux;
        serializeJson(gpio, json);
        _APP_DEBUG_(F("GPIO_STATUS"), json);
        espnowSend(json);
        break;
      // default:
      //         break;
    }
  }
  else if (String(incomingData).indexOf("msgRecv") != -1)
  {
    // Comprueba si el mensaje fue entrgado
    msgRequest = false;
  }
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  _APP_DEBUG_(F("Last Packet Send Status:\t"), status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int incomingData_len)
{

  _APP_DEBUG_(F("Bytes received: "), incomingData_len);

  char buffer[incomingData_len];
  memcpy(&buffer, incomingData, incomingData_len);

  _APP_DEBUG_(F("Incoming Data: "), buffer);

  managementIncomigData(buffer);
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
// void parseBytes(const char *str, char sep, uint8_t *bytes, int maxBytes, int base)
// {
//   for (int i = 0; i < maxBytes; i++)
//   {
//     bytes[i] = strtoul(str, NULL, base); // Convert byte
//     str = strchr(str, sep);              // Find next separator
//     if (str == NULL || *str == '\0')
//     {
//       break; // No more separators, exit
//     }
//     str++; // Point to next character after separator
//   }
// }

void broadcastInit()
{
  _APP_DEBUG_(F("Esp NOW"), F("broadcastInit()"));
  _APP_DEBUG_(F("MAC"), device.mac_ap);
  _APP_DEBUG_(F("CHAN"), device.chan_ap);

  //const char* macStr =  "24:62:AB:F3:08:D4";
  const char *macStr = device.mac_ap.c_str();
  //byte mac[6];

  uint8_t mac[6];

  parseBytes(macStr, ':', mac, 6, 16);

  memcpy(&peerInfo.peer_addr, &mac, 6);

  peerInfo.channel = device.chan_ap.toInt();
  peerInfo.encrypt = false;
  peerInfo.ifidx = ESP_IF_WIFI_STA; //ESP_IF_WIFI_AP

  // Add peer
  if (esp_now_add_peer(&peerInfo) == ESP_OK)
  {
    _APP_DEBUG_(F("Added Master Node"), ESP_OK == 0 ? "OK" : "nok");
  }
  else
  {
    _APP_DEBUG_(F("Master Node could not be added..."),"");
    bool exists = esp_now_is_peer_exist(mac);

    if (exists)
    {
      _APP_DEBUG_(F("Master Node Exist"),"");
    }
  }
}

void espnowSend(String msg)
{

  esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t *)msg.c_str(), msg.length() + 1);

  if (result == ESP_OK)
  {
    _APP_DEBUG_(F("espnowSend"), F("Sent with success"));
    espNowTimeOut = millis();
    msgRequest = true; //Esta pendiente de recibir confirmacion, si no recibe 1)No esta autorizado 2)No fue autorizado, y necesita validar
    _APP_DEBUG_(F("espnowSend"), F("Waiting response"));
  }
  else
  {
    _APP_DEBUG_(F("espnowSend"), F("Error sending the data"));
  }
}