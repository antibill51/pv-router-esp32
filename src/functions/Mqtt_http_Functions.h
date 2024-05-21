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
// String node_ids = WiFi.macAddress().substring(0,2)+ WiFi.macAddress().substring(4,6)+ WiFi.macAddress().substring(8,10) + WiFi.macAddress().substring(12,14)+ WiFi.macAddress().substring(15,17);
const String node_id = String("PvRouter-") + node_mac; 
const String topic_Xlyric = "Xlyric/"+ node_id +"/";
const String command_switch = String(topic_Xlyric + "command/switch");
const String command_number = String(topic_Xlyric + "command/number");
// const String command_select = String(topic_Xlyric + "command/select");
// const String command_button = String(topic_Xlyric + "command/button");
const String HA_status = String("homeassistant/status");


// void reconnect();
/***
 *  reconnexion au serveur MQTT
 */

//     void reconnect() {
      
//         if ((strcmp(config.mqttserver,"none") == 0) || (strcmp(config.mqttserver,"") == 0) || !(config.mqtt)) {
//         Serial.println("MQTT_init : MQTT désactivé");
//         return;
//         }

//       // const String pvname = String("PvRouter-") + WiFi.macAddress().substring(12,14)+ WiFi.macAddress().substring(15,17); 
//       // const String topic = "homeassistant/sensor/"+ pvname +"/status";
//       // Loop until we're reconnected
//       // while (!client.connected()) {
//         logging.clean_log_init();
//         Serial.println("-----------------------------");
//         Serial.println("Attempting MQTT reconnection...");
        
//         logging.Set_log_init("MQTT attempting reco \r\n",true);
//         //affichage du RSSI
//         logging.Set_log_init(String(WiFi.RSSI())+" dBm\r\n");

        // // Attempt to connect

        // if (client.connect(pvname.c_str(), configmqtt.username, configmqtt.password)) {
        //   client.publish(topic.c_str(), "online", true);         // Once connected, publish online to the availability topic
        //   client.setKeepAlive(30);

          
        //   logging.Set_log_init("MQTT : Reconnected\r\n",true);
        //   Serial.println("MQTT connected");
        // } else {
        //   Serial.print("MQTT failed, retcode="); 
        //   Serial.print(client.state());
        //   Serial.println(" try again in 2 seconds");
        //   ///dans le doute si le mode AP est actif on le coupe
        //   Serial.println(WiFi.status());

//       // Wait 2 seconds before retrying
//       // delay(2000);  // 24/01/2023 passage de 5 à 2s 
//     // }
//   // }
// }


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
  // topic_Xlyric = "Xlyric/" + String(node_id) +"/";
  // command_switch = String(topic_Xlyric + "command/switch");
  // command_number = String(topic_Xlyric + "command/number");
  // command_select = String(topic_Xlyric + "command/select");
  // command_button = String(topic_Xlyric + "command/button");
  // command_save = String("Xlyric/sauvegarde/"+ node_id );


  client.publish(String(topic_Xlyric +"status").c_str(),1,true, "online");         // Once connected, publish online to the availability topic
  // if (strlen(config.SubscribePV) !=0 ) {client.subscribe(config.SubscribePV,1);}
  // if (strlen(config.SubscribeTEMP) != 0 ) {client.subscribe(config.SubscribeTEMP,1);}
  // client.subscribe((command_button + "/#").c_str(),1);
  client.subscribe((command_number + "/#").c_str(),1);
  // client.subscribe((command_select + "/#").c_str(),1);
  client.subscribe((command_switch + "/#").c_str(),1);
  client.subscribe((HA_status).c_str(),1);
  // Serial.println((command_button + "/#").c_str());
  Serial.println((command_number + "/#").c_str());
  // Serial.println((command_select + "/#").c_str());
  Serial.println((command_switch + "/#").c_str());
  logging.Set_log_init("MQTT callback started \r\n",true);
 // mqttConnected = true;
  // HA_discover();
  

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

// char arrayWill[64];

// //#define MQTT_HOST IPAddress(192, 168, 1, 20)
// void async_mqtt_init() {
// 	const String LASTWILL_TOPIC = topic_Xlyric + "status";
// 	LASTWILL_TOPIC.toCharArray(arrayWill, 64);
//   IPAddress ip;
//   ip.fromString(config.mqttserver);
//   DEBUG_PRINTLN(ip);
//   client.setClientId(node_id.c_str());
//   client.setKeepAlive(60);
//   client.setWill(arrayWill, 2, true, "offline");
//   client.setCredentials(configmqtt.username, configmqtt.password);
//   client.onDisconnect(onMqttDisconnect);
//   client.onSubscribe(onMqttSubscribe);
//   client.onMessage(callback);
//   client.setServer(ip, config.mqttport);
//   // client.setMaxTopicLength(768); // 1024 -> 768 
//   client.onConnect(onMqttConnect);
//   logging.Set_log_init("MQTT topic for dimmer(s) : ",true);
//   logging.Set_log_init(topic_Xlyric.c_str());
//   logging.Set_log_init("sensors/dimmer/state\r\n");
//   }

