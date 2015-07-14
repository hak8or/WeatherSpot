#include <EtherCard.h>
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "Sensors.h"

#ifndef Network_h
#define Network_h

class Network
{
public:
	/**
	 * @brief Sets up a TCP connection to our backend server.
	 * 
	 * @details Attempts to connect to the network via DHCP, uses static if DHCP fails,
	 *          pings the backend to verify connectivity and DNS resolution.
	 */
	void init_wired(void);

	enum Interface {
		Wired,
		Wireless
	};


	void init_wireless(void);

	/**
	 * @brief Sends a peice of data over TCP to our backend.
	 * @details Does a DNS lookup for our backend, sets up a TCP connection, and sends our data.
	 * 
	 * @param  An enum for if we want to send our data over Wireless or Wired.
	 */
	void send_packet(Network::Interface interface, const Sensor_data sensor_data);

	/**
	 * @brief Turns the arduino in a proxy for serial commands from the PC to wifi module.
	 * @details Not really working yet, sad face. :(
	 */
	void serial_proxy_mode(void);
private:
#ifdef wired_network_enable
	// Hardcoded mac address.
	const byte mac[6]        = { 0x00, 0x04, 0xA3, 0x21, 0xCA, 0x38 };

	// Fallback info if the DHCP request fails.
	const uint8_t fallback_ip[4]      = { 192, 168, 1, 250 };  // The fallback board address.
	const uint8_t fallback_dns[4]     = { 192, 168, 1, 1 };    // The DNS server address.
	const uint8_t fallback_gateway[4] = { 192, 168, 1, 1 };    // The gateway router address.
	const uint8_t fallback_subnet[4]  = { 255, 255, 255, 0 };  // The subnet mask.
#endif

	// Reply buffer for communication from our serial based modules.
	// #define reply_buffer_size 25;
	// char reply_buffer[reply_buffer_size];
	// uint8_t reply_buffer_current_index = 0;

	// For talking to the wifi module.
	SoftwareSerial *wifi_serial;

	/**
	 * @brief Blocks till either timeout or we find the requested reply.
	 * 
	 * @param reply The reply we are waiting for.
	 * @param milliseconds The timeout for how long we are willing to wait.
	 * 
	 * @return If we found the reply within the timeout.
	 */
	bool find(const char reply[], const uint8_t reply_length, const uint16_t milliseconds);

	/**
	 * @brief Dumps wifi network information over to the pc serial.
	 */
	void get_wifi_info(void);
};
#endif
