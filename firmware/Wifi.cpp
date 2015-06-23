#include "Wifi.h"
#include <SoftwareSerial.h>
#include "Arduino.h" // Needed for types.

/**
 * @brief Makes a serial connection to wifi module.
 * 
 * @param RX_wifi_pin Recieve pin relative to arduino for serial comms.
 * @param RX_wifi_pin Transmit pin relative to arduino for serial comms.
 */
Wifi::Wifi(const uint8_t RX_wifi_pin, const uint8_t TX_wifi_pin){
	// Generate a software serial object using unused pins.
	wifi_serial = new SoftwareSerial(RX_wifi_pin, TX_wifi_pin);

	// And open up that serial port.
	wifi_serial->begin(9600);
}

/**
 * @brief Used to send and recieve raw serial commands.
 * 
 * @details Enters a infinite loop that exits when an "exit_loop" gets hit.
 * Used to send and recieve raw data to and from the Wifi module for debugging and whatnot.
 */
void Wifi::raw_commands_mode(void){
	Serial.println("In raw commands mode.");
	
	while(true){
		// debug_serial -> wifi_serial
		while (Serial.available())
			wifi_serial->write(Serial.read());

		// wifi_serial -> debug_serial
		while (wifi_serial->available())
			Serial.write(wifi_serial->read());
	}
}

