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
#include "TimerOne.h"
#include "SparkFunMPL3115A2.h"

// Global because arduno says we need setup and loop functions.
Network network;
Sensors sensors;

// Defines for our wifi credentials.
#define SSID "OpenWrt"
#define password "castle2004"

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

	// Read our s.
	Sensor_data sensor_data = sensors.read_sensors();

	// Dump the sensor data to our screen.
	sensors.print(sensor_data);

	// try to send 5 times, if 5 fail restart the module
	for(int i = 0; i < 5; i++){
		Serial.print("Trying to send data: ");
		Serial.println(i);

		if(!network.send_packet(sensor_data)){
			if(i == 2){
				// something went wrong with the connection - try to reconnect
				// Startup our network.
				while (!network.init_wireless(SSID, password)){
					Serial.println(F("Failed connecting to wireless network, retying in 3 seconds."));
					Heartbeat::panic();
					delay(3000);
				}

				delay(3000);

				// start panic mode
				Heartbeat::panic();
			}
		} else {
			// Make our heartbeat LED blink at a normal rate again if we were panicking earlier.
			Heartbeat::start();
			break;
		}
	}

	Serial.println(F("Waiting 1 min... - sleep time"));
	delay(60000); // wait

	// kill the led before taking measurements
	Heartbeat::stop();
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
