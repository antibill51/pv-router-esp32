#ifndef APPWEB_FUNCTIONS
#define APPWEB_FUNCTIONS

#include "energyFunctions.h"
#include "MQTT.h"

String configweb; 
extern DisplayValues gDisplayValues;
extern Config config; 
extern Configwifi configwifi; 
extern Mqtt configmqtt; 
extern Logs logging;
#ifdef  TTGO
#include <TFT_eSPI.h>
#include <SPI.h>
extern TFT_eSPI display ;   // Invoke library
#endif
int middleoscillo = 1800;

const char* PARAM_INPUT_1 = "send"; /// paramettre de retour sendmode
const char* PARAM_INPUT_2 = "cycle"; /// paramettre de retour cycle
const char* PARAM_INPUT_3 = "readtime"; /// paramettre de retour readtime
const char* PARAM_INPUT_4 = "cosphi"; /// paramettre de retour cosphi
const char* PARAM_INPUT_save = "save"; /// paramettre de retour cosphi
const char* PARAM_INPUT_dimmer = "dimmer"; /// paramettre de retour cosphi
const char* PARAM_INPUT_server = "server"; /// paramettre de retour server domotique
const char* PARAM_INPUT_IDX = "idx"; /// paramettre de retour idx
const char* PARAM_INPUT_IDXdimmer = "idxdimmer"; /// paramettre de retour idx
const char* PARAM_INPUT_port = "port"; /// paramettre de retour port server domotique
const char* PARAM_INPUT_delta = "delta"; /// paramettre retour delta
const char* PARAM_INPUT_deltaneg = "deltaneg"; /// paramettre retour deltaneg
const char* PARAM_INPUT_fuse = "fuse"; /// paramettre retour fusible numérique
const char* PARAM_INPUT_API = "apiKey"; /// paramettre de retour apiKey
const char* PARAM_INPUT_servermode = "servermode"; /// paramettre de retour activation de mode server
const char* PARAM_INPUT_dimmer_power = "POWER"; /// paramettre de retour activation de mode server
const char* PARAM_INPUT_facteur = "facteur"; /// paramettre retour delta
const char* PARAM_INPUT_tmax = "tmax"; /// paramettre retour delta
const char* PARAM_INPUT_mqttserver = "mqttserver"; /// paramettre retour mqttserver
const char* PARAM_INPUT_reset = "reset"; /// paramettre reset
const char* PARAM_INPUT_publish = "publish"; /// paramettre publication mqtt


//***********************************
//** Oscillo mode creation du tableau de mesure pour le graph
//***********************************

String oscilloscope() {
  int timer = 0; 
  int temp, signe; // , moyenne; 
  int freqmesure = 40; 
  String retour = "[[";
  #ifndef HARDWARE_MOD
    #ifndef HALF_AUTO
      config.cosphi = half;
    #endif
    int moyenne; 
    int sigma = 0;
  
    front();

    delay(15);
    
    delayMicroseconds (half*config.readtime); // correction décalage
    while ( timer < freqmesure ) {
      
      temp =  analogRead(ADC_INPUT); signe = analogRead(ADC_PORTEUSE);
      moyenne = middleoscillo  + signe/50; 
      sigma += temp;
      //moyenne = moyenne + abs(temp - middle) ;
      /// mode oscillo graph 

      
      retour += String(timer) + "," + String(moyenne) + "," + String(temp) + "],[" ; 
      timer ++ ;
      delayMicroseconds (config.readtime);
    } 
    
    temp =  analogRead(ADC_INPUT); signe = analogRead(ADC_PORTEUSE);
    moyenne = middleoscillo  + signe/50; 
    retour += String(timer) + "," + String(moyenne) + "," + String(temp) + "]]" ;
    middleoscillo = sigma / freqmesure ;

  #else  
    frontmod();

    while ( timer < freqmesure ) {
      
      temp =  analogRead(ADC_INPUT) - value0; signe = analogRead(ADC_PORTEUSE) - value0 ;
      retour += String(timer) + "," + String(signe) + "," + String(temp) + "],[" ; 
      timer ++ ;
      delayMicroseconds (config.readtime);
    } 
    
    temp =  analogRead(ADC_INPUT) - value0; signe = analogRead(ADC_PORTEUSE) - value0;
    retour += String(timer) + "," + String(signe) + "," + String(temp) + "]]" ;
    middleoscillo = value0 ;
  #endif

  return ( retour ); 
  
}


