#include <EtherCard.h>
#include "Arduino.h"
#include "Heartbeat.h"
#include "Network.h"

// The arduino IDE sucks and does not enable C++11 by default, and requires changing
// a file within the arduino installation directory to do so, screw that. This is why
// these are all the way out here and not even in a constructor.
byte Network::mac[6] = { 0x00, 0x04, 0xA3, 0x21, 0xCA, 0x38 };

// Fallback info if the DHCP request fails.
uint8_t Network::fallback_ip[4]      = { 192, 168, 1, 250 };  // The fallback board address.
uint8_t Network::fallback_dns[4]     = { 192, 168, 1, 1 };    // The DNS server address.
uint8_t Network::fallback_gateway[4] = { 192, 168, 1, 1 };    // The gateway router address.
uint8_t Network::fallback_subnet[4]  = { 255, 255, 255, 0 };  // The subnet mask.*/

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
		if(!ether.staticSetup(fallback_ip, fallback_gateway, fallback_dns)) {
		    Serial.println("Failed setting an IP address ...");
			Heartbeat::panic();
		}

	// Dump info to terminal upon bootup.
	ether.printIp("IP:   ",        ether.myip);
	ether.printIp("GW:   ",        ether.gwip);
	ether.printIp("Mask: ",        ether.netmask);
	ether.printIp("DHCP server: ", ether.dhcpip);

	// Attempt to do a DNS lookup to our backend.
	if(!ether.dnsLookup("thingspeak.com"))
	{
	    Serial.println("Can't do a DNS lookup of thingspeak.com ...");
		Heartbeat::panic();
	} else
		ether.printIp("thingspeak.com IP address: ", ether.hisip);
}

/**
 * @brief Sends a peice of data over TCP to our backend.
 * @details Does a DNS lookup for our backend, sets up a TCP connection, and sends our data.
 * 
 * @param  An enum for if we want to send our data over Wireless or Wired.
 */
void Network::send_packet(Network::Interface interface){
	if (interface == Wired) {
		// https://api.thingspeak.com/update?key=8ZISX29L64E3ZDHW&field1=0

		Stash stash;
		byte sd = stash.create();
		stash.print("field1=3");
		stash.save();

		// generate the header with payload - note that the stash size is used,
	    // and that a "stash descriptor" is passed in as argument using "$H" 
	    Stash::prepare(
	    	PSTR("POST /update HTTP/1.1\n"
	    		 "Host: api.thingspeak.com\n"
	             "Connection: close\n"
	             "X-THINGSPEAKAPIKEY: 8ZISX29L64E3ZDHW\n"
	             "Content-Type: application/x-www-form-urlencoded\n"
	             "Content-Length: $D\n\n"
	             "$H"),
	    	stash.size(), sd);

		// send the packet - this also releases all stash buffers once done
	    byte session = ether.tcpSend();

		Serial.println("Send TCP packet to thingspeak!");

		while(1){
			ether.packetLoop(ether.packetReceive());

			  const char* reply = ether.tcpReply(session);
			  if (reply != 0) {
			    Serial.println("Got a response!");
			    Serial.println(reply);
			  }
		}
	}
}

void Network::init_wireless(void){
	// Login to the network.
	wifi_serial.print('AT+CWJAP="OpenWrt","Castle2004"\r\n');



}
