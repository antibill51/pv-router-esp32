#ifndef MQTT_FUNCTIONS
#define MQTT_FUNCTIONS

#include <Arduino.h>
#include "../config/config.h"
#include "../config/enums.h"
#include "functions/spiffsFunctions.h"
#include <WiFi.h>
#include "HTTPClient.h"
#include "functions/ha.h"

WiFiClient espClient;
#ifndef LIGHT_FIRMWARE
extern uint32_t lastDisconnect;
extern System sysvar;
  // PubSubClient client(espClient);
  // AsyncMqttClient client;
  espMqttClientAsync  client;

void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len);
void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);
void callback(const espMqttClientTypes::MessageProperties& properties, const char* Subscribedtopic, const uint8_t* payload, size_t len, size_t index, size_t total);
#endif
extern Config config;
extern DisplayValues gDisplayValues;
extern Mqtt configmqtt;
extern Logs logging;
extern Dallas dallas;

#ifndef LIGHT_FIRMWARE
const String node_mac = WiFi.macAddress().substring(12,14)+ WiFi.macAddress().substring(15,17);
const String node_id = String("PvRouter-") + node_mac; 
const String topic_Xlyric = "Xlyric/"+ node_id +"/";
const String command_switch = String(topic_Xlyric + "command/switch");
const String command_number = String(topic_Xlyric + "command/number");
const String HA_status = String("homeassistant/status");



char arrayWill[64];// NOSONAR
void async_mqtt_init() {
  String topic_Xlyric = "Xlyric/" + String(node_id) +"/";
	const String LASTWILL_TOPIC = topic_Xlyric + "status";
	LASTWILL_TOPIC.toCharArray(arrayWill, 64);
  IPAddress ip;
  ip.fromString(config.mqttserver);
  DEBUG_PRINTLN(ip);
  // node_id = String("Pv-") + WiFi.macAddress().substring(12,14)+ WiFi.macAddress().substring(15,17); 
  client.setClientId(node_id.c_str());
  client.setKeepAlive(30);
  client.setWill(arrayWill, 2, true, "offline");
  client.setCredentials(configmqtt.username, configmqtt.password);
  client.onDisconnect(onMqttDisconnect);
  client.onSubscribe(onMqttSubscribe);
  client.onMessage(callback);
  client.setServer(ip, config.mqttport);
  // client.setMaxTopicLength(768); // 1024 -> 768 
  client.onConnect(onMqttConnect);
  logging.Set_log_init("MQTT topic for dimmer(s) : ",true);
  logging.Set_log_init(topic_Xlyric.c_str());
  logging.Set_log_init("sensors/dimmer/state\r\n");
  }

void connectToMqtt() {
  if (!client.connected() ) {
    DEBUG_PRINTLN("Connecting to MQTT...");
    logging.Set_log_init("Connecting to MQTT... \r\n",true);
    client.connect();
  }
  
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  logging.Set_log_init("Connected to MQTT.\r\n",true);
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  client.publish(String(topic_Xlyric +"status").c_str(),1,true, "online");         // Once connected, publish online to the availability topic
  client.subscribe((command_number + "/#").c_str(),1);
  client.subscribe((command_switch + "/#").c_str(),1);
  client.subscribe((HA_status).c_str(),1);
  Serial.println((command_number + "/#").c_str());
  Serial.println((command_switch + "/#").c_str());
  logging.Set_log_init("MQTT callback started \r\n",true);
 // mqttConnected = true;
}
void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason)
{    
    lastDisconnect = millis();
    Serial.println("Disconnected from MQTT.");
    logging.Set_log_init("MQTT disconnected \r\n",true);

    logging.Set_log_init("Disconnect reason:",true);
    switch (reason) {
    case espMqttClientTypes::DisconnectReason::TCP_DISCONNECTED:
        logging.Set_log_init("TCP_DISCONNECTED",true);
        logging.Set_log_init("\r\n");
        break;
    case espMqttClientTypes::DisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
        logging.Set_log_init("MQTT_UNACCEPTABLE_PROTOCOL_VERSION",true);
        logging.Set_log_init("\r\n");
        break;
    case espMqttClientTypes::DisconnectReason::MQTT_IDENTIFIER_REJECTED:
        logging.Set_log_init("MQTT_IDENTIFIER_REJECTED",true);
        logging.Set_log_init("\r\n");
        break;
    case espMqttClientTypes::DisconnectReason::MQTT_SERVER_UNAVAILABLE:
        logging.Set_log_init("MQTT_SERVER_UNAVAILABLE",true);
        logging.Set_log_init("\r\n");
        break;
    case espMqttClientTypes::DisconnectReason::MQTT_MALFORMED_CREDENTIALS:
        logging.Set_log_init("MQTT_MALFORMED_CREDENTIALS",true);
        logging.Set_log_init("\r\n");
        break;
    case espMqttClientTypes::DisconnectReason::MQTT_NOT_AUTHORIZED:
        logging.Set_log_init("MQTT_NOT_AUTHORIZED",true);
        logging.Set_log_init("\r\n");
        break;
    default:
        logging.Set_log_init("Unknown \r\n",true);
    }
}

