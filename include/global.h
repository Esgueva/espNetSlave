//#define DEBUG false
//#define USE_SERIAL Serial

//Pines de Conexiones a los reles
const int relayCH1 =  32;
const int relayCH2 =  33;

#define DELETEBEFOREPAIR 0
#define PRINTSCANRESULTS 1

#define _APP_DEBUG_ON_  // Descomentar para habilitar la depuración por Serial.

// Macros para facilitar la salida de información por el Serial.
#if defined(_APP_DEBUG_ON_)
    #define _APP_DEBUG_(type, text) Serial.print(F("(")); Serial.print(millis()); Serial.print(F(" millis)")); Serial.print(F(" [")); Serial.print(type); Serial.print(F("] ")); Serial.println(text);
    #define _APP_DEBUG_VALUE_(type, text, value) Serial.print(F("(")); Serial.print(millis()); Serial.print(F(" millis)")); Serial.print(F(" [")); Serial.print(type); Serial.print(F("] "));Serial.print(text);Serial.print(F(" "));Serial.println(value);
#else
    #define _APP_DEBUG_(type, text) void();
    #define _APP_DEBUG_VALUE_(type, text, value) void();
#endif // De _APP_DEBUG_ON_


// Prototipos
void espnowInit();
void broadcastInit();
bool bme280Init();


bool sensorsBme280Active = false;
unsigned long previousMillis = 0;

