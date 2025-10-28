#ifndef HA_FUNCTIONS
#define HA_FUNCTIONS

//#include <PubSubClient.h>
// #include <AsyncMqttClient.h>
// extern AsyncMqttClient  client;

#include <espMqttClientAsync.h>
extern espMqttClientAsync  client;

extern Config config;
extern Mqtt configmqtt; 

String stringBool(bool mybool);

struct MQTT
{
  private:int MQTT_INTERVAL = 60;

  private:String name; 
  public:void Set_name(String setter) {name=setter; }

  private:char object_id[30]; 
  public:void Set_object_id(String setter) {
    snprintf(object_id, sizeof(object_id), "%s", setter.c_str());}

  private:String dev_cla; 
  public:void Set_dev_cla(String setter) {dev_cla=setter; }

  private:String unit_of_meas; 
  public:void Set_unit_of_meas(String setter) {unit_of_meas=setter; }

  private:String stat_cla; 
  public:void Set_stat_cla(String setter) {stat_cla=setter; }

  private:String entity_category; 
  public:void Set_entity_category(String setter) {entity_category=setter; }
  
  private:char entity_type[14]; 
  public:void Set_entity_type(String setter) {
    snprintf(entity_type, sizeof(entity_type), "%s", setter.c_str());}

  private:String icon; 
  public:void Set_icon(String setter) {icon=setter; }
  //{icon="\"ic\": \""+ setter +"\", "; }

  private:String min; 
  public:void Set_entity_valuemin(String setter) {min=setter; }

  private:String max; 
  public:void Set_entity_valuemax(String setter) {max=setter; }

  private:String step; 
  public:void Set_entity_valuestep(String setter) {step=setter; }

  private:String entity_option; 
  public:void Set_entity_option(String setter) {entity_option=setter; }

  private:bool retain_flag; 
  public:void Set_retain_flag(bool setter) {retain_flag=setter; }

  private:int qos; 
  public:void Set_entity_qos(int setter) {qos=setter; }

  private:String retain; 
  public:void Set_retain(bool setter) {
  if (setter) {retain=stringBool(setter); }
  }

  private:String expire_after; 
  public:void Set_expire_after(bool setter) {
    if (setter) {expire_after=R"("exp_aft": ")" + String(MQTT_INTERVAL) + R"(", )"; }
  }

private:char node_id[21];

private:
  void createHA_sensor_type(JsonObject& root) {



char obj_id[62]; // node_id + object_id + 1 -1
// char uniq_id[51]; // node_id + object_id -1
char topic_Xlyric[40]; // 8 + node_id
char stat_t[83]; // 14 + topic_Xlyric + object_id -1 
char avty_t[46]; // 6 + topic_Xlyric
char value_template[47]; // 17 + object_id

char cmd_t[93]; //9+topic_Xlyric + entity_type + object_id -2
      // snprintf(node_mac, sizeof(node_mac), "%s%s", (WiFi.macAddress().substring(12,14)).c_str(), (WiFi.macAddress().substring(15,17)).c_str());
      // snprintf(uniq_id, sizeof(uniq_id), "%s-%s", node_mac, object_id);
      snprintf(obj_id, sizeof(obj_id), "%s-%s", node_id, object_id);
      snprintf(topic_Xlyric, sizeof(topic_Xlyric), "Xlyric/%s/", node_id);
      snprintf(stat_t, sizeof(stat_t), "%ssensors/%s/state", topic_Xlyric,object_id );
      snprintf(avty_t, sizeof(avty_t), "%sstatus", topic_Xlyric);

      root["name"] = name;
      root["obj_id"] = obj_id;
      root["uniq_id"] = obj_id;
      root["stat_t"] =  stat_t;
      root["avty_t"] = avty_t;
      if (!strcmp(entity_type, "button") == 0) {
        snprintf(value_template, sizeof(value_template), "{{ value_json.%s }}", object_id);
      }
      if (!strcmp(entity_type, "sensor") == 0 && !strcmp(entity_type, "binary_sensor") == 0) {
      snprintf(cmd_t, sizeof(cmd_t), "%scommand/%s/%s", topic_Xlyric,entity_type,object_id );
      }
      
      if (strcmp(entity_type, "sensor") == 0) {
          root["dev_cla"] = dev_cla;
          root["unit_of_meas"] = unit_of_meas;
          root["stat_cla"] = stat_cla;
          root["val_tpl"] = value_template;
      }
      else if (strcmp(entity_type, "switch") == 0) {
          char pl_on[44]; //14+object_id
          char pl_off[44]; //14+object_id
          snprintf(pl_on, sizeof(pl_on), "{ \"%s\" : \"1\"  } ", object_id);
          snprintf(pl_off, sizeof(pl_off), "{ \"%s\" : \"0\"  } ", object_id);

          root["val_tpl"] = value_template;
          root["pl"] = value_template;
          root["pl_on"] = pl_on;
          root["pl_off"] = pl_off;
          root["stat_on"] = 1;
          root["stat_off"] = 0;
          root["qos"] = 1;
          root["cmd_t"] = cmd_t;
      } 
      else if (strcmp(entity_type, "number") == 0 || strcmp(entity_type, "select") == 0) {
          char cmd_tpl[50]; //20+object_id
          snprintf(cmd_tpl, sizeof(cmd_tpl), "{\"%s\": {{ value }} }", object_id );
          root["val_tpl"] = value_template;
          root["cmd_t"] = cmd_t;
          root["cmd_tpl"] = cmd_tpl;
          root["entity_category"] = entity_category;

          if (strcmp(entity_type, "number") == 0) {
              root["max"] = max;
              root["min"] = min;
              root["step"] = step;
          } 
          else if (strcmp(entity_type, "select") == 0) {
            // JsonArray options = root.createNestedArray("options");
            JsonArray options = root["options"].to<JsonArray>();
            options.add("off");
            options.add("delester");
            options.add("equal");
          }
      } 
      else if (strcmp(entity_type, "binary_sensor") == 0) {
          root["dev_cla"] = dev_cla;
          root["pl_on"] = "true";
          root["pl_off"] = "false";
          root["val_tpl"] = value_template;
      }
      else if (strcmp(entity_type, "button") == 0) {
        char pl_prs[44]; //14+object_id    
          snprintf(pl_prs, sizeof(pl_prs), "{\"%s\": \"1\" }", object_id );
          root["entity_category"] = entity_category;
          root["cmd_t"] = cmd_t;
          root["pl_prs"] = pl_prs;
      }
      if (!icon.isEmpty()) {
          root["ic"] = icon;
      }
        if (!retain.isEmpty()) {
          root["ret"] = retain;
      }
      if (!expire_after.isEmpty()) {
          root["exp_aft"] = String(MQTT_INTERVAL);
      }
  }