// void connectToMqtt() {
//   DEBUG_PRINTLN("Connecting to MQTT...");
//   logging.Set_log_init("Connecting to MQTT... \r\n",true);
//   client.connect();
// }

// void onMqttConnect(bool sessionPresent) {
//   Serial.println("Connected to MQTT.");
//   Serial.print("Session present: ");
//   Serial.println(sessionPresent);
//   client.publish(String(topic_Xlyric +"status").c_str(),1,true, "online");         // Once connected, publish online to the availability topic
//   client.subscribe((command_switch + "/#").c_str(),1);
//   client.subscribe((command_number + "/#").c_str(),1);
//   client.subscribe((HA_status).c_str(),1);
//   if (strcmp(config.topic_Shelly,"") != 0) client.subscribe(config.topic_Shelly,1);
//   logging.Set_log_init("MQTT connected \r\n",true);

// }

// void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
//   Serial.println("Disconnected from MQTT.");
//   logging.Set_log_init("MQTT disconnected \r\n",true);

//   // if (WiFi.isConnected()) {
//   //   connectToMqtt();
//     // delay(1000);
//     // HA_discover();
//   // }
// }


// void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
//   Serial.println("Subscribe acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
//   DEBUG_PRINTLN("  qos: ");
//   DEBUG_PRINTLN(qos);
// }


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

  // logging.Set_log_init("Subscribedtopic : " + String(Subscribedtopic)+ "\r\n");
  String fixedpayload = ((String)message).substring(0,len);
  // logging.Set_log_init("Payload : " + String(fixedpayload)+ "\r\n");
  JsonDocument doc2;
  deserializeJson(doc2, message);
  // if (strcmp( Subscribedtopic, command_switch.c_str() ) == 0) { 
  if (strstr( Subscribedtopic, command_switch.c_str() ) != NULL) { 
    if (doc2.containsKey("relay1")) { 
        int relay = doc2["relay1"]; 
        if ( relay == 0) { digitalWrite(RELAY1 , LOW); }
        else { digitalWrite(RELAY1 , HIGH); } 
        switch_1.send(String(relay));
    }
    if (doc2.containsKey("relay2")) { 
        int relay = doc2["relay2"]; 
        if ( relay == 0) { digitalWrite(RELAY2 , LOW); }
        else { digitalWrite(RELAY2 , HIGH); } 
        switch_2.send(String(relay));
    }
  }
  // Je n'ai pas trouvé d'infos sur le shelly em pour valider ceci : 
  if (strcmp(Subscribedtopic, config.topic_Shelly ) == 0 ) {
      if (strcmp( doc2["state"] , "unavailable" ) == 0 ) { gDisplayValues.Shelly = -2; }
      else { gDisplayValues.Shelly = doc2["state"];  }
  }
  if (strstr( Subscribedtopic, command_number.c_str() ) != NULL) { 
    if (doc2.containsKey("resistance")) { 
      int resistance = doc2["resistance"]; 
      if (config.resistance != resistance ) {
        config.resistance = resistance;
        device_resistance.send(String(resistance));
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
      // logging.Set_log_init("MQTT resend HA discovery \r\n");
      // HA_discover();
      // logging.Set_log_init("MQTT resend all exeptionnals values\r\n");

      // #ifdef RELAY1
      //   int relaystate = digitalRead(RELAY1); 
      //   switch_1.send(String(relaystate));
      // #endif
      // #ifdef RELAY2
      //   relaystate = digitalRead(RELAY2); 
      //   switch_2.send(String(relaystate));
      // #endif
      // device_grid.send(String("0"));
      // device_inject.send(String("0"));
      // device_resistance.send(String(config.resistance));

      // if (discovery_temp) {
      //   logging.Set_log_init("MQTT resend HA temperature discovery \r\n");
      //   temperature.HA_discovery();
      //   device_alarm_temp.HA_discovery();
      //   logging.Set_log_init("MQTT resend HA temperature values \r\n");
      //   temperature.send(String(gDisplayValues.temperature));
      //   device_alarm_temp.send(stringBoolMQTT(dallas.security));
      // }
    }
  }

  //
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
  connectToMqtt();
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