#ifndef TASK_MEASURE_ELECTRICITY
#define TASK_MEASURE_ELECTRICITY

#include <Arduino.h>

#include "../config/config.h"
#include "../config/enums.h"
// #include "mqtt-home-assistant.h"
#include "functions/energyFunctions.h"
#include "functions/dimmerFunction.h"
#include "functions/drawFunctions.h"
#include "functions/enphaseFunction.h"
#include "functions/froniusFunction.h"

#include "functions/ha.h"

extern DisplayValues gDisplayValues;
extern Configmodule configmodule; 
extern Logs Logging;

// #ifndef LIGHT_FIRMWARE
//       extern MQTT device_routeur; 
//       extern MQTT device_grid; 
//       extern MQTT device_inject; 
//       extern MQTT compteur_inject;
//       extern MQTT compteur_grid;
//       extern MQTT temperature;
//       extern MQTT device_alarm_temp;
//       extern MQTT compteur_route;
      
//       #ifdef HARDWARE_MOD
//             extern MQTT power_factor;
//             extern MQTT power_vrms;
//             extern MQTT power_irms;
//             extern MQTT power_apparent;
//       #endif
//       extern MQTT enphase_cons_whLifetime;
//       extern MQTT enphase_prod_whLifetime;
//       extern MQTT enphase_current_power_consumption;
//       extern MQTT enphase_current_power_production;

// #endif

int slowlog = TEMPOLOG - 1 ; 
// long beforetime; 
// #define timemilli 3.6e+6 
// float WHtempgrid=0;
// float WHtempinject=0;
// float WHrouted=0;

// int Pow_mqtt_send = 0;
extern Memory task_mem; 

void measureElectricity(void * parameter)
{
    for(;;){
    //  serial_println("[ENERGY] Measuring...");
       /// vérification qu'une autre task ne va pas fausser les valeurs
      long start = millis();
      int porteuse; 
      /*if ( configmodule.enphase_present || configmodule.Fronius_present || strcmp(config.topic_Shelly,"") != 0 ) {
            porteuse = false; || (String(configmodule.envoy) == "R")
      }*/ /// refaire des tests... 
      
      if ( configmodule.enphase_present == false && configmodule.Fronius_present == false ) {  ///correction Fred 230423--> marche pas 
            if (strcmp(config.topic_Shelly,"") == 0 ) {
                  #ifndef HARDWARE_MOD
                  injection2();
                  #else
                  injection3();
                  #endif
            if ( gDisplayValues.porteuse == false  && configmodule.enphase_present == false && configmodule.Fronius_present == false) {
                        gDisplayValues.watt =0 ; 
                        slowlog ++; 
                        if (slowlog == TEMPOLOG) {     
                              logging.Set_log_init("--> No sinus, check 12AC power \r\n");
                              slowlog =0 ; 
                        }

                  }
                  if (logging.serial){
                  serial_println(int(gDisplayValues.watt)) ;
                
                  }
                    
            }
      }
      else{
            gDisplayValues.porteuse = true;

      }
     




if (!AP) {

// shelly 
      #ifdef NORMAL_FIRMWARE
         if (client.connected() && (WiFi.status() == WL_CONNECTED ))  {
            if (strcmp(config.topic_Shelly,"") != 0)   { 
            // client.loop(); // on vérifie coté mqtt si nouvelle info
            gDisplayValues.watt = gDisplayValues.Shelly ;  // on met à jour
            gDisplayValues.porteuse = true; // et c'est bon. 
            }
         }
      #endif
///enphase
      if (configmodule.enphase_present ) {
            if (WiFi.status() == WL_CONNECTED )  {
                  Enphase_get();
            }
            //if ( configmodule.pilote ) { 
                  //// inversion des valeurs pour enphase piloteur
                  if (String(configmodule.envoy) == "S") {
                  int tempo = gDisplayValues.watt; 
                  gDisplayValues.watt = gDisplayValues.Fronius_conso ; 
                  gDisplayValues.Fronius_conso = tempo; }
                  else 
                  {  /// si c'est un modèle R, il ne fait pas les mesures. 
                  #ifndef HARDWARE_MOD
                  injection2();
                  #else
                  injection3();
                  #endif
                  }

              //    }
      }
///fronius
      if (configmodule.Fronius_present ){
            if (WiFi.status() == WL_CONNECTED )  {
                  Fronius_get();
            }
      }           



}

long end = millis();
      task_mem.task_measure_electricity = uxTaskGetStackHighWaterMark(NULL);
      // Schedule the task to run again in 1 second (while
      // taking into account how long measurement took) ///&& configmodule.pilote
      if (configmodule.enphase_present) {
            vTaskDelay(pdMS_TO_TICKS(5000));
      }
      else
      {      
            vTaskDelay(pdMS_TO_TICKS(3000));
      }

    }    
}

#endif
