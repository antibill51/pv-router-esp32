#ifndef CONFIG
#define CONFIG

/**
 * WiFi credentials
 */
#define WEBSSERVER true
#define WIFI_ACTIVE  true
#define MQTT_CLIENT true
#define OTA true
#define WIFI_NETWORK "xxx"
#define WIFI_PASSWORD "xxx"


/**
 * Set this to false to disable Serial logging
 */
#define DEBUG true
//#define configMAX_PRIORITIES 1024


/**
 * The name of this device (as defined in the AWS IOT console).
 * Also used to set the hostname on the network
 */
#define DEVICE_NAME "Pv router ESP32"

/**
 * ADC input pin that is used to read out the CT sensor
 */

#ifdef  DEVKIT1
#define ADC_INPUT 34 // linky
#define ADC_PORTEUSE 35 // porteuse
#endif

#ifdef  TTGO
#define ADC_INPUT 32 // linky
#define ADC_PORTEUSE 33 // porteuse
#endif


#define ADC_MIDDLE 1893  /// en dessous laquelle on considère une valeur négative

/**
 * The voltage of your home, used to calculate the wattage.
 * Try setting this as accurately as possible.
 */
#define HOME_VOLTAGE 225.0

/**
 *  Dimmer 
 */

#define DIMMER true
#define DIMMERLOCAL false

#if DIMMERLOCAL 
#define outputPin  18 
#define zerocross  19 // for boards with CHANGEBLE input pins
#endif




/**
 * Timeout for the WiFi connection. When this is reached,
 * the ESP goes into deep sleep for 30seconds to try and
 * recover.
 */
#define WIFI_TIMEOUT 20000 // 20 seconds

/**
 * How long should we wait after a failed WiFi connection
 * before trying to set one up again.
 */
#define WIFI_RECOVER_TIME_MS 20000 // 20 seconds

/**
 * Dimensions of the OLED display attached to the ESP
 */
#define OLED_ON true
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/**
 * Force Emonlib to assume a 3.3V supply to the CT sensor
 */
#define emonTxV3 1


/**
 * Local measurements
 */
#define LOCAL_MEASUREMENTS 30


/**
 * Syncing time with an NTP server
 */
#define NTP_TIME_SYNC_ENABLED true
#define NTP_SERVER "europe.pool.ntp.org"
#define NTP_OFFSET_SECONDS 3600
#define NTP_UPDATE_INTERVAL_MS 3600000 /// synch de l'heure toute les heures

/**
 * Wether or not you want to enable Home Assistant integration
 */
#define HA_ENABLED false
#define HA_ADDRESS "*** YOUR HOME ASSISTANT IP ADDRESSS ***"
#define HA_PORT 8883
#define HA_USER ""
#define HA_PASSWORD ""

/**
 * The MQTT endpoint of the service we should connect to and receive messages
 * from.
 */
#define AWS_ENABLED false
#define AWS_IOT_ENDPOINT "**** YOUR AWS IOT ENDPOINT ****"
#define AWS_IOT_TOPIC "**** YOUR AWS IOT RULE ARN ****"

#define MQTT_CONNECT_DELAY 200
#define MQTT_CONNECT_TIMEOUT 20000 // 20 seconds


// Check which core Arduino is running on. This is done because updating the 
// display only works from the Arduino core.
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif



#endif