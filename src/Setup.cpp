#include "Setup.h"
#include "declarations.h"

/** Timer to collect light information from TSL2561 sensor */
Ticker getWeatherTimer;
/** Timer for heart beat */
Ticker heartBeatTimer;

/**
 * Initialization of GPIO pins, WiFi connection, timers and sensors
 */
void setup() {

	initLeds();
	pinMode(pirPort, INPUT); // PIR signal
	pinMode(relayPort, OUTPUT); // Relay trigger signal
	pinMode(speakerPin, OUTPUT); // Loudspeaker/piezo signal
	digitalWrite(relayPort, LOW); // Turn off Relay
	digitalWrite(speakerPin, HIGH); // Switch Piezo buzzer off

	Serial.begin(115200);
	Serial.setDebugOutput(false);
	Serial.println("");
	Serial.println("====================");

	// Initialize file system.
	bool spiffsOK = false;
	if (!SPIFFS.begin())
	{
		if (SPIFFS.format()){
			spiffsOK = true;
		}
	} else { // SPIFFS ready to use
		spiffsOK = true;
	}
	if (spiffsOK) {
		char tmpLoc[40];
		if (getConfigEntry("loc", tmpLoc)) {
			devLoc = String(tmpLoc);
		}
		if (getConfigEntry("light", tmpLoc)) {
			lightID = String(tmpLoc);
		}
		if (getConfigEntry("cam", tmpLoc)) {
			camID = String(tmpLoc);
		}
		if (getConfigEntry("sec", tmpLoc)) {
			secID = String(tmpLoc);
		}
	}

	// Create device ID from MAC address
	String macAddress = WiFi.macAddress();
	hostApName[8] = OTA_HOST[4] = macAddress[0];
	hostApName[9] = OTA_HOST[5] = macAddress[1];
	hostApName[10] = OTA_HOST[6] = macAddress[9];
	hostApName[11] = OTA_HOST[7] = macAddress[10];
	hostApName[12] = OTA_HOST[8] = macAddress[12];
	hostApName[13] = OTA_HOST[9] = macAddress[13];
	hostApName[14] = OTA_HOST[10] = macAddress[15];
	hostApName[15] = OTA_HOST[11] = macAddress[16];

	Serial.println(hostApName);

	// resetWiFiCredentials();
	// Add parameter for the wifiManager
	WiFiManagerParameter wmDevLoc("loc","House",(char *)&devLoc[0],40);
	wifiManager.addParameter(&wmDevLoc);
	WiFiManagerParameter wmLightID("light","Light",(char *)&lightID[0],40);
	wifiManager.addParameter(&wmLightID);
	WiFiManagerParameter wmCamID("cam","Camera",(char *)&camID[0],40);
	wifiManager.addParameter(&wmCamID);
	WiFiManagerParameter wmSecID("sec","Security",(char *)&secID[0],40);
	wifiManager.addParameter(&wmSecID);
	wifiManager.setSaveConfigCallback(saveConfigCallback);

	// First try to connect with known credentials
	if (!WiFi.reconnect()) {
		// connection failed, try to connect with captive portal
		ipAddr = connectWiFi(hostApName);

		// Check if the configuration data has changed
		if (shouldSaveConfig) {
			if (wmDevLoc.getValueLength() != 0) {
				devLoc = String(wmDevLoc.getValue());
				saveConfigEntry("loc", devLoc);
			}
			if (wmLightID.getValueLength() != 0) {
				lightID = String(wmLightID.getValue());
				saveConfigEntry("light", lightID);
			}
			if (wmCamID.getValueLength() != 0) {
				camID = String(wmCamID.getValue());
				saveConfigEntry("cam", camID);
			}
			if (wmSecID.getValueLength() != 0) {
				secID = String(wmSecID.getValue());
				saveConfigEntry("sec", secID);
			}
		}

	} else {
		ipAddr = WiFi.localIP();
		wmIsConnected = true;
	}

	if (!wmIsConnected) {
		Serial.println("WiFi connection failed!");
		Serial.println("Only audible alert and auto light is available!");
	} else {
		Serial.println("");
		Serial.print("Connected to ");
		Serial.println(WiFi.SSID());
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
	}

	// Start UDP listener
	startListenToUDPbroadcast();

	Serial.print("Build: ");
	Serial.println(compileDate);

	Serial.print("Device: ");
	Serial.println(DEVICE_ID);
	Serial.println("====================");

	// Initialize temperature sensor
	dht.setup(DHTPIN, DHTesp::DHT11);

	// Get initial temperature
	getTemperature();
	// Start update of weather value every 10 seconds
	getWeatherTimer.attach(10, triggerGetWeather);

	// Set initial time
	if (!tryGetTime(debugOn)) {
		tryGetTime(debugOn); // Failed to get time from NTP server, retry
	}
	if (gotTime) {
		lastKnownYear = year();
	} else {
		lastKnownYear = 0;
	}

	// Start heart beat sending every 1 minute
	heartBeatTimer.attach(60, triggerHeartBeat);

	// Initialize interrupt for PIR signal
	attachInterrupt(pirPort, pirTrigger, CHANGE);

	// Initialize file system.
	if (!SPIFFS.begin())
	{
		sendDebug("Failed to mount file system", OTA_HOST);
		return;
	}

	// Try to get last status & last reboot reason from status.txt
	Serial.println("====================");
	if (!readStatus()) {
		sendDebug("No status file found, try to format the SPIFFS", OTA_HOST);
		if (formatSPIFFS(OTA_HOST)){
			sendDebug("SPIFFS formatted", OTA_HOST);
		} else {
			sendDebug("SPIFFS format failed", OTA_HOST);
		}
		writeRebootReason("Unknown");
		lastRebootReason = "No status file found";
	} else {
		Serial.println("Last reboot because: " + rebootReason);
		lastRebootReason = rebootReason;
	}
	Serial.println("====================");

	// Send Security restart message
	sendAlarm(true);

	// Reset boot status flag
	inSetup = false;

	// Start the tcp socket server to listen on port tcpComPort
	tcpServer.begin();

	if (alarmOn) {
		actLedFlashStart(1);
	} else {
		actLedFlashStop();
	}

	// Initialize DHT sensor
	dht.setup(DHTPIN, DHTesp::DHT11);
	ArduinoOTA.onStart([]() {
		sendDebug("OTA start", OTA_HOST);
		// Safe reboot reason
		writeRebootReason("OTA");
		otaRunning = true;
		// Detach all interrupts and timers
		wdt_disable();
		doubleLedFlashStart(0.1);
		getWeatherTimer.detach();
		alarmTimer.detach();
		heartBeatTimer.detach();
		stopListenToUDPbroadcast();

		WiFiUDP::stopAll();
		WiFiClient::stopAll();
	});

	// Start OTA server.
	ArduinoOTA.setHostname(hostApName);
	ArduinoOTA.begin();

	MDNS.addServiceTxt("arduino", "tcp", "board", "ESP8266");
	MDNS.addServiceTxt("arduino", "tcp", "type", secDevice);
	MDNS.addServiceTxt("arduino", "tcp", "id", String(hostApName));
	MDNS.addServiceTxt("arduino", "tcp", "service", mhcIdTag);
	wdt_enable(WDTO_8S);
}
