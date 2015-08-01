#include <DHT.h>

#ifndef Sensors_h
#define Sensors_h

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
	void init_DH11(uint8_t data_pin);

	/**
	 * @brief Reads sensor data.
	 * 
	 * @return Data from the reading in the form of a sensor_data struct.
	 */
	struct Sensor_data read_sensors(void);

private:
	uint8_t data_pin;
	DHT *dht_module;
};

// Struct to hold our weather data.
struct Sensor_data {
	float humidity;
	float temperature_f;
	uint16_t light;
  float pressure;
};

#endif