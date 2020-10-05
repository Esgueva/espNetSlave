#include <Preferences.h>


Preferences preferences;


// WIFI credenciales.
//WIFI SETTINGS
//Wi-Fi channel (must match the gateway wi-fi channel as an access point)
//#define CHAN_AP 8
#define CHAN_AP_DEFAULT 0

// const char* WIFI_SSID_STATION = "MIWIFI_2G_RXMu";
// const char* WIFI_PASSWORD_STATION = "sAbubrRY";

//const char* WIFI_SSID_AP_DEFAULT = "espAP_default";
const char* WIFI_PASSWORD_AP_DEFAULT = "123456789";


//const char BROADCAST_MASTER[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; 
//const char BROADCAST_MASTER[] = { 0x30, 0xAE, 0xA4, 0xDD, 0x94, 0xC0 }; 



String getPreferenceByName(String name, String defaultValue){ 
  preferences.begin("config",false); //readOnly = false
  String pref = preferences.getString(name.c_str(), defaultValue);
  preferences.end();
  return pref;
}//getPreferenceByName


char getPreferenceBytes(String name){ 
    int schLen = preferences.getBytesLength(name.c_str());
    char buffer[schLen]; // prepare a buffer for the data
    preferences.getBytes(name.c_str(), buffer, schLen);
    return *buffer;

}//getPreferenceByName


void setPreferenceByName(String name, String value){
  preferences.begin("config",false); //readOnly = false
  preferences.putString(name.c_str(), value.c_str());
  preferences.end();
}//setPreferenceByName

void cleanPreference(String name){
  preferences.begin("config", false); //readOnly = false
  preferences.clear();
  preferences.end();
}//cleanPreference

void cleanWifiPreferences(){
  cleanPreference("SSID_AP");
  cleanPreference("PASS_AP");
  cleanPreference("BROADCAST");
}//cleanWifiPreferences