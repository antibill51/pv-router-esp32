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
#include "functions/shelly.h" 

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
int demoloop = 0;

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
      

      //// recherche du mode de fonctionnement
      int mode = 0;   /// 0 = porteuse  ; 1 = shelly , 2 = enphase 3 = fronius  , 4 = demo 
      if ((strcmp(config.topic_Shelly,"none") != 0) && (strcmp(config.topic_Shelly,"") != 0)) {
            mode = 1; 
      }
      else if (configmodule.enphase_present && String(configmodule.envoy) == "S") {
            mode = 2; 
      }
      else if (configmodule.Fronius_present) {
            mode = 3; 
      }
      // logging.Set_log_init("Mode calcul energie : ",true);
      // logging.Set_log_init(String(mode));
      // logging.Set_log_init(" \r\n");

            /// SCT 013 
      if (mode == 0 ) { 
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

      if (mode == 4 ) {
                        //// mode demo

            gDisplayValues.porteuse = true;
            
            if (demoloop < TABLEAU_SIZE ) {
                  gDisplayValues.watt = tableaudemo[demoloop];
                  demoloop++;
            } 
            else {
                  demoloop = 0;
            }
      }

      /// que dans les cas sans mode AP
      if (!AP) {
            /// shelly
            if (mode == 1 ) { 
                  if (WiFi.status() == WL_CONNECTED )  {
                        /// on vérifie si config.topic_Shelly est une IP ou un topic mqtt
                        if (checkIP(config.topic_Shelly)) {
                              gDisplayValues.watt = shelly_get_data(config.topic_Shelly);
                        }
                        #ifdef NORMAL_FIRMWARE
                        else {
                              
                              // client.loop();
                              gDisplayValues.watt = gDisplayValues.Shelly ;
                            
                        }
                        #endif
                        //  // on met à jour
                        gDisplayValues.porteuse = true; // et c'est bon. 

                  }
            }

            /// enphase
            if (mode == 2 ) { 
                  if (WiFi.status() == WL_CONNECTED )  {
                        //// inversion des valeurs pour enphase piloteur
                        Enphase_get();
                        int tempo = gDisplayValues.watt; 
                        gDisplayValues.watt = gDisplayValues.Fronius_conso ; 
                        gDisplayValues.Fronius_conso = tempo; 
                  }

            }


            /// fronius
            if (mode == 3 ) { 
                  if (WiFi.status() == WL_CONNECTED )  {
                        Fronius_get();
                  }
            }
      }





// shelly quand c était en mqtt
      #ifdef NORMAL_FIRMWARE

      #endif



long end = millis();
      task_mem.task_measure_electricity = uxTaskGetStackHighWaterMark(NULL);
      // Schedule the task to run again in 1 second (while
      // taking into account how long measurement took) ///&& configmodule.pilote
      if (mode != 0 ) {
            vTaskDelay(pdMS_TO_TICKS(7000));
      }
      else
      {      
            vTaskDelay(pdMS_TO_TICKS(2000));
      }

    }    
}

#endif
