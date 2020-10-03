/*
 platformio run --target uploadfs

  INFO :: Gpio12 es un pin de arranque, puede establecer el voltaje de flash (incorrecto) y habilitar el ldo interno. NO UTILIZAR

*/
#include <Arduino.h>
#include <global.h>
#include <tools.cpp>

#include <ArduinoJson.h>
// #include <math.h>

// #include <iostream>
// #include <exception>

#include <models/preferences.cpp>
#include <storage/flash.cpp>

#include <net/tcp_ip.cpp>
#include <network.cpp>

#include <sensors/bme280.cpp>


void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);


  //Define las salidas GPIO
  pinMode(relayCH1, OUTPUT);
  pinMode(relayCH2, OUTPUT);


  // FILES
	mountFlash();
  openFiles();
  showMemory();

  wait(10);

  //cleanWifiPreferences();

  device.readPreferences();
  // setPreferenceByName("DEVICE_NAME", "Lab 1");

#ifdef _APP_DEBUG_ON_

  device.showPreferences();

#endif
  
  initComunications();

  wait(1000);

}

void loop()
{

  if (msgRequest &&  millis() - espNowTimeOut > msgTimeOut){
    _APP_DEBUG_("[Loop] >> Reiniciar dispositivo, validar dispositivo", msgRequest);
    msgRequest = false;
    networkStationInit();
    espnowInit();
    broadcastInit();
  }


  //Actualiza los valores del sensor temp,hum,pres

  if (!msgRequest && sensorsBme280Active &&  millis() - previousMillis > 25000)
  {
    previousMillis = millis();
    #if defined(_APP_DEBUG_ON_)
        String msg = readbme280();
        _APP_DEBUG_(F("SEND"), msg);
    #endif 
    espnowSend(readbme280());
  }
}