#include <EtherCard.h>
#include "Arduino.h"

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
	void init_wireless(void);

	// Used to tell apart over what interface to send to.
	enum Interface { Wireless, Wired };

	/**
	 * @brief Sends a peice of data over TCP to our backend.
	 * @details Does a DNS lookup for our backend, sets up a TCP connection, and sends our data.
	 * 
	 * @param  An enum for if we want to send our data over Wireless or Wired.
	 */
	void send_packet(Network::Interface interface);
private:
	// The arduino IDE sucks and does not enable C++11 by default, and requires changing
	// a file within the arduino installation directory to do so, screw that. This is why
	// these are not const's but instead must be written to in outside.
	static byte mac[6];

	// Fallback info if the DHCP request fails.
	static uint8_t fallback_ip[4];      // The fallback board address.
	static uint8_t fallback_dns[4];     // The DNS server address.
	static uint8_t fallback_gateway[4]; // The gateway router address.
	static uint8_t fallback_subnet[4];  // The subnet mask.
};

#endif
