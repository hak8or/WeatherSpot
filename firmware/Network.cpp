#include <SoftwareSerial.h>
#include "Arduino.h"
#include "Heartbeat.h"
#include "Network.h"
#include "Sensors.h"

/**
 * @brief Sends a peice of data over TCP to our backend.
 * @details Does a DNS lookup for our backend, sets up a TCP connection, and sends our data.
 * 
 * @param  An enum for if we want to send our data over Wireless or Wired.
 */
void Network::send_packet(Sensor_data sensor_data){
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

/**
 * @brief Connects to a wireless network using the ESP module.
 * 
 * @param SSID      The SSID of the wireless network we want to connect to.
 * @param password  The password of the wireless network we want to connect to.
 * 
 * @return True if we were able to connect to the network and get an IP, false if otherwise.
 */
bool Network::init_wireless(const String SSID, const String password){
	// Generate the member object
	// Pin D3 -> RX
	// Pin D4 -> TX
	wifi_serial = new SoftwareSerial(3, 4);
	wifi_serial->begin(9600);

	// Rest the module so it is in a known state and check if it works.
	if (send_command("AT+RST", "OK", 2, 1500))
		Serial.println(F("Wifi reseting ack recieved."));
	else{
		Serial.println(F("Wifi reseting failed."));
		return false;
	}
	
	// Delay a bit in case we get a "WIFI Disconnect" messege afterwards.
	delay(60);

	// And the correct wireless mode.
	if (send_command("AT+CWMODE=3", "OK", 2, 1500))
		Serial.println(F("Wifi mode select was succesfull."));
	else{
		Serial.println(F("Wifi mode select failed, no reply or garbage returned."));
		return false;
	}

	// Delay a bit since for some reason the CWMODE seems to return some gunk at the end.
	delay(60);
	
	// Login to the network.
	// Todo: search for the AP we want to to confirm it exists.
	String command = "AT+CWJAP_CUR=\"" + SSID + "\",\"" + password + "\"";
	if (send_command(command, "WIFI GOT IP", 11, 5000))
		Serial.println(F("Wifi connected to OpenWRT network succesfully!"));
	else{
		Serial.println(F("Wifi connected to OpenWRT network failed."));
		return false;
	}

	// Allow multiple TCP connections.
	if (send_command("AT+CIPMUX=1", "OK", 2, 1500))
		Serial.println(F("Wifi multiple TCP setting was succesfull."));
	else{
		Serial.println(F("Wifi multiple TCP setting failed, no reply or garbage returned."));
		return false;
	}

	// Do a ping request to make sure the server is up.
	if (send_command("AT+PING=\"www.weatherspot.us\"", "OK", 2, 1500))
		Serial.println(F("Wifi ping test to weatherspot.us OK"));
	else{
		Serial.println(F("Wifi ping test to weatherspot.us Failed."));
		return false;
	}

	// Dumps the wifi info to the serial port.
	get_wifi_info();

	return true;
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
	uint32_t start_time = millis();

	// Mirror output of a "get network info" command.
	wifi_serial->println("AT+CIFSR");
	while ((millis() - start_time) < 500)
		if (wifi_serial->available())
	    	Serial.write(wifi_serial->read());
}

/**
 * @brief Blocks till either timeout or we find the requested reply on the wifi serial port.
 * 
 * @param reply The reply we are waiting for.
 * @param milliseconds The timeout for how long we are willing to wait.
 * 
 * @return If we found the reply within the timeout.
 */
bool Network::find(const String reply, const uint8_t reply_length, const uint16_t milliseconds){
	uint32_t start_time = millis();
	uint8_t  current_reply_index = 0;

	// Make sure we don't spend forever waiting for the char sequence.
	while((millis() - start_time) < milliseconds){
		// Serial.println("Start time: " + String(start_time) + " Millis: " + String(millis()));
		// Serial.println("Time passed: " + String(millis() - start_time) + " Timeout: " + String(milliseconds));
		if (wifi_serial->available() > 0) {
			// Check if the reply on our serial port is the first char of reply.
			char reply_char = wifi_serial->read();
			if (reply_char == reply[current_reply_index]){
				// Check if we succesfully compared the entire reply.
				if ((reply_length - 1) == current_reply_index)
					return true;
				else 
					current_reply_index++;
			}
			else{
				// Reset the index back to zero so we can start searching again.
				current_reply_index = 0;

				// Increment buffer index.
				reply_buffer_current_index++;

				// Empty our buffer if it is full.
				if (reply_buffer_current_index == reply_buffer_size){
					reply_buffer_current_index = 0;

					for (int index = 0; index < reply_buffer_size; index++)
						reply_buffer[index] = ' ';
				}

				// Put the char in our buffer.
				reply_buffer[reply_buffer_current_index] = reply_char;
			}
		}
	}

	// If we timed out, then it's considerd we didn't find our char array.
	return false;
}

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
bool Network::send_command(const String command, const String reply, const uint8_t reply_length, const uint16_t timeout){
	// Send the command.
	wifi_serial->println(command);

	// Check if we got the expected reply back.
	if(!find(reply, reply_length, timeout)){
		Serial.print("Command failed: ");
		Serial.println(command);

		// Dump the buffer.
		Serial.println(F("Reply ======= start"));
		for (int i = 0; i < reply_buffer_current_index; i++)
			Serial.print(reply_buffer[i]);
		Serial.println(F("Reply ======= end"));

		// Empty our buffer.
		reply_buffer_current_index = 0;
		for (int index = 0; index < reply_buffer_size; index++)
			reply_buffer[index] = ' ';

		return false;
	} else
		return true;
}