//***********************************
//************* retour des pages
//***********************************

String getState() {
  String state=STABLE; 
  if (gDisplayValues.watt >= config.delta  ) {   state = GRID; }
  if (gDisplayValues.watt <= config.deltaneg ) {   state = INJECTION; }
  
  state = state + ";" + int(gDisplayValues.watt) + ";" + gDisplayValues.dimmer + ";" + config.delta + ";" + config.deltaneg + ";" + gDisplayValues.temperature ;
  return String(state);
}

String stringbool(bool mybool){
  String truefalse = "true";
  if (mybool == false ) {truefalse = "";}
  return String(truefalse);
  }
//***********************************
String getSendmode() {
  String sendmode;
  if ( config.sending == 0 ) {   sendmode = "Off"; }
  else {   sendmode = "On"; }
  return String(sendmode);
}
//***********************************
String getServermode(String Servermode) {
  if ( Servermode == "screen" ) {  gDisplayValues.screenstate = !gDisplayValues.screenstate; }
  if ( Servermode == "Jeedom" ) {   config.UseJeedom = !config.UseJeedom;}
  if ( Servermode == "Autonome" ) {   config.autonome = !config.autonome; }
  if ( Servermode == "Dimmer local" ) {   config.dimmerlocal = !config.dimmerlocal; }
  if ( Servermode == "MQTT" ) {   config.mqtt = !config.mqtt; }
  if ( Servermode == "polarité" ) {   config.polarity = !config.polarity; }
  if ( Servermode == "HA" ) {   configmqtt.HA = !configmqtt.HA; 
                    if (configmqtt.HA) init_HA_sensor(); 
                    }
  if ( Servermode == "JEEDOM" ) { configmqtt.JEEDOM = !configmqtt.JEEDOM; }
  if ( Servermode == "DOMOTICZ" ) { configmqtt.DOMOTICZ = !configmqtt.DOMOTICZ; }
  if ( Servermode == "HTTP" ) { configmqtt.HTTP = !configmqtt.HTTP; }

  if ( Servermode == "flip" ) {   
              config.flip = !config.flip; 
              #ifdef  TTGO
              if (config.flip) display.setRotation(3);
              else display.setRotation(1);
              #endif
              }
  

return String(Servermode);
}
//***********************************
String getSigma() {
   return String(gDisplayValues.watt);
}

//***********************************
String getcosphi() {
  String rawdata ="";
   return String(rawdata) ;
}
//***********************************
String getpuissance() {
  //int tempvalue=15;
   int bestpuissance =1 ;
  return String(bestpuissance*config.facteur) ;
}
//***********************************
String getconfig() {
  configweb = String(config.IDXdimmer) + ";" +  config.num_fuse + ";"  + String(config.IDX) + ";"  +  String(VERSION) +";" + "middle" +";"+ config.delta +";"+config.cycle+";"+config.dimmer+";"+config.readtime +";"+stringbool(config.UseDomoticz)+";"+stringbool(config.UseJeedom)+";"+stringbool(config.autonome)+";"+config.apiKey+";"+stringbool(config.dimmerlocal)+";"+config.facteur+";"+stringbool(config.mqtt)+";"+config.mqttserver+ ";"  + String(config.Publish)+";"+config.deltaneg+";"+config.resistance+";"+config.polarity+";"+config.ScreenTime+";"+config.localfuse+";"+config.tmax+";"+config.voltage+";"+config.offset+";"+stringbool(config.flip)+";"+stringbool(configmqtt.HA)+";"+config.relayon+";"+config.relayoff+";"+ stringbool(configmqtt.JEEDOM)+";"+stringbool(configmqtt.DOMOTICZ)+";"+stringbool(configmqtt.HTTP);
  return String(configweb);
}
//***********************************
String getchart() {
  String retour ="" ;
    retour = oscilloscope() ;
      return String(retour);
}
//***********************************
//***********************************
String getwifi() {

   String retour =String(configwifi.SID) + ";" + String(configwifi.passwd)  ;
  return String(retour) ;
}

