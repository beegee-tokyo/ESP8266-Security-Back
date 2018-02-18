#include <ESP8266WiFi.h>

/** Input from PIR sensor */
#define pirPort 4
/** Output to activate Relay */
#define relayPort 5
/** Output to loudspeaker or piezo */
// #define speakerPin 15
#define speakerPin 12
/** Definition of data pin for DHT sensor */
#define DHTPIN 14

/** Hostname & AP name created from device function & 1. and 4. to 6. part of MAC address */
extern char hostApName[];
/** Debug name created from last part of hostname */
extern String OTA_HOST;
/** IP address of this module */
extern IPAddress ipAddr;
/** ID for monitor to be replaced by function in the future */
#define DEVICE_ID "sb1"

/** Build time */
extern const char compileDate [];
/** WiFiServer class to create TCP socket server on port tcpComPort */
extern WiFiServer tcpServer;
/** IP address of this module */
extern IPAddress ipAddr;

/** Timer to switch off the relay */
extern Ticker relayOffTimer;
/** Timer for alarm siren */
extern Ticker alarmTimer;

/** Flag for alarm activity */
extern boolean alarmOn;
/** Flag if heart beat was triggered */
extern boolean heartBeatTriggered;
/** Flag if panic button was pressed */
extern boolean panicOn;
/** Flag for debugging */
extern bool debugOn;
/** Relay on delay time in seconds */
extern int onTime;
/** Flag for WiFi connection */
extern bool wmIsConnected;
/** Bug capture trial year of last good NTP time received */
extern int lastKnownYear;

/** Flag if lights should be switched on after movement detection */
extern boolean switchLights;
/** Flag for PIR status change */
extern boolean pirTriggered;
/** Flag for request to read out light sensor */
extern boolean weatherUpdateTriggered;
/** Flag for detection status */
extern boolean hasDetection;
/** Flag for light switched off */
extern boolean lightOffTriggered;

/** Flag for boot status */
extern boolean inSetup;
/** String with reboot reason */
extern String rebootReason;
/** String with last known reboot reason */
extern String lastRebootReason;

/** Instance of the DHT sensor */
extern DHTesp dht; // 11 works fine for ESP8266
/** Result of last temperature measurement */
extern float tempValue;
/** Result of last humidity measurement */
extern float humidValue;

/** Flag for OTA update */
extern bool otaRunning;

/** Flag for automatic activation/deactivation of alarm */
extern boolean hasAutoActivation;
/** Hour for automatic activation of alarm (24h format) */
extern int autoActivOn;
/** Hour for automatic deactivation of alarm (24h format) */
extern int autoActivOff;
