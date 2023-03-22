#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "appweb.h"

extern DisplayValues gDisplayValues;
extern Configmodule configmodule; 
extern Configwifi configwifi; 
extern Logs logging;

//***********************************
//************* Gestion du serveur WEB
//***********************************
// Create AsyncWebServer object on port 80
//WiFiClient domotic_client;
// mqtt
//PubSubClient client(domotic_client);

String inputMessage;

AsyncWebServer server(80);
//DNSServer dns;
//HTTPClient http;

		//***********************************
		//************* Setup - Web pages
		//***********************************

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void compress_html(AsyncWebServerRequest *request,String filefs , String format ) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, filefs, format );
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
}

void serveur_response(AsyncWebServerRequest *request, String response) {
  request->send_P(200, "text/plain", response.c_str());
}


void call_pages() {

//// pages ////
if (AP) {

    server.on("/",HTTP_GET, [](AsyncWebServerRequest *request){
    if(SPIFFS.exists("/index.html.gz")){
      compress_html(request,"/index-ap.html.gz", "text/html");
    }
    else {request->send_P(200, "text/html", "<html><body>Filesystem is not present. <a href='https://ota.apper-solaire.org/firmware/spiffs-ttgo.bin'>download it here</a> <br>and after  <a href='/update'>upload on the ESP here </a></body></html>" ); }
  });

  server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *request){
    if(SPIFFS.exists("/config.html.gz")){
      compress_html(request,"/config-ap.html.gz", "text/html");
    }
    else {
      //request->send_P(200, "text/plain", SPIFFSNO ); 
      serveur_response(request, SPIFFSNO);
    }

  });
}
else {
  server.on("/",HTTP_GET, [](AsyncWebServerRequest *request){
    if(SPIFFS.exists("/index.html.gz") ){
    #ifndef LIGHT_FIRMWARE
      compress_html(request,"/index.html.gz", "text/html");
    #else
      compress_html(request,"/index-light.html.gz", "text/html");
    #endif

    }
    else {
            //request->send_P(200, "text/plain", SPIFFSNO ); 
      serveur_response(request, SPIFFSNO);
     }
  });

  server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *request){
    if(SPIFFS.exists("/config.html.gz")){
    #ifndef LIGHT_FIRMWARE
       compress_html(request,"/config.html.gz", "text/html");
    #else
       compress_html(request,"/config-ap.html.gz", "text/html");
    #endif

    }
    else {
            //request->send_P(200, "text/plain", SPIFFSNO ); 
      serveur_response(request, SPIFFSNO);
      }

  });

}

  server.on("/all.min.css",  HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request,"/all.min.css.gz", "text/css");
  });

server.on("/jquery.min.js",  HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request,"/jquery.min.js.gz", "text/css");
  });

server.on("/bootstrap.bundle.min.js",  HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request,"/bootstrap.bundle.min.js.gz", "text/css");
  });

server.on("/bootstrap.bundle.min.js.map",  HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request,"/bootstrap.bundle.min.js.map.gz", "text/css");
  });

server.on("/favicon.ico",  HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "image/png");
  });

server.on("/fa-solid-900.woff2", HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request, "/fa-solid-900.woff2.gz", "text/css");
  });
  
server.on("/sb-admin-2.js", HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request, "/sb-admin-2.js.gz", "text/javascript");
  });

server.on("/sb-admin-2.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request, "/sb-admin-2.min.css.gz", "text/css");
  });

server.on("/mqtt.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/mqtt.json", "text/css");
  });

server.on("/config.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/config.json", "application/json");
  });
  

server.on("/log.html", HTTP_ANY, [](AsyncWebServerRequest *request){
      compress_html(request,"/log.html.gz", "text/html");
  });

///// Pages 
/// Appel de fonction 

if (!configmodule.pilote) {
  server.on("/chart.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "application/json", getchart().c_str());
  }); 
}

/*

  server.on("/doc.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/doc.txt", "text/plain");
  });

  server.on("/jquery.easing.min.js",  HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery.easing.min.js", "text/css");
  });

  server.on("/sendmode", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getSendmode().c_str());
  });

  server.on("/loader.js",  HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/loader.js", "text/css");
  });

  server.on("/google.css",  HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/google.css", "text/css");
  });

  server.on("/memory", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getmemory().c_str());
    serveur_response(request, getmemory() );
  });
  server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getdebug().c_str());
    serveur_response(request, getdebug() );
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getlogs().c_str());
    serveur_response(request, getlogs() );
  });
*/
  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getState().c_str());
    serveur_response(request, getState());
  });

  server.on("/serial", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getState().c_str());
    serveur_response(request, getState());
  });
  
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getconfig().c_str());
    serveur_response(request, getconfig() );
  });