  String getIPaddress() {
      return WiFi.localIP().toString();
  }

  void createHA_device_declare(JsonObject& root) { 
      root["ids"] = node_id;
      root["name"] = node_id;
      root["sw"] = VERSION;
      root["mdl"] = "ESP32 TTGO " + getIPaddress();
      root["mf"] = "Cyril Poissonnier";
      root["cu"] = "http://" + getIPaddress();
  }

public:
  void HA_discovery(){
    strcpy(node_id, ("PvRouter-"+WiFi.macAddress().substring(12,14)+ WiFi.macAddress().substring(15,17)).c_str()); 
    if (client.connected() && configmqtt.HA){
      JsonDocument device;
      JsonObject root = device.to<JsonObject>();

      char topic[97]; // 23 + entity_type + node_id + object_id -2
      snprintf(topic, sizeof(topic), "homeassistant/%s/%s/%s/config", entity_type,node_id,object_id );

      JsonObject deviceObj = root["device"].to<JsonObject>(); // Création d'un objet JSON imbriqué pour "device"
      createHA_device_declare(deviceObj);

      createHA_sensor_type(root); // Appel de la fonction pour créer les données relatives au capteur

      char output[700];
      // String output;
      serializeJson(root, output);

      // logging.Set_log_init("String lenght: ",true);
      // logging.Set_log_init(String(output.length()));
      // logging.Set_log_init("\r\n");

      int status;
      status = client.publish(topic, 1, false, output);
      // status = client.publish(topic, 1, false, output.c_str());

      if (status == 0) {
        logging.Set_log_init("MQTT ERROR : discovery not sended for ",true);
        logging.Set_log_init(object_id);
        logging.Set_log_init("\r\n");
      }
    }
  }

    public:void send(String value){
      if (client.connected()){
    if (configmqtt.JEEDOM || configmqtt.HA) {

      char topic[84]; // 22 +  node_id  + object_id
      snprintf(topic, sizeof(topic), "Xlyric/%s/sensors/%s/state",node_id,object_id );

      char message[50]; // 11 +  object_id + value?
      snprintf(message, sizeof(message),"{\"%s\":\"%s\"}" ,object_id, value.c_str());
      int status;
      // String message = R"({")" + object_id + R"(" : ")" + value.c_str() + R"("} )";
      status = client.publish(topic ,qos, retain_flag , message);  
      if (status == 0) {
        logging.Set_log_init("MQTT ERROR : discovery not sended for ",true);
        logging.Set_log_init(object_id);
        logging.Set_log_init("\r\n");
      }
    }
      }
  } 
};

#endif