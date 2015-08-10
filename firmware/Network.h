#include <SoftwareSerial.h>
#include "Arduino.h"
#include "Sensors.h"

#pragma once

/**
 * API for all our network ncesessities.
 */
class Network
{
public:
	/**
	 * @brief Connects to a wireless network using the ESP module.
	 * 
	 * @param SSID      The SSID of the wireless network we want to connect to.
	 * @param password  The password of the wireless network we want to connect to.
	 * 
	 * @return True if we were able to connect to the network and get an IP, false if otherwise.
	 */
	bool init_wireless(const String SSID, const String password);

	/**
	 * @brief Sends a peice of data over TCP to our backend.
	 * @details Does a DNS lookup for our backend, sets up a TCP connection, and sends our data.
	 * 
	 * @param  An enum for if we want to send our data over Wireless or Wired.
	 * 
	 * @return True if we were able succesfully send the POST request, false if otherwise.
	 */
	bool send_packet(const Sensors::Sensor_data sensor_data, const String series);

	/**
	 * @brief Sends a command to the WIFI module.
	 * @details Sends a command to the wifi module, waits for a verification before a timeout.
	 * 
	 * @param command The command we will be sending.
	 * @param reply The reply we expect back.
	 * @param reply_length The length of the reply in chars.
	 * @param timeout How long we are willing to wait for the reply in milliseconds.
	 * 
	 * @return True if command and verification within timeout was correct, false if otherwise.
	 */
	bool send_command(const String command, const String reply, const uint8_t reply_length, const uint16_t timeout);
private:
	// Reply buffer for communication from our serial based modules.
	#define reply_buffer_size 100
	char reply_buffer[reply_buffer_size];
	uint8_t reply_buffer_current_index = 0;

	// For talking to the wifi module.
	SoftwareSerial *wifi_serial;

	/**
	 * @brief Blocks till either timeout or we find the requested reply.
	 * 
	 * @param reply The reply we are waiting for.
	 * @param reply_length Number of chars of our reply.
	 * @param milliseconds The timeout for how long we are willing to wait.
	 * 
	 * @return True if we found the reply within the timeout.
	 */
	bool find(const String reply, const uint8_t reply_length, const uint16_t milliseconds);

	/**
	 * @brief Dumps wifi network information over to the pc serial.
	 */
	void get_wifi_info(void);
};