void onMqttSubscribe(uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  for (size_t i = 0; i < len; ++i) {
    Serial.print("  qos: ");
    Serial.println(static_cast<uint8_t>(codes[i]));
  }
}


/*
*    Fonction d'envoie info MQTT
*/

void Mqtt_send_DOMOTICZ ( String idx, String value, String otherpub = "" , String name = "") {
  if (configmqtt.DOMOTICZ || configmqtt.JEEDOM ) {
    if (idx != "0" || idx != "" ) { /// si IDX = 0 ou vide on ne fait rien

      String nvalue = "0" ; 
      
      if ( value != "0" ) { 
          nvalue = "2" ; 
          }
      
      String message; 
      if (otherpub == "" ) {
        message = R"( { "idx" : )" + idx + R"( , "svalue" : ")" + value + R"(",  "nvalue" : )" + nvalue + R"( } )";
      }

    String jdompub = String(config.Publish) + "/"+idx ;
    if (otherpub != "" ) {jdompub += "/"+otherpub; }
    


      if (client.connected() && (WiFi.status() == WL_CONNECTED ))  {
      // client.loop();
        if (otherpub == "" ) {
          client.publish(config.Publish, 1, true, message.c_str());
        }
        client.publish(jdompub.c_str(),1 ,true, value.c_str());
      }
    }
  }
}

/*
Fonction MQTT callback
*
*/