/// beta ? 
server.on("/cosphi", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getcosphi().c_str());
    serveur_response(request, getcosphi());
  });
  
server.on("/puissance", HTTP_GET, [](AsyncWebServerRequest *request){
   // request->send_P(200, "text/plain",  getpuissance().c_str());
     serveur_response(request, getpuissance());
  });

///////////////
//// wifi
///////////////

server.on("/wifi.html", HTTP_GET, [](AsyncWebServerRequest *request){
      compress_html(request,"/wifi.html.gz", "text/html");
  });


server.on("/getwifi", HTTP_ANY, [] (AsyncWebServerRequest *request) {
  //request->send(200, "text/plain",  getwifi().c_str()); 
  serveur_response(request, getwifi());
  
});

///////////////
///// MQTT
///////////////

server.on("/mqtt.html", HTTP_GET, [](AsyncWebServerRequest *request){
    compress_html(request,"/mqtt.html.gz", "text/html");
  });

server.on("/getmqtt", HTTP_ANY, [] (AsyncWebServerRequest *request) {
  //request->send(200, "text/plain",  getmqtt().c_str()); 
  serveur_response(request, getmqtt());
});
 /// il serait bien que /getmqtt et getwifi soit directement en processing de l'appel de la page 


///////////////
//// logs ////
///////////////

server.on("/cs", HTTP_ANY, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", getlogs().c_str());
    serveur_response(request,  getlogs());
    logging.init="197}11}1";
    logging.start = "";
  });

  server.on("/reset", HTTP_ANY, [](AsyncWebServerRequest *request){
     //request->send_P(200, "text/plain",PV_RESTART);
     serveur_response(request,  PV_RESTART);
       ESP.restart();
  });

  server.on("/reboot", HTTP_ANY, [](AsyncWebServerRequest *request){
   request->redirect("/");
    config.restart = true;
  });

server.onNotFound(notFound);

/////////////////////////
////// mise à jour parametre d'envoie vers domoticz et récupération des modifications de configurations
/////////////////////////