String getmqtt() {

   String retour =String(config.mqttserver) + ";" + String(config.Publish) + ";" + String(configmqtt.username) + ";" + String(configmqtt.password) + ";" + stringbool(config.mqtt) + ";" + String(config.IDX) + ";" + String(config.IDXdimmer) + ";" + String(config.mqttport)+";"+stringbool(configmqtt.HA)+";"+stringbool(configmqtt.JEEDOM)+";"+stringbool(configmqtt.DOMOTICZ)+";"+stringbool(configmqtt.HTTP);
  return String(retour) ;
}

String getdebug() {
  configweb = "";
  configweb += "middle:" + String(middle_debug) + "\r\n" ; 
  #ifndef HARDWARE_MOD
    // calcul du cos phi par recherche milieu de demi onde positive 
    int start=0;int end=0;int half=0  ;
      for ( int i=0; i < nbmesure; i ++ )
      {
        if ( porteuse[i] !=0  && start == 0 ) {start = i ;}
        if ( porteuse[i] ==0 && start != 0 && end == 0 ) {end = i  ;}
        configweb += String(i) + "; "+ String(tableau[i]) + "; "+ String(porteuse[i]) + "\r\n" ;
      }
      half = (nbmesure/2) - (end - start); 
      configweb += "cosphi :" + String(half) + "  end :" + String(end ) +"  start :" + String(start)  + "\r\n" ; 
    #else
      configweb += "Vrms :" + String(Vrms) + "V. \r\n" ;  
      configweb += "Irms :" + String(Irms) + "A. \r\n" ;  
      configweb += "P_VA :" + String(PVA) + "VA. \r\n" ;  
    #endif
    configweb += "PW :" + String(gDisplayValues.watt) + "W. \r\n" ;  
    return String(configweb);
  }

//***********************************
String getmemory() {
   String memory = "";
   // memory = String(ESP.getFreeHeap()) + ";" + String(ESP.getHeapFragmentation()) ;
      return String(memory);
  }

//***********************************
String getlogs() {
    logging.start = logging.init + logging.start  + "}1"; 
    logging.power = true ; logging.sct = true; logging.sinus = true; 
    return logging.start ; 
  }

//***********************************
String processor(const String& var){
   Serial.println(var);
   if (var == "SIGMA"){
    return getSigma();
  }
  else if (var == "SENDMODE"){
  
    return getSendmode();
  }
  else if (var == "STATE"){
    
    return getState();
  }  
return getState();
}


//***********************************
//************* Fonction domotique 
//***********************************

void SendToDomotic(String Svalue){
  String baseurl; 
  Serial.print("connecting to mqtt & dimmer");
  Serial.println(config.hostname);
  
 /* if ( config.mqtt == 1 ) {     mqtt(config.IDX,Svalue);  }*/
  Serial.println(baseurl);

  // http.begin(config.hostname,config.port,baseurl);
  //int httpCode = http.GET();
  //Serial.println("closing connection");
  //http.end();
/*
  if ( config.autonome == 1 && change == 1   ) {  baseurl = "/?POWER=" + String(dimmer_power) ; http.begin(config.dimmer,80,baseurl);   int httpCode = http.GET();
    http.end(); 
    if ( config.mqtt == 1 ) { mqtt(config.IDXdimmer, String(dimmer_power));  }
    delay (3000); // delay de transmission réseau dimmer et application de la charge
    */  
    
    //}


 
}

/*
void mqtt(String idx, String value)
{
  String nvalue = "0" ; 
  if ( value != "0" ) { nvalue = "2" ; }
String message = "  { \"idx\" : " + idx +" ,   \"svalue\" : \"" + value + "\",  \"nvalue\" : " + nvalue + "  } ";

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  client.publish("domoticz/in", String(message).c_str(), true);
  
}*/


String injection_type() {
      String state = STABLE; 
      if (gDisplayValues.watt >= config.delta ) {   state = GRID; }
      if (gDisplayValues.watt <= config.deltaneg ) {   state = INJECTION; }
      
      return (state);
}

/*
*  récupération de la température sur le dimmer 
*/
String Dimmer_temp(char* host) {
WiFiClient client;
  
  String url = "/state";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('OK');

  return (line);
   
}
#endif