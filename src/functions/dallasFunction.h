#ifndef DALLAS_FUNCTIONS
#define DALLAS_FUNCTIONS

#include "../config/config.h"
#include "../config/enums.h"
#include "functions/ha.h"

    //***********************************
    //************* Test de la présence d'une 18b20 
    //***********************************
    
extern Dallas dallas ;
extern Logs logging; 
OneWire ds(ONE_WIRE_BUS);
DallasTemperature sensors(&ds);
DeviceAddress insideThermometer;

 /* 
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius = 0.00 ;
  
  int refresh = 30;
  int refreshcount = 0; 
*/
byte i;
bool dallaspresent () {

if ( !ds.search(dallas.addr)) {
    Serial.println("Dallas not connected");
    logging.init += loguptime();
    logging.init += "Dallas not connected\r\n";
    Serial.println();
    ds.reset_search();
    delay(250);
    return false;
  }
  
  Serial.print("ROM =");
  for(i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(dallas.addr[i], HEX);
  }

   Serial.println();
 
  // the first ROM byte indicates which chip
  switch (dallas.addr[0]) {
    case 0x10:
      Serial.println(DALLAS_TEXT);  // or old DS1820
      dallas.type_s = 1;
      break;
    case 0x28:
      Serial.println(DALLAS_TEXT);
      dallas.type_s = 0;
      break;
    case 0x22:
      Serial.println(DALLAS_TEXT);
      dallas.type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return false;
  } 

  ds.reset();
  ds.select(dallas.addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  dallas.present = ds.reset();    ///  byte 0 > 1 si present
  ds.select(dallas.addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  present = ");
  Serial.println(dallas.present, HEX);
      logging.init += loguptime();
  logging.init += "Dallas present at address" + String(dallas.present, HEX) + "\r\n";

#ifndef LIGHT_FIRMWARE
  if (!discovery_temp) {
    discovery_temp = true;
    temperature.HA_discovery();
    device_alarm_temp.HA_discovery();
  }
#endif

  return true;
   
  }

    //***********************************
    //************* récupération d'une température du 18b20
    //***********************************

float CheckTemperature(String label, byte deviceAddress[12]){
  sensors.requestTemperatures(); 

  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(label);
  //logging.init += loguptime();
    if ( (tempC == -127.00) || (tempC == -255.00) ) {
    
    //// cas d'une sonde trop longue à préparer les valeurs 
    delay(187); /// attente de 187ms ( temps de réponse de la sonde )
    tempC = sensors.getTempC(deviceAddress);
      if ( (tempC == -127.00) || (tempC == -255.00) ) {
      Serial.print("Error getting temperature");
       logging.start += "Dallas on error\r\n";
       tempC = gDisplayValues.temperature.toFloat(); 
      }
  } else {
    Serial.print(" Temp C: ");
    Serial.println(tempC);
    logging.start += "temp :"+ String(tempC) +" \r\n";
    return (tempC); 
   
    
  }  
  return (tempC); 
}

#endif