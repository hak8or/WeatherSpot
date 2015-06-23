#include <SoftwareSerial.h>
#include "Arduino.h" // Needed for types.

class Wifi
{
public:
	/**
	 * @brief Makes a serial connection to wifi module.
	 * 
	 * @param RX_wifi_pin Recieve pin relative to arduino for serial comms.
	 * @param RX_wifi_pin Transmit pin relative to arduino for serial comms.
	 */
	Wifi(const uint8_t RX_wifi_pin = 9, const uint8_t TX_wifi_pin = 10);

	/**
	 * @brief Used to send and recieve raw serial commands.
	 * 
	 * @details Enters a infinite loop that exits when an "exit_loop" gets hit.
	 * Used to send and recieve raw data to and from the Wifi module for debugging and whatnot.
	 */
	void raw_commands_mode(void);
private:
	// Since Softwareserial is wierd, this is just a pointer while
	// we initilize it and generate the object in wifi's constructor.
	SoftwareSerial *wifi_serial;
};

