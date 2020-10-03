#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1024.7) //(1013.25)

Adafruit_BME280 bme; // use I2C interface

bool bme280Init()
{

  int intentos = 0;
  bool status = false;
  _APP_DEBUG_(F("Iniciando"), F("BME280"));

  do
  {
#if defined(_APP_DEBUG_ON_)
    Serial.print(F("."));
#endif
    status = bme.begin(0x76);
    intentos++;
    wait(100);
  } while (!status || intentos < 5);

  if (!status)
  {
    _APP_DEBUG_(F("No se pudo encontrar un sensor BME280 válido"), F("verifique el cableado!"));
    wait(2000);
  }
  else
  {
    _APP_DEBUG_(F("BME280"), F("init OK"));
  }

  return status;
}

/*
  Lee los datos del sensor y los covierte a formaton JSON
*/

String readbme280()
{

  StaticJsonDocument<250> doc;
  String json;
  doc["sensor"] = "BME280";
  doc["mac"] = WiFi.macAddress();
  doc["data"]["temp"] = roundf(bme.readTemperature() * 100) / 100;
  doc["data"]["hum"] = roundf(bme.readHumidity() * 100) / 100;
  doc["data"]["press"] = roundf((bme.readPressure() / 100.0F) * 100) / 100;
  serializeJson(doc, json);

  return json;

} //readbme280