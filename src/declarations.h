#ifndef declarations_h
#define declarations_h

#include <Arduino.h>
#include <ESP8266WiFi.h>

/** Build time */
const char compileDate[] = __DATE__ " " __TIME__;

/** WiFiServer class to create TCP socket server on port tcpComPort */
WiFiServer tcpServer(tcpComPort);

/** Hostname & AP name created from device function & 1. and 4. to 6. part of MAC address */
char hostApName[] = "MHC-Sec-xxxxxxxx";
/** Debug name created from last part of hostname */
String OTA_HOST = "Sec-xxxxxxxx";
/** IP address of this module */
IPAddress ipAddr;

/** Timer to collect light information from DHT11 sensor */
Ticker getWeatherTimer;
/** Timer for heart beat */
Ticker heartBeatTimer;
/** Timer to switch off the relay */
Ticker relayOffTimer;
/** Timer for alarm siren */
Ticker alarmTimer;

/** Flag for alarm activity */
boolean alarmOn = true;
/** Flag if heart beat was triggered */
boolean heartBeatTriggered = false;
/** Flag if panic button was pressed */
boolean panicOn = false;

/** Flag if lights should be switched on after movement detection */
boolean switchLights = false;
/** Flag for PIR status change */
boolean pirTriggered = false;
/** Flag for request to read out light sensor */
boolean weatherUpdateTriggered = false;
/** Flag for detection status */
boolean hasDetection = false;
/** Flag for light switched off */
boolean lightOffTriggered = false;

/** Relay on delay time in seconds */
int onTime = 300;
/** Bug capture trial year of last good NTP time received */
int lastKnownYear = 0;

/** Instance of the DHT sensor */
DHTesp dht;
/** Result of last temperature measurement */
float tempValue = 0.0;
/** Result of last humidity measurement */
float humidValue = 0.0;

/** Flag for OTA update */
bool otaRunning = false;
/** Flag for debugging */
bool debugOn = false;

/** Flag for automatic activation/deactivation of alarm */
boolean hasAutoActivation = true;
/** Hour for automatic activation of alarm (24h format) */
int autoActivOn = 23;
/** Hour for automatic deactivation of alarm (24h format) */
int autoActivOff = 7;

#endif
