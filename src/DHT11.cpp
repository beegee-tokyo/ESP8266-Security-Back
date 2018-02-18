#include "Setup.h"

void getTemperature() {
	// Reading temperature for humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
	wdt_reset();
	TempAndHumidity lastValues = dht.getTempAndHumidity();
	wdt_reset();

	if (dht.getStatus() != 0) {
		String dbgMsg = "Error reading from DHT11" + String(dht.getStatusString());
		sendDebug(dbgMsg, OTA_HOST);
		return;
	}

	/******************************************************* */
	/* Trying to calibrate the humidity values               */
	/******************************************************* */
	humidValue = lastValues.humidity * 4.2;
	tempValue = lastValues.temperature;
}