server.on("/get", HTTP_ANY, [] (AsyncWebServerRequest *request) {
      ///   /get?send=on
   /* if (request->hasParam(PARAM_INPUT_1)) 		  { inputMessage = request->getParam(PARAM_INPUT_1)->value();
													config.sending = 0; 
													if ( inputMessage != "On" ) { config.sending = 1; }
													request->send(200, "text/html", getSendmode().c_str()); 	}*/
	   // /get?cycle=x
    if (request->hasParam(PARAM_INPUT_save)) { Serial.println(F("Saving configuration..."));
                          saveConfiguration(filename_conf, config);   
                            }
                           
	 if (request->hasParam(PARAM_INPUT_2)) { config.cycle = request->getParam(PARAM_INPUT_2)->value().toInt(); }
	 if (request->hasParam(PARAM_INPUT_3)) { config.readtime = request->getParam(PARAM_INPUT_3)->value().toInt();}
	 if (request->hasParam(PARAM_INPUT_4)) { config.cosphi = request->getParam(PARAM_INPUT_4)->value().toInt();  }
   if (request->hasParam(PARAM_INPUT_dimmer)) { request->getParam(PARAM_INPUT_dimmer)->value().toCharArray(config.dimmer,16);  }
   if (request->hasParam(PARAM_INPUT_server)) { request->getParam(PARAM_INPUT_server)->value().toCharArray(config.hostname,16);  }
   if (request->hasParam(PARAM_INPUT_delta)) { config.delta = request->getParam(PARAM_INPUT_delta)->value().toInt(); }
   if (request->hasParam(PARAM_INPUT_deltaneg)) { config.deltaneg = request->getParam(PARAM_INPUT_deltaneg)->value().toInt(); }
   if (request->hasParam(PARAM_INPUT_fuse)) { config.num_fuse = request->getParam(PARAM_INPUT_fuse)->value().toInt(); }
   if (request->hasParam(PARAM_INPUT_port)) { config.port = request->getParam(PARAM_INPUT_port)->value().toInt(); }
   if (request->hasParam(PARAM_INPUT_IDX)) { config.IDX = request->getParam(PARAM_INPUT_IDX)->value().toInt();}
   if (request->hasParam(PARAM_INPUT_IDXdimmer)) { config.IDXdimmer = request->getParam(PARAM_INPUT_IDXdimmer)->value().toInt();}
   if (request->hasParam(PARAM_INPUT_API)) { request->getParam(PARAM_INPUT_API)->value().toCharArray(config.apiKey,64);}
   if (request->hasParam(PARAM_INPUT_dimmer_power)) {gDisplayValues.dimmer = request->getParam( PARAM_INPUT_dimmer_power)->value().toInt(); gDisplayValues.change = 1 ;  } 
   if (request->hasParam(PARAM_INPUT_facteur)) { config.facteur = request->getParam(PARAM_INPUT_facteur)->value().toFloat();}
   if (request->hasParam(PARAM_INPUT_tmax)) { config.tmax = request->getParam(PARAM_INPUT_tmax)->value().toInt();}
   if (request->hasParam("resistance")) { config.resistance = request->getParam("resistance")->value().toInt();}
   if (request->hasParam("screentime")) { config.ScreenTime = request->getParam("screentime")->value().toInt();}
   if (request->hasParam("voltage")) { config.voltage = request->getParam("voltage")->value().toFloat();}
   if (request->hasParam("offset")) { config.offset = request->getParam("offset")->value().toFloat();}

   /// @brief  wifi
   if (request->hasParam("ssid")) { request->getParam("ssid")->value().toCharArray(configwifi.SID,50);  }
   if (request->hasParam("password")) { request->getParam("password")->value().toCharArray(configwifi.passwd,50);    
    logging.start += loguptime();
    logging.start += "saving wifi\r\n";
   configwifi.sauve_wifi();
   }

   //// MQTT
   if (request->hasParam(PARAM_INPUT_mqttserver)) { request->getParam(PARAM_INPUT_mqttserver)->value().toCharArray(config.mqttserver,16);  }
   if (request->hasParam(PARAM_INPUT_publish)) { request->getParam(PARAM_INPUT_publish)->value().toCharArray(config.Publish,100); 
      saveConfiguration(filename_conf, config);   }
   if (request->hasParam("mqttuser")) { request->getParam("mqttuser")->value().toCharArray(configmqtt.username,50);  }
   if (request->hasParam("mqttport")) { config.mqttport = request->getParam("mqttport")->value().toInt();}
   if (request->hasParam("mqttpassword")) { request->getParam("mqttpassword")->value().toCharArray(configmqtt.password,50); 
      savemqtt(mqtt_conf, configmqtt); }

  //// Dimmer local
    if (request->hasParam("Fusiblelocal")) { config.localfuse = request->getParam("Fusiblelocal")->value().toInt();}
    if (request->hasParam("maxtemp")) { config.tmax = request->getParam("maxtemp")->value().toInt();}

   //reset
   if (request->hasParam(PARAM_INPUT_reset)) {Serial.println("Resetting ESP");  ESP.restart();}

   //// for check boxs in web pages  
   if (request->hasParam(PARAM_INPUT_servermode)) { inputMessage = request->getParam( PARAM_INPUT_servermode)->value();
                                            getServermode(inputMessage);
                                            request->send(200, "text/html", getconfig().c_str());
                                            saveConfiguration(filename_conf, config);
                                            savemqtt(mqtt_conf, configmqtt);
                                        }

    /// relays : 0 : off , 1 : on , other : switch 
    if (request->hasParam("relay1")) { int relay = request->getParam("relay1")->value().toInt(); 
        if ( relay == 0 ) { digitalWrite(RELAY1 , LOW); }
        else if ( relay == 1 ) { digitalWrite(RELAY1 , HIGH); } 
        else digitalWrite(RELAY1, !digitalRead(RELAY1));
        #ifndef LIGHT_FIRMWARE
          switch_1.send(String(relay));
        #endif

    }
    if (request->hasParam("relay2")) { int relay = request->getParam("relay2")->value().toInt(); 
        if ( relay == 0) { digitalWrite(RELAY2 , LOW); }
        else if ( relay == 1 ) { digitalWrite(RELAY2 , HIGH); } 
        else digitalWrite(RELAY2, !digitalRead(RELAY2));
        #ifndef LIGHT_FIRMWARE
          switch_2.send(String(relay));
        #endif

    }
    if (request->hasParam("relaystart")) { config.relayon = request->getParam("relaystart")->value().toInt();}
    if (request->hasParam("relaystop")) { config.relayoff = request->getParam("relaystop")->value().toInt();}

    //request->send(200, "text/html", getconfig().c_str());
    serveur_response(request,  getconfig());
	}); 

}

