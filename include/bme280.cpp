#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1024.7)  //(1013.25)

Adafruit_BME280 bme; // use I2C interface

void bme280Init(){

  int intentos = 0;
  bool status = false;
  Serial.println("Iniciando BME280");

  do {
    Serial.print('.');
    status = bme.begin(0x76);
    delay(100);
    intentos++;
  }while (!status || intentos < 5);

  if (!status) {
        Serial.println(F("No se pudo encontrar un sensor BME280 válido, verifique el cableado!"));
        delay(2000);
  }

}

String bme280Temperature(){
    return (String)bme.readTemperature();
}

String bme280readHumidity(){
    return (String)bme.readHumidity();
}

//struct_message_bme 

String readbme280(){

  StaticJsonDocument<250> doc;
  String json;
  doc["sensor"] = "BME280";
  doc["mac"] = WiFi.macAddress();
  doc["data"]["temp"] =  roundf(bme.readTemperature() * 100) / 100;
  doc["data"]["hum"] = roundf(bme.readHumidity() * 100) / 100; 
  doc["data"]["press"] =  roundf((bme.readPressure() / 100.0F) * 100) / 100;  
  serializeJson(doc, json);

  _APP_DEBUG_("BME JSON", json)

  return json;

}