#ifndef TASK_MQTT
#define TASK_MQTT

///// cette task servira à faire les remontés MQTT pour à terme les retirer des autres fonctions et éviter certains bugs 
#include <Arduino.h>
#include <WiFiClientSecure.h>
   
#include "../config/config.h"
#include "../config/enums.h"
#include "../config/config.h"
#include "../config/enums.h"
// #include "mqtt-home-assistant.h"
#include "functions/Mqtt_http_Functions.h"
#include "functions/energyFunctions.h"

// #ifndef LIGHT_FIRMWARE
//   #include <PubSubClient.h>
//   extern PubSubClient client;
// #endif
#include "functions/ha.h"

extern DisplayValues gDisplayValues;
extern Config config;
extern Configmodule configmodule; 
extern Logs Logging;
extern Mqtt configmqtt;
extern System sysvar;



#ifndef LIGHT_FIRMWARE
      // extern HA device_routeur; 
      // extern HA device_grid; 
      // extern HA device_routed; // Ajout RV - 20230304
      // extern HA device_dimmer; // Ajout RV - 20230304
      // extern HA device_inject; 
      // extern HA compteur_inject;
      // extern HA compteur_grid;
      // extern HA temperature_HA;

      // extern HA power_factor;
      // extern HA power_vrms;
      // extern HA power_irms;
      // extern HA power_apparent;
      extern MQTT device_routeur; 
      extern MQTT device_grid; 
      extern MQTT device_routed; // Ajout RV - 20230304
      extern MQTT device_dimmer_power;
      extern MQTT device_dimmer; // Ajout RV - 20230304
      extern MQTT device_inject; 
      extern MQTT compteur_inject;
      extern MQTT compteur_grid;
      extern MQTT temperature;
      extern MQTT device_alarm_temp;
      extern MQTT compteur_route;
      
      #ifdef HARDWARE_MOD
            extern MQTT power_factor;
            extern MQTT power_vrms;
            extern MQTT power_irms;
            extern MQTT power_apparent;
      #endif
#endif

int Pow_mqtt_send = 0;
int Relays_mqtt_send = 0;
float WHtempgrid=0;
float WHtempinject=0;
float WHrouted=0;

long beforetime; 
#define timemilli 3.6e+6 
extern Memory task_mem; 

