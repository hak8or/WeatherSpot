#include "Heartbeat.h"
#include "Sensors.h"
#include <DHT.h>
#include "SparkFunMPL3115A2.h"

Sensors::Sensors(void){
	// This is arduino, and changing the compilation flags 
	// on a per project basis is nonexistant here, woooo!
	// Therefore, we can't use C++11, and nullptr.
	this->dht_module = NULL;
	this->pressure_sensor = NULL;
}

/**
 * @brief Just configures what pin our DH11 module uses for data.
 * 
 * @param data_pin Pin used for data communication.
 */
void Sensors::init_DH11(const uint8_t data_pin){
	this->data_pin = data_pin;
	dht_module = new DHT(this->data_pin, DHT11, 2);
	dht_module->begin();
}

/**
 * @brief Sets up the MPL3115A2 module for pressure readings.
 */
void Sensors::init_MPL3115A2(void){
	this->pressure_sensor = new MPL3115A2();

	// initiate the pressure sensor
	this->pressure_sensor->begin();

	// configure the pressure sensor
	pressure_sensor->setModeBarometer();
	pressure_sensor->setOversampleRate(7);
	pressure_sensor->enableEventFlags();
}

/**
 * @brief Reads sensor data.
 * 
 * @return Data from the reading in the form of a sensor_data struct.
 */
Sensor_data Sensors::read_sensors(void){
	Sensor_data sensor_data;

	// Check if this class was properly initiated.
	if (dht_module == NULL || pressure_sensor == NULL) {
		sensor_data.temperature_f = 0.0;
		sensor_data.humidity = 0.0;
		sensor_data.light = 0;
		sensor_data.pressure = 0.0;

		Heartbeat::panic();
		Serial.println(F("Attempt to read sensors before initilization."));

		return sensor_data;
	}

	// Read all our sensors.
	sensor_data.temperature_f = pressure_sensor->readTempF();
	sensor_data.humidity = dht_module->readHumidity();
	sensor_data.light = analogRead(A0);
	sensor_data.pressure = pressure_sensor->readPressure();

	return sensor_data;
}

/**
 * @brief Prints the contents of sensor data nicely over uart.
 * 
 * @param sensor_data The sensor data struct we will be displaying.
 */
void Sensors::print(const Sensor_data sensor_data){
	Serial.print(F("-  Humidity: "));
	Serial.println(sensor_data.humidity);
	Serial.print(F("-  Temperature: "));
	Serial.println(sensor_data.temperature_f);
	Serial.print(F("-  Light: "));
	Serial.println(sensor_data.light);
	Serial.print(F("-  Pressure: "));
	Serial.println(sensor_data.pressure);
}
