#include "Heartbeat.h"
#include "Sensors.h"
#include <DHT.h>

Sensors::Sensors(void){
	// This is arduino, and changing the compilation flags 
	// on a per project basis is nonexistant here, woooo!
	// Therefore, we can't use C++11, and nullptr.
	this->dht_module = NULL;
}

/**
 * @brief Just configures what pin our DH11 module uses for data.
 * 
 * @param data_pin Pin used for data communication.
 */
void Sensors::init_DH11(uint8_t data_pin){
	this->data_pin = data_pin;

	dht_module = new DHT(this->data_pin, DHT11);

	dht_module->begin();

	Serial.println("Setting up DHT11.");
}

/**
 * @brief Reads sensor data.
 * 
 * @return Data from the reading in the form of a sensor_data struct.
 */
Sensor_data Sensors::read_sensors(void){
	Sensor_data sensor_data;

	// Check if this class was properly initiated.
	if (dht_module == NULL) {
		sensor_data.temperature_f = 0.0;
  		sensor_data.humidity = 0.0;

  		Heartbeat::panic();
  		Serial.println("Attempt to read DH11 before initilized.");

  		return sensor_data;
	}

  // True means Fahrenheit. Yeah, this library is trash.
  sensor_data.temperature_f = dht_module->readTemperature(true);
  sensor_data.humidity = dht_module->readHumidity();

  return sensor_data;
}