void send_to_mqtt(void * parameter){ // NOSONAR
  for (;;){
            if(!WiFi.isConnected()){   /// si pas de connexion Wifi test dans 10 s 
                vTaskDelay(10*1000 / portTICK_PERIOD_MS);
                continue;
            }

    /// vérification que l'envoie mqtt est souhaité et les connexions actives
    #ifndef LIGHT_FIRMWARE
    if (config.mqtt && (WiFi.status() == WL_CONNECTED ) && client.connected() ){
    #else
      if (config.mqtt && (WiFi.status() == WL_CONNECTED ) ){
    #endif
        long start = millis();
    
      #if WIFI_ACTIVE == true
                  Pow_mqtt_send ++ ;
                  Relays_mqtt_send ++ ;
                  if ( Relays_mqtt_send >= 20 ) { // Envoi apres 20 secondes, toutes les 20 secondes. 1er démarrage => récup valeur callback si existante, sinon false
                       switch_1.send(String(sysvar.relay1));
                       switch_2.send(String(sysvar.relay2)); 
                       Relays_mqtt_send = 0 ;
                  }
                  if ( Pow_mqtt_send >= 5 ) { //5 ou 2? 
                  long timemesure = start-beforetime;
                  float wattheure = (timemesure * abs(gDisplayValues.watt) / timemilli) ;  
                  float wattrouted = (timemesure * abs(gDisplayValues.puissance_route) / timemilli) ;  
            #ifndef LIGHT_FIRMWARE
                  WHrouted += wattrouted; 
                  compteur_route.send(String(WHrouted));

                  if (configmqtt.DOMOTICZ) {Mqtt_send_DOMOTICZ(String(config.IDX), String(int(gDisplayValues.watt)));  }
                  if ((configmqtt.HA) || ( configmqtt.JEEDOM)) {
                        device_routeur.send(String(int(gDisplayValues.watt)));
                        device_routed.send(String(gDisplayValues.puissance_route));
                        if (config.dimmerlocal) {
                              device_dimmer_power.send(String(int((unified_dimmer.get_power()) * config.charge/100)));
                        }

                        #ifdef HARDWARE_MOD
                              power_apparent.send(String(int(PVA)));
                              power_vrms.send(String(int(Vrms)));
                              power_irms.send(String(Irms));
                              power_factor.send(String(PowerFactor));
                        #endif
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
                              if (WHtempinject >= 0) {
                                    compteur_inject.send(String(WHtempinject));
                              }
                              else {
                                    WHtempinject = 0;
                              }
                              
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
                              if (WHtempgrid >= 0) {
                                    compteur_grid.send(String(WHtempgrid));
                              }
                              else {
                                    WHtempgrid = 0;
                              }

                        }
                  }
                        if (discovery_temp) {
                              if (configmqtt.DOMOTICZ) {Mqtt_send_DOMOTICZ(String(config.IDXdallas), String(gDisplayValues.temperature),"","Dallas" );} //  bug#11  remonté domoticz
                              if ((configmqtt.HA) || (configmqtt.JEEDOM)) {
                                    temperature.send(String(gDisplayValues.temperature));
                                    device_alarm_temp.send(stringBool(dallas.security));
                              }
                        }
            #endif
                  beforetime = start; 
                  Pow_mqtt_send = 0 ;
                  }                   
      #endif
      //    #if WIFI_ACTIVE == true
      //             Pow_mqtt_send ++ ;
      //             if ( Pow_mqtt_send > 5 ) {
      //             long timemesure = start-beforetime;
      //             float wattheure = (timemesure * abs(gDisplayValues.watt) / timemilli) ;  
      //       #ifndef LIGHT_FIRMWARE
      //        // domoticz et jeedom
      //             if (config.IDX != 0 ) {Mqtt_send(String(config.IDX), String(int(gDisplayValues.watt)),"","watt");  }
      //             if (config.IDXdallas != 0) {Mqtt_send(String(config.IDXdallas), String(gDisplayValues.temperature),"","Dallas" ); }//  bug#11  remonté domoticz 
      //       // HA
      //             if (configmqtt.HA) {
      //                   device_routeur.send(String(int(gDisplayValues.watt)));
      //                   power_apparent.send(String(int(PVA)));
      //                   power_vrms.send(String(int(Vrms)));
      //                   power_irms.send(String(Irms));
      //                   power_factor.send(String(PowerFactor));
      //                   enphase_cons_whLifetime.send(String(int(gDisplayValues.enp_cons_whLifetime)));
      //                   enphase_prod_whLifetime.send(String(int(gDisplayValues.enp_prod_whLifetime)));
      //                   enphase_current_power_consumption.send(String(int(gDisplayValues.enp_current_power_consumption)));
      //                   enphase_current_power_production.send(String(int(gDisplayValues.enp_current_power_production)));
      //                   temperature_HA.send(String(gDisplayValues.temperature));
      //             }

      //               /// remonté énergie domoticz et jeedom
      //             // send if injection
      //             if (gDisplayValues.watt < 0 ){
      //                   if (config.IDX != 0 && config.mqtt) {
      //                         Mqtt_send(String(config.IDX), String(int(-gDisplayValues.watt)),"injection","Reseau");
      //                         Mqtt_send(String(config.IDX), String("0") ,"grid","Reseau");
      //                   }
      //                   if (configmqtt.HA) {
      //                   device_inject.send(String(int(-gDisplayValues.watt)));
      //                   device_grid.send(String("0"));
      //                   WHtempgrid += wattheure; 
      //                   compteur_inject.send(String(WHtempgrid));
      //                   compteur_grid.send(String("0"));
      //                   }
      //             }
      //             else {
      //                   if (config.IDX != 0 && config.mqtt) {
      //                         Mqtt_send(String(config.IDX), String("0"),"injection","Reseau");
      //                         Mqtt_send(String(config.IDX), String(int(gDisplayValues.watt)),"grid","Reseau");
      //                   }
      //                   if (configmqtt.HA) {
      //                   device_grid.send(String(int(gDisplayValues.watt)));
      //                   device_inject.send(String("0"));
      //                   compteur_inject.send(String("0"));
      //                   WHtempinject += wattheure;
      //                   compteur_grid.send(String(WHtempinject));
      //                   }

      //             }
      //             //maj 202030209
                  
      //       #endif
      //             beforetime = start; 
      //             Pow_mqtt_send = 0 ;
      //             }
      // #endif   
      } 
      task_mem.task_send_mqtt = uxTaskGetStackHighWaterMark(NULL);
   // Sleep for 1 second
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}



#endif