// void callback(char* Subscribedtopic, byte* message, unsigned int length) {
// void callback(char* Subscribedtopic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
void callback(const espMqttClientTypes::MessageProperties& properties, const char* Subscribedtopic, const uint8_t* payload, size_t len, size_t index, size_t total) {
    char message[len + 1];
    memcpy(message, payload, len);
    message[len] = '\0';

  String fixedpayload = ((String)message).substring(0,len);
  // logging.Set_log_init("Subscribedtopic : " + String(Subscribedtopic)+ "\r\n");
  // logging.Set_log_init("Payload : " + String(fixedpayload)+ "\r\n");
  JsonDocument doc2;
  deserializeJson(doc2, message);
  if (strstr( Subscribedtopic, command_switch.c_str() ) != NULL) { 
    if (doc2.containsKey("relay1")) { 
        int relay = doc2["relay1"]; 
        if ( relay == 0) { digitalWrite(RELAY1 , LOW); sysvar.relay1 = false; }
        else { digitalWrite(RELAY1 , HIGH); sysvar.relay1 = true; } 
        switch_1.send(String(relay));
    }
    if (doc2.containsKey("relay2")) { 
        int relay = doc2["relay2"]; 
        if ( relay == 0) { digitalWrite(RELAY2 , LOW);sysvar.relay2 = false; }
        else { digitalWrite(RELAY2 , HIGH);sysvar.relay2 = true; } 
        switch_2.send(String(relay));
    }
  }
  // Je n'ai pas trouvé d'infos sur le shelly em pour valider ceci : 

  
    if (strcmp( Subscribedtopic, config.topic_Shelly ) == 0 ) {
        if (strcmp( message , "unavailable" ) == 0 ) { 
            gDisplayValues.Shelly = -2; 
        }
        else { 
         DEBUG_PRINTLN("MQTT callback : Shelly = "+String(message));
      // Utiliser strtol pour une conversion plus robuste
          char* endPtr;
          double shellyValue = strtod(message, &endPtr);

          if (endPtr != message && *endPtr == '\0') {
            // La conversion s'est déroulée avec succès
            gDisplayValues.Shelly = shellyValue;
          } else {
            DEBUG_PRINTLN("Erreur : Conversion de la chaîne en virgule flottante a échoué");
          } 
        } 
      }


  // if (strcmp(Subscribedtopic, config.topic_Shelly ) == 0 ) {
  //     if (strcmp( doc2["state"] , "unavailable" ) == 0 ) { gDisplayValues.Shelly = -2; }
  //     else { gDisplayValues.Shelly = doc2["state"];  }
  // }
  if (strstr( Subscribedtopic, command_number.c_str() ) != NULL) { 
    if (doc2.containsKey("charge1")) { 
      int charge1 = doc2["charge1"]; 
      if (config.charge1 != charge1 ) {
        config.charge1 = charge1;
        device_charge1.send(String(charge1));
      }
    }
    if (doc2.containsKey("charge2")) { 
      int charge2 = doc2["charge2"]; 
      if (config.charge2 != charge2 ) {
        config.charge2 = charge2;
        device_charge2.send(String(charge2));
      }
    }
    if (doc2.containsKey("charge3")) { 
      int charge3 = doc2["charge3"]; 
      if (config.charge3 != charge3 ) {
        config.charge3 = charge3;
        device_charge3.send(String(charge3));
      }
    }
  }
  
  if (strcmp( Subscribedtopic, HA_status.c_str() ) == 0) { 
    logging.Set_log_init("MQTT HA_status ",true);
    logging.Set_log_init(fixedpayload);
    logging.Set_log_init("\r\n");
    if (strcmp( fixedpayload.c_str(), "online" ) == 0) {  
      client.disconnect();
      logging.Set_log_init("MQTT Disconnection to resend HA discovery \r\n",true);
    }
  }
}



/*
*    Fonction d'init de MQTT 
*/

void Mqtt_init() {

  // comparaison de config.mqttserver avec none 
    if ((strcmp(config.mqttserver,"none") == 0) || (strcmp(config.mqttserver,"") == 0) || !(config.mqtt)) {
    Serial.println("MQTT_init : MQTT désactivé");
    return;
  }

  Serial.println("MQTT_init : server="+String(config.mqttserver));
  Serial.println("MQTT_init : port="+String(config.mqttport));
  
  // client.setServer(config.mqttserver, config.mqttport);
  // client.setCallback(callback);
  Serial.println("MQTT_init : connexion...");
  async_mqtt_init();
  // connectToMqtt();
  // reconnect();
  // // récupération des topics des anciennes valeurs 
  // Serial.println("récupération des anciennes valeurs de consommation...");
  // Serial.println(("memory/"+compteur_grid.topic+"#").c_str());
  // Serial.println("memory/"+compteur_grid.topic+compteur_grid.Get_name()) ;

  // client.subscribe(("memory/"+compteur_grid.topic+"#").c_str());
  // client.loop();
  //   if (configmqtt.DOMOTICZ){ Mqtt_send_DOMOTICZ(String(config.IDXdimmer),"0","","Dimmer"); }
    // if (strcmp(config.topic_Shelly,"") != 0) client.subscribe(config.topic_Shelly);


}


#endif

#endif