#include <EtherCard.h>
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "Heartbeat.h"
#include "Network.h"
#include "Sensors.h"

// Uncomment to enable wired network. Done using define instead of just a normal
// if since it seems we can't optimize it away.
// #define wired_network_enable

/**
 * @brief Sets up a TCP connection to our backend server.
 * 
 * @details Attempts to connect to the network via DHCP, uses static if DHCP fails,
 *          pings the backend to verify connectivity and DNS resolution.
 */
void Network::init_wired(void){
	Serial.println(F("Wired Ethernet is not yet tested/implimented!"));
#ifdef wired_network_enable
	// First check if we can connect to the ethernet module.
	// Make sure any changes to this are manually changed also in
	// Network::Network since sizeof doesn't work because somehow
	// arduino makes this not be global (Wtf?!).
	uint8_t firmware_version = ether.begin(500, mac);
	if (firmware_version == 0){
		Serial.println(F("Ethernet module is not responding ..."));
		Heartbeat::panic();
	} else {
		Serial.print(F("\nEthernet module firmware: "));
		Serial.println(firmware_version, DEC);
	}

	// Attempt to get a IP address.
	if(!ether.dhcpSetup())
		if(!ether.staticSetup(fallback_ip, fallback_gateway, fallback_dns)) {
		    Serial.println(F("Failed setting an IP address ..."));
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
	    Serial.println(F("Can't do a DNS lookup of thingspeak.com ..."));
		Heartbeat::panic();
	} else
		ether.printIp("thingspeak.com IP address: ", ether.hisip);
#endif
}

/**
 * @brief Sends a peice of data over TCP to our backend.
 * @details Does a DNS lookup for our backend, sets up a TCP connection, and sends our data.
 * 
 * @param  An enum for if we want to send our data over Wireless or Wired.
 */
void Network::send_packet(Network::Interface interface, Sensor_data sensor_data){
	if (interface == Wired) {
		Serial.println(F("Wired Ethernet is not yet tested/implimented!"));

#ifdef wired_network_enable
		Stash stash;
		byte sd = stash.create();
		stash.print("field1=3");
		stash.save();

		// https://api.thingspeak.com/update?key=8ZISX29L64E3ZDHW&field1=0
		// Generate the header with payload - note that the stash size is used,
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

		Serial.println(F("Send TCP packet to thingspeak!"));

		while(1){
			ether.packetLoop(ether.packetReceive());

			  const char* reply = ether.tcpReply(session);
			  if (reply != 0) {
			    Serial.println(F("Got a response!"));
			    Serial.println(reply);
			  }
		}
#endif
	}else if(interface == Wireless){
		/*
		// Make sure we are disconnected first.
		wifi_serial->println("AT+CIPSTATUS");
		wifi_serial->println("AT+CIPCLOSE");

		// Dump anything in the recieve buffers.
		while(wifi_serial->available() > 0)
			volatile char foo = wifi_serial->read();*/

		// Connect to our server.
		wifi_serial->println("AT+CIPSTART=4,\"TCP\",\"104.131.85.242\",80");

		// Check if the TCP connection is setup.
		if(wifi_serial->find("OK"))
			Serial.println(F("Wifi TCP connection attempt succesfull."));
		else{
			// Serial.println(F("Wifi TCP connection attempt failed."));
			// wifi_serial->println("AT+CIPCLOSE");
			// return;
		}

		String getStr = "POST /db/query.php?";
		getStr = getStr + "series=Downtown";
		getStr = getStr + "&temperature=" + String(sensor_data.temperature_f);
		getStr = getStr + "&humidity=" + String(sensor_data.humidity);
		getStr = getStr + "&pressure=65";
		getStr = getStr + "&lighting=65";
		getStr = getStr + "\r\n\r\n";

		String cmd = "AT+CIPSEND=4," + String(getStr.length());

		Serial.print(F("Sending:  \n\t"));
		Serial.print(cmd);
		Serial.print(F("\n\t"));
		Serial.print(getStr);
		Serial.print(F("\n"));

		wifi_serial->println(cmd);

		// Send a packet if we can.
		if(wifi_serial->find(">"))
			wifi_serial->print(getStr);
		else{
			Serial.println(F("Wifi module didn't query what http contents to send, error."));
			// wifi_serial->println("AT+CIPCLOSE");
			// return;
		}

		// Check if the packet was sent.
		if(wifi_serial->find("OK"))
			Serial.println(F("Wifi packet was sent."));
		else{
			// Wifi return is not as expected for success:
			// 		+IPD,4,1:8
			// 		OK
			// Serial.println(F("Wifi packet was not sent!"));
		}

		// Close the connection.
		wifi_serial->println("AT+CIPCLOSE");
	}
}

