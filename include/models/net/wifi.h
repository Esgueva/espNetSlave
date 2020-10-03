class WifiPoint
{

public:
  String ssid_ap;
  String mac_ap;
  String pass_ap;
  String chan_ap;
  String deviceName;

  void readPreferences()
  {
    ssid_ap = getPreferenceByName("SSID_AP", "");
    mac_ap = getPreferenceByName("MAC_AP", "");
    pass_ap = getPreferenceByName("PASS_AP", "");
    chan_ap = getPreferenceByName("CHAN_AP", "");
    deviceName = getPreferenceByName("DEVICE_NAME", "");
  }

  void showPreferences()
  {
    _APP_DEBUG_VALUE_("AP PREFERENCES", "Ssid: ", ssid_ap);
    _APP_DEBUG_VALUE_("AP PREFERENCES", "MAC: ", mac_ap);
    _APP_DEBUG_VALUE_("AP PREFERENCES", "Pass: ", pass_ap);
    _APP_DEBUG_VALUE_("AP PREFERENCES", "Chan: ", ssid_ap);
    _APP_DEBUG_VALUE_("DEVICE PREFERENCES", "Device Name: ", deviceName);
    
    Serial.println("");
  }

  // void show()
  // {
  //   Serial.print(ssid_ap);
  //   Serial.print(" [");
  //   Serial.print(ssid_ap);
  //   Serial.print("]");
  //   Serial.print(" Ch ");
  //   Serial.print(ssid_ap);
  //   Serial.println("");
  // }
};
