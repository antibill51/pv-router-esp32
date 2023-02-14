#ifndef TASK_DALLAS
#define TASK_DALLAS

    #include <Arduino.h>
    #include "../config/config.h"
    #include "../config/enums.h"
    #include "../functions/dallasFunction.h"
    #include "mqtt-home-assistant.h"
   

extern DisplayValues gDisplayValues;
extern Dallas dallas ;
extern HA temperature_HA;

/**
 * Task: Modifier le dimmer en fonction de la production
 * 
 * récupère les informations, conso ou injection et fait varier le dimmer en conséquence
 * 
 */
void dallasread(void * parameter){
  for (;;){
    if (dallas.detect) {
    gDisplayValues.temperature = CheckTemperature("Inside : ", dallas.addr); 

   } 
   // Sleep for 5 seconds, avant de refaire une analyse
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
#endif