/**
 * @brief Setup the wireless module and connect to the wifi network.
 */
void Network::init_wireless(void){
	// Generate the member object
	// Pin D5 -> TX (Make sure to have a 5v -> 3.3v resistor divider here!!!)
	// Pin D6 -> RX
	wifi_serial = new SoftwareSerial(6, 5);
	wifi_serial->begin(9600);

	// Set the wifi_serial to timeout after 10s instead of default 1s
	wifi_serial->setTimeout(10000);

	// TODO
	//	Make proper commands with verification as a function.

	// Rest the module so it is in a known state and check if it works.
	wifi_serial->println("AT+RST");
	if(find("OK", 15000))
		Serial.println(F("Wifi reseting ack recieved ..."));
	else
		Serial.println(F("Wifi reset failed, no reply or garbage returned."));

	return;

	if(wifi_serial->find("ready"))
		Serial.println(F("Wifi reset succesfull!"));
	else
		Serial.println(F("Wifi reset failed, no reply or garbage returned."));

	// And the correct wireless mode.
	wifi_serial->print("AT+CWMODE=3\r\n");
	if(wifi_serial->find("OK"))
		Serial.println(F("Wifi mode select was succesfull."));
	else
		Serial.println(F("Wifi mode select failed, no reply or garbage returned."));
	
	// Login to the network.
	// Todo: search for the AP we want to to confirm it exists.
	wifi_serial->print("AT+CWJAP=\"OpenWrt\",\"castle2004\"\r\n");

	// Check to make sure we connected to the wifi network.
	if(wifi_serial->find('WIFI GOT IP'))
		Serial.println(F("Wifi connected to OpenWRT network succesfully!"));
	else
		Serial.println(F("Wifi connected to OpenWRT network failed."));

	// Allow multiple TCP connections.
	wifi_serial->print("AT+CIPMUX=1");
	if(wifi_serial->find("AT+CIPMUX=1\r\n\r\nOK\r\n"))
		Serial.println(F("Wifi multiple TCP setting was succesfull."));
	else
		Serial.println(F("Wifi multiple TCP setting failed, no reply or garbage returned."));


	// Make the TCP timout after 5 seconds, done in order to help prevent the "busy s..." error.	
	wifi_serial->print("AT+CIPSTO=5");
	if(wifi_serial->find("AT+CIPSTO=5\r\n\r\nOK\r\n"))
		Serial.println(F("Wifi TCP timeout setting was succesfull."));
	else
		Serial.println(F("Wifi TCP timeout setting failed, no reply or garbage returned."));

	// Check to *really* make sure we connected to the wifi network.
	/*wifi_serial->print("AT+CWJAP=?\r\n");
	if(wifi_serial->find('OK'))
		Serial.println(F("Wifi network connectivity verification succesfully!"));
	else
		Serial.println(F("Wifi network connectivity verification failed!"));*/

	// Dumps the wifi info to the serial port.
	// get_wifi_info();
}

/**
 * @brief Turns the arduino in a proxy for serial commands from the PC to wifi module.
 * @details Not really working yet, sad face. :(
 */
void Network::serial_proxy_mode(void){
	Serial.println(F("Entering Serial proxy mode."));
	while(true){
		while (wifi_serial->available() > 0)
			Serial.write(wifi_serial->read());
		while (Serial.available() > 0){
			wifi_serial->write(Serial.read());
		}
	}
}

/**
 * @brief Dumps wifi network information over to the pc serial.
 */
 void Network::get_wifi_info(void){
	Serial.println(F("\r\n============== Wifi Network info =============="));

	// Mirror output of a "get network info" command.
	wifi_serial->println("AT+CIFSR");
	while (wifi_serial->available())
    	Serial.write(wifi_serial->read());
}

/**
 * @brief Blocks till either timeout or we find the requested reply.
 * 
 * @param reply The reply we are waiting for.
 * @param milliseconds The timeout for how long we are willing to wait.
 * 
 * @return If we found the reply within the timeout.
 */
bool Network::find(char reply[], uint16_t milliseconds){
	uint32_t start_time = millis();

	while(start_time - millis() > milliseconds){
		// Dump any chars we find to the buffer.
		while (wifi_serial->available() > 0){
			this->reply_buffer[this->buffer_filled] = 
		}

	}
	

	return false;
}