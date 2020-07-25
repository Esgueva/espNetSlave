//#define DEBUG false
//#define USE_SERIAL Serial

#define DELETEBEFOREPAIR 0
#define PRINTSCANRESULTS 1

#define _APP_DEBUG_ON_  // Descomentar para habilitar la depuración por Serial.

// Macros para facilitar la salida de información por el Serial.
#if defined(_APP_DEBUG_ON_)
    #define _APP_DEBUG_(type, text) Serial.print("("); Serial.print(millis()); Serial.print(" millis)"); Serial.print(" ["); Serial.print(type); Serial.print("] "); Serial.println(text);
    #define _APP_DEBUG_VALUE_(type, text, value) Serial.print("("); Serial.print(millis()); Serial.print(" millis)"); Serial.print(" ["); Serial.print(type); Serial.print("] "); Serial.print(text); Serial.println(value);
#else
    #define _APP_DEBUG_(type, text) void();
    #define _APP_DEBUG_VALUE_(type, text, value) void();
#endif // De _APP_DEBUG_ON_

typedef struct AppPreferences{
    String ssid_ap;
    String pass_ap;
    String mac_ap;
    String chan_ap;
} AppPreferences;

AppPreferences appPreferences;


//Structure example to send data BME280
//Must match the receiver structure
// typedef struct struct_message {
//     float temp;
//     float hum;
//     float pres;
//     byte macAddress[6];   //Identificador dispositivo
// } struct_message;

// // Create a struct_message called BME280Readings to hold sensor readings
// struct_message BME280Readings;

// // Create a struct_message to hold incoming sensor readings
// struct_message incomingReadings;




// MQTT credenciales. Seran declaradas en otro lugar de codigo
// extern const char* MQTT_SERVER; 
// extern const int  MQTT_SERVERPORT;
// extern const char* MQTT_USERNAME;   
// extern const char* MQTT_PASSWORD;   


//Utilizado para el envio de mensajes
// extern char buffer[20]; 

//Metodos accesibles
// String readDHTTemperature(); 
// String readDHTHumidity();
// IPAddress getIp();

// String bme280Temperature();
// String bme280readHumidity();

//struct_message_bme readbme280();

