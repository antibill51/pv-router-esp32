#ifndef TASK_DIMMER
#define TASK_DIMMER

    #include <Arduino.h>
    #include "../config/config.h"
    #include "../config/enums.h"
    #include "../functions/dimmerFunction.h"



extern DisplayValues gDisplayValues;

/**
 * Task: Modifier le dimmer en fonction de la production
 * 
 * récupère les informations, conso ou injection et fait varier le dimmer en conséquence
 * 
 */
void updateDimmer(void * parameter){
  for (;;){
  gDisplayValues.task = true;
#if WIFI_ACTIVE == true
    dimmer();
    
    #if CLEAN
    /*
    /// si changement à faire
    if  (gDisplayValues.change != 0 ) {
        Serial.println(F("changement des valeurs dimmer-MQTT"));
        // envoie de l'information au dimmer et au serveur MQTT ( mosquito ou autre )
        dimmer_change(); 
    }*/ 
    #endif
   
#endif
    gDisplayValues.task = false;
   // Sleep for 5 seconds, avant de refaire une analyse
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    // 24/01/2023 changement de 5 à 4s 
  }
}
#endif