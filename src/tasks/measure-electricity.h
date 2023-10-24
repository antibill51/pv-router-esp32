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

#ifndef LIGHT_FIRMWARE
      extern MQTT device_routeur; 
      extern MQTT device_grid; 
      extern MQTT device_inject; 
      extern MQTT compteur_inject;
      extern MQTT compteur_grid;
      extern MQTT temperature;
      extern MQTT device_alarm_temp;
      #ifdef HARDWARE_MOD
            extern MQTT power_factor;
            extern MQTT power_vrms;
            extern MQTT power_irms;
            extern MQTT power_apparent;
      #endif
      extern MQTT enphase_cons_whLifetime;
      extern MQTT enphase_prod_whLifetime;
      extern MQTT enphase_current_power_consumption;
      extern MQTT enphase_current_power_production;

#endif

int slowlog = TEMPOLOG - 1 ; 
long beforetime; 
#define timemilli 3.6e+6 
float WHtempgrid=0;
float WHtempinject=0;

int Pow_mqtt_send = 0;

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
                        if (slowlog == TEMPOLOG) {     logging.start  += loguptime(); logging.start +=  String("--> No sinus, check 12AC power \r\n"); slowlog =0 ; }

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
            if (strcmp(config.topic_Shelly,"") != 0)   { 
            // client.loop(); // on vérifie coté mqtt si nouvelle info
            gDisplayValues.watt = gDisplayValues.Shelly ;  // on met à jour
            gDisplayValues.porteuse = true; // et c'est bon. 
            }
      #endif
///enphase
      if (configmodule.enphase_present ) {
            Enphase_get();
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
///enphase
      if (configmodule.Fronius_present ){
            Fronius_get();
            }           


      #if WIFI_ACTIVE == true
                  Pow_mqtt_send ++ ;
                  if ( Pow_mqtt_send >= 5 ) {
                  long timemesure = start-beforetime;
                  float wattheure = (timemesure * abs(gDisplayValues.watt) / timemilli) ;  
            #ifndef LIGHT_FIRMWARE

                  if (configmqtt.DOMOTICZ) {Mqtt_send_DOMOTICZ(String(config.IDX), String(int(gDisplayValues.watt)));  }
                  if ((configmqtt.HA) || ( configmqtt.JEEDOM)) {
                        device_routeur.send(String(int(gDisplayValues.watt)));
                        #ifdef HARDWARE_MOD
                              power_apparent.send(String(int(PVA)));
                              power_vrms.send(String(int(Vrms)));
                              power_irms.send(String(Irms));
                              power_factor.send(String(PowerFactor));
                        #endif
                         if (configmodule.enphase_present ) {
                              enphase_cons_whLifetime.send(String(int(gDisplayValues.enp_cons_whLifetime)));
                              enphase_prod_whLifetime.send(String(int(gDisplayValues.enp_prod_whLifetime)));
                              enphase_current_power_consumption.send(String(int(gDisplayValues.enp_current_power_consumption)));
                              enphase_current_power_production.send(String(int(gDisplayValues.enp_current_power_production)));
                         }
                  }

                  // send if injection
                  if (gDisplayValues.watt < 0 ){
                        if (configmqtt.DOMOTICZ) {
                              Mqtt_send_DOMOTICZ(String(config.IDX), String(int(-gDisplayValues.watt)),"injection","Reseau");
                              Mqtt_send_DOMOTICZ(String(config.IDX), String("0") ,"grid","Reseau");
                        }
                        if ((configmqtt.HA) || ( configmqtt.JEEDOM)) {
                              device_inject.send(String(int(-gDisplayValues.watt)));
                              device_grid.send(String("0"));
                              WHtempinject += wattheure; 
                              compteur_inject.send(String(WHtempinject));
                        }
                  
                  
                  // if (configmqtt.HA)compteur_grid.send(String("0"));
                  }
                  else {
                        if (configmqtt.DOMOTICZ) {
                                          Mqtt_send_DOMOTICZ(String(config.IDX), String("0"),"injection","Reseau");
                                          Mqtt_send_DOMOTICZ(String(config.IDX), String(int(gDisplayValues.watt)),"grid","Reseau");
                        }
                  
                        if ((configmqtt.HA) || (configmqtt.JEEDOM)) {
                              device_grid.send(String(int(gDisplayValues.watt)));
                              device_inject.send(String("0"));
                              WHtempgrid += wattheure;
                              compteur_grid.send(String(WHtempgrid));
                        }
                  }
                        if (discovery_temp) {
                              if (configmqtt.DOMOTICZ) {Mqtt_send_DOMOTICZ(String(config.IDXdallas), String(gDisplayValues.temperature),"","Dallas" );} //  bug#11  remonté domoticz
                              if ((configmqtt.HA) || (configmqtt.JEEDOM)) {
                                    temperature.send(String(gDisplayValues.temperature));
                                    device_alarm_temp.send(stringboolMQTT(security));
                              }
                        }
            #endif
                  beforetime = start; 
                  Pow_mqtt_send = 0 ;
                  }                   
      #endif

}

long end = millis();

      // Schedule the task to run again in 1 second (while
      // taking into account how long measurement took) ///&& configmodule.pilote
      if (configmodule.enphase_present) {
            vTaskDelay(pdMS_TO_TICKS(4000));
      }
      else
      {      
            vTaskDelay(pdMS_TO_TICKS(2000));
      }

    }    
}

#endif
