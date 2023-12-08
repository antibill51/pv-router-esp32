#ifndef MQTT_SENSORS
#define MQTT_SENSORS

#include <Arduino.h>
#ifndef LIGHT_FIRMWARE
// #include <PubSubClient.h>
#include <AsyncMqttClient.h>
#include "functions/ha.h"

extern Configmodule configmodule; 
extern Mqtt configmqtt; 

extern MQTT device_dimmer;
extern MQTT device_dimmer_routed_power;
extern MQTT device_routeur; 
extern MQTT device_grid;
extern MQTT device_inject;
extern MQTT compteur_inject;
extern MQTT compteur_grid;
extern MQTT switch_1;
extern MQTT switch_2;
extern MQTT temperature;
extern MQTT device_alarm_temp;
// extern MQTT surplus_routeur;
extern MQTT device_resistance;
extern MQTT compteur_route;


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

void init_MQTT_sensor(){
        device_dimmer.Set_object_id("dimmer");
        device_dimmer.Set_retain_flag(true);

        device_dimmer_routed_power.Set_object_id("dimmer_power");
        device_dimmer_routed_power.Set_retain_flag(true);

        device_routeur.Set_object_id("power");
        device_routeur.Set_retain_flag(true);

        device_grid.Set_object_id("grid");
        device_grid.Set_retain_flag(true);

        device_inject.Set_object_id("inject");
        device_inject.Set_retain_flag(true);

        compteur_grid.Set_object_id("grid_Wh");
        compteur_grid.Set_retain_flag(true);

        compteur_inject.Set_object_id("inject_Wh");
        compteur_inject.Set_retain_flag(true);

        compteur_route.Set_object_id("route_Wh");
        compteur_route.Set_retain_flag(true);

        // surplus_routeur.Set_object_id("surplus");
        // surplus_routeur.Set_retain_flag(true);

        temperature.Set_object_id("temperature");
        temperature.Set_retain_flag(true);

        switch_1.Set_object_id("relay1");
        switch_1.Set_retain_flag(true);

        switch_2.Set_object_id("relay2");
        switch_2.Set_retain_flag(true);

        device_alarm_temp.Set_object_id("alarm_temp");
        device_alarm_temp.Set_retain_flag(true);

        device_resistance.Set_object_id("resistance");
        device_resistance.Set_retain_flag(true);

        compteur_inject.send(String("0"));
        compteur_grid.send(String("0"));
        device_resistance.send(String(config.resistance));

        // switch_1.send(String(0));
        // switch_2.send(String(0));
        if (configmodule.enphase_present == true) {
                enphase_cons_whLifetime.Set_object_id("enphase_cons_wh");
                enphase_cons_whLifetime.Set_retain_flag(true);

                enphase_prod_whLifetime.Set_object_id("enphase_prod_wh");
                enphase_prod_whLifetime.Set_retain_flag(true);

                enphase_current_power_consumption.Set_object_id("enphase_current_cons");
                enphase_current_power_consumption.Set_retain_flag(true);

                enphase_current_power_production.Set_object_id("enphase_current_prod");
                enphase_current_power_production.Set_retain_flag(true);
        }

        #ifdef HARDWARE_MOD
                power_factor.Set_object_id("PowerFactor");
                power_factor.Set_retain_flag(true);
                power_vrms.Set_object_id("Vrms");
                power_vrms.Set_retain_flag(true);
                power_irms.Set_object_id("Irms");
                power_irms.Set_retain_flag(true);
                power_apparent.Set_object_id("ApparentPower");
                power_apparent.Set_retain_flag(true);
        #endif

}
void init_HA_sensor(){
        // init_MQTT_sensor();
        device_dimmer.Set_name("Consigne");
        // device_dimmer.Set_object_id("dimmer");
        device_dimmer.Set_unit_of_meas("%");
        device_dimmer.Set_stat_cla("measurement");
        device_dimmer.Set_dev_cla("power_factor"); // fix is using native unit of measurement '%' which is not a valid unit for the device class ('power') it is using
        device_dimmer.Set_icon("mdi:percent");
        device_dimmer.Set_entity_type("sensor");
        // device_dimmer.Set_retain_flag(true);
        // device_dimmer.Set_expire_after(true);
        
        device_dimmer_routed_power.Set_name("Puissance routée");
        // device_dimmer_routed_power.Set_object_id("dimmer_power");
        device_dimmer_routed_power.Set_unit_of_meas("W");
        device_dimmer_routed_power.Set_stat_cla("measurement");
        device_dimmer_routed_power.Set_dev_cla("power"); // fix is using native unit of measurement '%' which is not a valid unit for the device class ('power') it is using
        // device_dimmer_routed_power.Set_icon("mdi:percent");
        device_dimmer_routed_power.Set_entity_type("sensor");
        // device_dimmer_routed_power.Set_retain_flag(true);
        // device_dimmer_routed_power.Set_expire_after(true);

        // surplus_routeur.Set_name("Ecart zone neutre");
        // surplus_routeur.Set_unit_of_meas("W");
        // surplus_routeur.Set_stat_cla("measurement");
        // surplus_routeur.Set_dev_cla("power");
        // surplus_routeur.Set_entity_type("sensor");
        // // surplus_routeur.Set_retain_flag(true);
        // // surplus_routeur.Set_expire_after(true);


        device_routeur.Set_name("Puissance");
        // device_routeur.Set_object_id("power");
        device_routeur.Set_unit_of_meas("W");
        device_routeur.Set_stat_cla("measurement");
        device_routeur.Set_dev_cla("power");
        device_routeur.Set_entity_type("sensor");
        // device_routeur.Set_retain_flag(true);
        // device_routeur.Set_expire_after(true);

        device_grid.Set_name("Soutirage");
        // device_grid.Set_object_id("grid");
        device_grid.Set_unit_of_meas("W");
        device_grid.Set_stat_cla("measurement");
        device_grid.Set_dev_cla("power");
        device_grid.Set_entity_type("sensor");
        // device_grid.Set_retain_flag(true);
        // device_grid.Set_expire_after(true);


        device_inject.Set_name("Injection");
        // device_inject.Set_object_id("inject");
        device_inject.Set_unit_of_meas("W");
        device_inject.Set_stat_cla("measurement");
        device_inject.Set_dev_cla("power");
        device_inject.Set_entity_type("sensor");
        // device_inject.Set_retain_flag(true);
        // device_inject.Set_expire_after(true);

        compteur_grid.Set_name("Compteur soutirage");
        // compteur_grid.Set_object_id("grid_Wh");
        compteur_grid.Set_unit_of_meas("Wh");
        compteur_grid.Set_stat_cla("total_increasing");
        compteur_grid.Set_dev_cla("energy");
        compteur_grid.Set_entity_type("sensor");
        // compteur_grid.Set_retain_flag(true);
        // compteur_grid.Set_expire_after(true);

        compteur_inject.Set_name("Compteur injection");
        // compteur_inject.Set_object_id("inject_Wh");
        compteur_inject.Set_unit_of_meas("Wh");
        compteur_inject.Set_stat_cla("total_increasing");
        compteur_inject.Set_dev_cla("energy");      
        compteur_inject.Set_entity_type("sensor");
        // compteur_inject.Set_retain_flag(true);
        // compteur_inject.Set_expire_after(true);

        compteur_route.Set_name("Compteur routage");
        // compteur_route.Set_object_id("grid_Wh");
        compteur_route.Set_unit_of_meas("Wh");
        compteur_route.Set_stat_cla("total_increasing");
        compteur_route.Set_dev_cla("energy");
        compteur_route.Set_entity_type("sensor");
        // compteur_grid.Set_retain_flag(true);
        // compteur_grid.Set_expire_after(true);
        
        temperature.Set_name("Température");
        // temperature.Set_object_id("temperature");
        temperature.Set_dev_cla("temperature"); 
        temperature.Set_unit_of_meas("°C");
        temperature.Set_stat_cla("measurement");
        temperature.Set_entity_type("sensor");
        // temperature.Set_retain_flag(true);
        // temperature.Set_expire_after(true);

        switch_1.Set_name("Relais 1");
        // switch_1.Set_object_id("relay1");
        switch_1.Set_entity_type("switch");
        switch_1.Set_retain_flag(true);
        switch_1.Set_retain(true);
        // switch_1.Set_expire_after(true);


        switch_2.Set_name("Relais 2");
        // switch_2.Set_object_id("relay2");
        switch_2.Set_entity_type("switch");
        switch_2.Set_retain_flag(true);
        switch_2.Set_retain(true);
        // switch_2.Set_expire_after(true);

        device_alarm_temp.Set_name("Surchauffe");
        // device_alarm_temp.Set_object_id("alarm_temp");
        device_alarm_temp.Set_entity_type("binary_sensor");
        device_alarm_temp.Set_dev_cla("problem");
        device_alarm_temp.Set_retain_flag(true);
        // device_alarm_temp.Set_expire_after(true);


        device_resistance.Set_name("Résistance");
        device_resistance.Set_entity_type("number");
        device_resistance.Set_entity_category("config");
        device_resistance.Set_entity_valuemin("0");
        device_resistance.Set_entity_valuemax("5000"); 
        device_resistance.Set_entity_valuestep("100");
        device_resistance.Set_unit_of_meas("W");
        device_resistance.Set_retain(true);
        // device_resistance.Set_expire_after(true);
        #ifdef HARDWARE_MOD

                power_factor.Set_name("Facteur de puissance");
                // power_factor.Set_object_id("PowerFactor");
                power_factor.Set_unit_of_meas("%");
                power_factor.Set_stat_cla("measurement");
                power_factor.Set_dev_cla("power_factor");
                power_factor.Set_entity_type("sensor");
                power_factor.Set_icon("mdi:percent");
                // power_factor.Set_retain_flag(true);
                // power_factor.Set_expire_after(true);

                power_vrms.Set_name("Tension RMS");
                // power_vrms.Set_object_id("Vrms");
                power_vrms.Set_unit_of_meas("V");
                power_vrms.Set_stat_cla("measurement");
                power_vrms.Set_dev_cla("voltage");
                power_vrms.Set_entity_type("sensor");
                // power_vrms.Set_retain_flag(true);
                // power_vrms.Set_expire_after(true);

                power_irms.Set_name("Intensité RMS");
                // power_irms.Set_object_id("Irms");
                power_irms.Set_unit_of_meas("A");
                power_irms.Set_stat_cla("measurement");
                power_irms.Set_dev_cla("current");
                power_irms.Set_entity_type("sensor");
                // power_irms.Set_retain_flag(true);
                // power_irms.Set_expire_after(true);

                power_apparent.Set_name("Puissance apparente");
                // power_apparent.Set_object_id("ApparentPower");
                power_apparent.Set_unit_of_meas("VA");
                power_apparent.Set_stat_cla("measurement");
                power_apparent.Set_dev_cla("apparent_power");
                power_apparent.Set_entity_type("sensor");
                // power_apparent.Set_retain_flag(true);
                // power_apparent.Set_expire_after(true);
        #endif

        if (configmodule.enphase_present == true) {

                enphase_cons_whLifetime.Set_name("ENPHASE Consommation totale");
                // enphase_cons_whLifetime.Set_object_id("cons_enphase_cons_whLifetime");
                enphase_cons_whLifetime.Set_unit_of_meas("Wh");
                enphase_cons_whLifetime.Set_stat_cla("total_increasing");
                enphase_cons_whLifetime.Set_dev_cla("energy");      
                enphase_cons_whLifetime.Set_entity_type("sensor");
                // enphase_cons_whLifetime.Set_retain_flag(true);
                // enphase_cons_whLifetime.Set_expire_after(true);

                enphase_prod_whLifetime.Set_name("ENPHASE Production totale");
                // enphase_prod_whLifetime.Set_object_id("cons_enphase_prod_whLifetime");
                enphase_prod_whLifetime.Set_unit_of_meas("Wh");
                enphase_prod_whLifetime.Set_stat_cla("total_increasing");
                enphase_prod_whLifetime.Set_dev_cla("energy");      
                enphase_prod_whLifetime.Set_entity_type("sensor");
                // enphase_prod_whLifetime.Set_retain_flag(true);
                // enphase_prod_whLifetime.Set_expire_after(true);

                enphase_current_power_consumption.Set_name("ENPHASE Puissance consommation");
                // enphase_current_power_consumption.Set_object_id("enphase_current_power_consumtion");
                enphase_current_power_consumption.Set_unit_of_meas("W");
                enphase_current_power_consumption.Set_stat_cla("measurement");
                enphase_current_power_consumption.Set_dev_cla("power");
                enphase_current_power_consumption.Set_entity_type("sensor");
                // enphase_current_power_consumption.Set_retain_flag(true);
                // enphase_current_power_consumption.Set_expire_after(true);

                enphase_current_power_production.Set_name("ENPHASE Puissance production");
                // enphase_current_power_production.Set_object_id("enphase_current_power_production");
                enphase_current_power_production.Set_unit_of_meas("W");
                enphase_current_power_production.Set_stat_cla("measurement");
                enphase_current_power_production.Set_dev_cla("power");
                enphase_current_power_production.Set_entity_type("sensor");
                // enphase_current_power_production.Set_retain_flag(true);
                // enphase_current_power_production.Set_expire_after(true);
        }




        // client.setBufferSize(1024);
        if (configmqtt.HA){
        device_routeur.HA_discovery();
        device_dimmer.HA_discovery();
        device_dimmer_routed_power.HA_discovery();
        device_grid.HA_discovery();
        device_inject.HA_discovery();
        // surplus_routeur.HA_discovery();
        compteur_inject.HA_discovery();
        compteur_grid.HA_discovery();
        switch_1.HA_discovery(); 
        switch_2.HA_discovery(); 
        device_resistance.HA_discovery(); 
        compteur_route.HA_discovery(); 
        
        if (configmodule.enphase_present == true) {
                enphase_cons_whLifetime.HA_discovery();
                enphase_prod_whLifetime.HA_discovery();
                enphase_current_power_consumption.HA_discovery();
                enphase_current_power_production.HA_discovery();
        }


        }
        #ifdef HARDWARE_MOD
                if (configmqtt.HA){
                        power_factor.HA_discovery();
                        power_vrms.HA_discovery();
                        power_irms.HA_discovery();
                        power_apparent.HA_discovery();
                }
                
        #endif

}
String stringboolMQTT(bool mybool){
String truefalse = "true";
if (mybool == false ) {truefalse = "false";}
return String(truefalse);
}

#endif
#endif