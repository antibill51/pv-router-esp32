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

  private:char default_entity_id[30]; 
  public:void Set_default_entity_id(String setter) {
    snprintf(default_entity_id, sizeof(default_entity_id), "%s", setter.c_str());}

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

char uniq_id[62]; // (MODIFIE) Pour garder l'ID unique sans préfixe
char def_ent_id[80]; // (MODIFIE) Taille augmentée pour "domaine.id"
char topic_Xlyric[40]; // 8 + node_id
char stat_t[100]; // (MODIFIE) augmentation pour la sécurité
char avty_t[46]; // 6 + topic_Xlyric
char value_template[47]; // 17 + default_entity_id

char cmd_t[93]; //9+topic_Xlyric + entity_type + default_entity_id -2

      // MODIFICATION ICI : Séparation de uniq_id et construction du default_entity_id avec son domaine
      snprintf(uniq_id, sizeof(uniq_id), "%s-%s", node_id, default_entity_id);
      snprintf(def_ent_id, sizeof(def_ent_id), "%s.%s", entity_type, uniq_id);
      
      snprintf(topic_Xlyric, sizeof(topic_Xlyric), "Xlyric/%s/", node_id);
      
      // MODIFICATION ICI : remplacement de "sensors" en dur
      snprintf(stat_t, sizeof(stat_t), "%s%s/%s/state", topic_Xlyric, entity_type, default_entity_id );
      
      snprintf(avty_t, sizeof(avty_t), "%sstatus", topic_Xlyric);

      root["name"] = name;
      root["def_ent_id"] = def_ent_id;
      root["uniq_id"] = uniq_id; // On passe la variable uniq_id propre
      root["stat_t"] =  stat_t;
      root["avty_t"] = avty_t;
      if (!strcmp(entity_type, "button") == 0) {
        snprintf(value_template, sizeof(value_template), "{{ value_json.%s }}", default_entity_id);
      }
      if (!strcmp(entity_type, "sensor") == 0 && !strcmp(entity_type, "binary_sensor") == 0) {
      snprintf(cmd_t, sizeof(cmd_t), "%scommand/%s/%s", topic_Xlyric,entity_type,default_entity_id );
      }
      
      if (strcmp(entity_type, "sensor") == 0) {
          root["dev_cla"] = dev_cla;
          root["unit_of_meas"] = unit_of_meas;
          root["stat_cla"] = stat_cla;
          root["val_tpl"] = value_template;
      }
      else if (strcmp(entity_type, "switch") == 0) {
          char pl_on[44]; //14+default_entity_id
          char pl_off[44]; //14+default_entity_id
          snprintf(pl_on, sizeof(pl_on), "{ \"%s\" : \"1\"  } ", default_entity_id);
          snprintf(pl_off, sizeof(pl_off), "{ \"%s\" : \"0\"  } ", default_entity_id);

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
          char cmd_tpl[50]; //20+default_entity_id
          snprintf(cmd_tpl, sizeof(cmd_tpl), "{\"%s\": {{ value }} }", default_entity_id );
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
        char pl_prs[44]; //14+default_entity_id    
          snprintf(pl_prs, sizeof(pl_prs), "{\"%s\": \"1\" }", default_entity_id );
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

      char topic[97]; // 23 + entity_type + node_id + default_entity_id -2
      snprintf(topic, sizeof(topic), "homeassistant/%s/%s/%s/config", entity_type,node_id,default_entity_id );

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
        logging.Set_log_init(default_entity_id);
        logging.Set_log_init("\r\n");
      }
    }
  }

    public:void send(String value){
      if (client.connected()){
    if (configmqtt.JEEDOM || configmqtt.HA) {

      // MODIFICATION ICI: taille augmentée et remplacement de "sensors" par %s et entity_type
      char topic[100]; 
      snprintf(topic, sizeof(topic), "Xlyric/%s/%s/%s/state",node_id, entity_type, default_entity_id );

      // MODIFICATION ICI: taille augmentée de sécurité
      char message[100]; 
      snprintf(message, sizeof(message),"{\"%s\":\"%s\"}" ,default_entity_id, value.c_str());
      int status;
      // String message = R"({")" + default_entity_id + R"(" : ")" + value.c_str() + R"("} )";
      status = client.publish(topic ,qos, retain_flag , message);  
      if (status == 0) {
        logging.Set_log_init("MQTT ERROR : discovery not sended for ",true);
        logging.Set_log_init(default_entity_id);
        logging.Set_log_init("\r\n");
      }
    }
      }
  } 
};

#endif
