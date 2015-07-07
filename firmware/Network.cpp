#include <EtherCard.h>
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "Heartbeat.h"
#include "Network.h"
#include "Sensors.h"

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
			    Serial.println("Got a response!");
			    Serial.println(reply);
			  }
		}
	}else if(interface == Wireless){
		/*
		// Make sure we are disconnected first.
		wifi_serial->println("AT+CIPSTATUS");
		wifi_serial->println("AT+CIPCLOSE");

		// Dump anything in the recieve buffers.
		while(wifi_serial->available() > 0)
			volatile char foo = wifi_serial->read();*/

		// Connect to our server.
		wifi_serial->println("AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80");

		// Check if the TCP connection is setup.
		if(wifi_serial->find("OK"))
			Serial.println(F("Wifi TCP connection attempt succesfull."));
		else{
			// Serial.println(F("Wifi TCP connection attempt failed."));
			// wifi_serial->println("AT+CIPCLOSE");
			// return;
		}

		String getStr = "GET /update?key=8ZISX29L64E3ZDHW";
		getStr = getStr + "&field1=" + String(sensor_data.temperature_f);
		getStr = getStr + "&field2=" + String(sensor_data.humidity) + "\r\n";

		String cmd = "AT+CIPSEND=4," + String(getStr.length());
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

	// Set the wifi_serial to timeout after 5s instead of default 1s
	wifi_serial->setTimeout(5000);

	// TODO
	//	Make proper commands with verification as a function.

	// Rest the module so it is in a known state and check if it works.
	wifi_serial->println("AT+RST");
	delay(500);
	if(wifi_serial->find("AT+RST\r\n\r\nOK\r\n"))
		Serial.println(F("Wifi reseting ack recieved ..."));
	else
		Serial.println(F("Wifi reset failed, no reply or garbage returned."));

	// Wait for the reset to finish.
	delay(750);

	if(wifi_serial->find("ready"))
		Serial.println(F("Wifi reset succesfull!"));
	else
		Serial.println(F("Wifi reset failed, no reply or garbage returned."));

	// And the correct wireless mode.
	wifi_serial->println("AT+CWMODE=3");
	if(wifi_serial->find("AT+CWMODE=3\r\n\r\nOK\r\n"))
		Serial.println(F("Wifi mode select was succesfull."));
	else
		Serial.println(F("Wifi mode select failed, no reply or garbage returned."));
	
	// Login to the network.
	// Todo: search for the AP we want to to confirm it exists.
	wifi_serial->println("AT+CWJAP=\"OpenWrt\",\"castle2004\"");

	// For debugging when using a logic sniffer.
	if(debug_wifi){
		pinMode(10, OUTPUT);
		digitalWrite(10, HIGH);
		digitalWrite(10, LOW);
		digitalWrite(10, HIGH);
	}

	// Check to make sure we connected to the wifi network.
	if(wifi_serial->find('OK'))
		Serial.println(F("Wifi connected to OpenWRT network succesfully!"));
	else
		Serial.println(F("Wifi connected to OpenWRT network failed."));

	// Allow multiple TCP connections.
	wifi_serial->println("AT+CIPMUX=1");
	if(wifi_serial->find("AT+CIPMUX=1\r\n\r\nOK\r\n"))
		Serial.println(F("Wifi multiple TCP setting was succesfull."));
	else
		Serial.println(F("Wifi multiple TCP setting failed, no reply or garbage returned."));

	// Check to *really* make sure we connected to the wifi network.
	/*wifi_serial->println("AT+CWJAP=?");
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
