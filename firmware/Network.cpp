#include <EtherCard.h>
#include "Arduino.h"
#include "Heartbeat.h"
#include "Network.h"

/**
 * @brief Sets up a TCP connection to our backend server.
 * 
 * @details Attempts to connect to the network via DHCP, uses static if DHCP fails,
 *          pings the backend to verify connectivity and DNS resolution.
 */
void Network::init_wired(void){
	// First check if we can connect to the ethernet module.
	// Make sure any changes to this are manually changed also in
	// Network::Network since sizeof doesn't work because somehow
	// arduino makes this not be global (Wtf?!).
	uint8_t firmware_version = ether.begin(500, mac);
	if (firmware_version == 0){
		Serial.println("Ethernet module is not responding ...");
		Heartbeat::panic();
	} else {
		Serial.print("\nEthernet module firmware: ");
		Serial.println(firmware_version, DEC);
	}

	// Attempt to get a IP address.
	if(!ether.dhcpSetup())
		if(!ether.staticSetup(fallback_ip, fallback_gateway, fallback_dns))
		{
		    Serial.println("Failed setting an IP address ...");
			Heartbeat::panic();
		}

	// Dump info to terminal upon bootup.
	ether.printIp("IP:   ",        ether.myip);
	ether.printIp("GW:   ",        ether.gwip);
	ether.printIp("Mask: ",        ether.netmask);
	ether.printIp("DHCP server: ", ether.dhcpip);

	// Attempt to do a DNS lookup to our backend.
	if(!ether.dnsLookup("weatherspot.us"))
	{
	    Serial.println("Can't do a DNS lookup of weatherspot.us ...");
		Heartbeat::panic();
	} else
		ether.printIp("weatherspot.us IP address: ", ether.hisip);
}

void Network::init_wireless(void){

}
