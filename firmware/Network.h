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
private:
	// Hardcoded mac address.
	const byte mac[6]        = { 0x00, 0x04, 0xA3, 0x21, 0xCA, 0x38 };

	// Fallback info if the DHCP request fails.
	const uint8_t fallback_ip[4]      = { 192, 168, 1, 250 };  // The fallback board address.
	const uint8_t fallback_dns[4]     = { 192, 168, 1, 1 };    // The DNS server address.
	const uint8_t fallback_gateway[4] = { 192, 168, 1, 1 };    // The gateway router address.
	const uint8_t fallback_subnet[4]  = { 255, 255, 255, 0 };  // The subnet mask.
};

#endif
