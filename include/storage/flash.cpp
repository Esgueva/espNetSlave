#include <SPIFFS.h>

/**
 * Mount the Flash
 */
void mountFlash()
{
  _APP_DEBUG_("FLASH", !SPIFFS.begin(true) ? "Error mounting" : "Mounted!");
}

/**
 * Show the Files 
 */
void openFiles()
{

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    _APP_DEBUG_VALUE_("FLASH", "File:", file.name());
    file = root.openNextFile();
  }
}

/**
 * Show the Memory State 
 */
void showMemory()
{
  _APP_DEBUG_VALUE_("FLASH", "Total Bytes: ", SPIFFS.totalBytes());
  _APP_DEBUG_VALUE_("FLASH", "Used Bytes: ", SPIFFS.usedBytes());
}

/**
 * Show the Memory State 
 */
void cleanApPreferences()
{
 
  cleanPreference("SSID_AP");
  cleanPreference("PASS_AP");
  cleanPreference("CHAN_AP");
  _APP_DEBUG_("---------------------------", "CLEAR");
} //cleanWifiPreferences
