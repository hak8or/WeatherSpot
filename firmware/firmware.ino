// Gotta include this because the arduino IDE does some magic including nonsense.
#include <SoftwareSerial.h>
#include <Wire.h>

// For the WDT sleep functionality.
#include <avr/sleep.h>
#include <avr/wdt.h>

// Includes relevant to our project.
#include "Sensors.h"
#include "Heartbeat.h"
#include "Network.h"
#include "DHT.h"
#include <TimerOne.h>
#include "SparkFunMPL3115A2.h"

// Global because arduno says we need setup and loop functions.
Network network;
Sensors sensors;

// Defines for our wifi credentials.
#define SSID "OpenWrt"
#define password "castle2004"

// Defines what location (Series) we will be sending our data too.
#define location "Queens"

/**
 * @brief The main setup loop for our sensors and WIFI.
 */
void setup(){
	// Start up the serial communication.
	Serial.begin(115200);

	// Dump a decent header to our console.
	Serial.println(F("============================"));
	Serial.println(F("\t WeatherSpot firmware Uno-V1.0 RC1"));
	Serial.println(F("============================"));

	// Startup our network.
	while (!network.init_wireless(SSID, password)){
		Serial.println(F("Failed connecting to wireless network, retying in 3 seconds."));
		Heartbeat::panic();
		delay(3000);
	}

	// Startup a heartbeat LED.
	Heartbeat::start();

	// Extra delay for extra safety.
	delay(3000);

	// initiate DH11 sesnor at pin 2
	sensors.init_DH11(2);

	// Setup the pressure sensor.
	sensors.init_MPL3115A2();
}

/**
 * @brief Main loop of our application to read sensors and send data to server.
 */
void loop(){ 
	// time stamp each iteration
	Serial.println("\n\n************ loop() starting at millis: " + String(millis()) + " ************");

	// Make sure the LED doesn't interefere with light sensing.
	Heartbeat::stop();

	// Read our sensors.
	Sensors::Sensor_data sensor_data = sensors.read_sensors();

	// Dump the sensor data to our screen.
	sensors.print(sensor_data);

	// try to send 5 times, if 5 fail restart the module
	for(int attempt = 0; attempt < 5; attempt++){
		Serial.println("======== Data transmit attempt: [" + String(attempt) + "]/5");

		// Make LED panic if sending failed, attempt to reconnect to wifi on 2nd try.
		if(!network.send_packet(sensor_data, location)){
			// start panic mode
			Heartbeat::panic();

			// Reconnect to the WIFI, seems to make the WIFI module happier on the 2nd try.
			if(attempt == 2){
				while (!network.init_wireless(SSID, password)){
					Serial.println(F("Failed connecting to wireless network, retying in 3 seconds."));
					delay(3000);
				}

				// Extra delay for extra safety.
				delay(3000);
			}
		} else {
			// Make our heartbeat LED blink at a normal rate again if we were panicking earlier.
			Heartbeat::start();

			Serial.println(F("Sensor data was sent to the server succesfully!"));

			break;
		}
	}

	Serial.println(F("Waiting 1 min ...."));
	delay(60000);
}

/**
 * @brief Disable WDT.
 */
ISR(WDT_vect){
	wdt_disable();
}

/**
 * @brief Enables WDT to fire after a certain time.
 * 
 * @param time_to_sleep How long till the WDT fires again.
 */
void EnableWatchDog(const byte time_to_sleep){
	MCUSR = 0;
	WDTCSR |= 0b00011000;
	WDTCSR =  0b01000000 | time_to_sleep;

	wdt_reset();

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// sleep until we are interrupted
	sleep_mode();
}
