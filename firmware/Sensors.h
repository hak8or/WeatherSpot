#include <DHT.h>
#include "SparkFunMPL3115A2.h"

#pragma once

/**
 * API for all our sensor needs.
 */
class Sensors
{
public:
	/**
	 * @brief Just makes pointers point to null.
	 */
	Sensors(void);

	/**
	 * @brief Just configures what pin our DH11 module uses for data.
	 * 
	 * @param data_pin [Pin used for data communication.
	 */
	void init_DH11(const uint8_t data_pin);

	/**
	 * @brief Sets up the MPL3115A2 module for pressure readings.
	 */
	void init_MPL3115A2(void);

	/**
	 * Just a struct to hold all our sensor data.
	 */
	struct Sensor_data {
		float humidity;
		float temperature_f;
		uint16_t light;
	    float pressure;
	};

	/**
	 * @brief Reads sensor data.
	 * 
	 * @return Data from the reading in the form of a sensor_data struct.
	 */
	Sensor_data read_sensors(void);

	/**
	 * @brief Prints the contents of sensor data nicely over uart.
	 * 
	 * @param sensor_data The sensor data struct we will be displaying.
	 */
	void print(const Sensor_data sensor_data);

private:
	DHT *dht_module;
	MPL3115A2 *pressure_sensor;
};


