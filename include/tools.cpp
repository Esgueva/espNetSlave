
// unsigned long waiting = 0;

/*
    Espera un tiempo indicado, alternativa a DELAY
*/
void wait(unsigned int t){
  unsigned long waiting = millis();
  while(millis() - waiting < t){}
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


// Switch precisa de numeros para hacer entrar case, convierte la entrada para ser recogida por su case!!

enum Functions
  {
    ERR_FUNCTION_NOT_EXIST = 0,
    CLEAN_WIFI_PREFERENCES = 1,
    RESTART = 2,
    GPIO_STATUS = 3
};

Functions convertFunction(const std::string& str)
{
    if(str == "CLEAN_WIFI_PREFERENCES") return CLEAN_WIFI_PREFERENCES;
    else if(str == "RESTART") return RESTART;
    else if(str == "GPIO_STATUS") return GPIO_STATUS;
    
    return ERR_FUNCTION_NOT_EXIST;
}

