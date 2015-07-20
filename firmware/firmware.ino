#include "Arduino.h"
#include "Heartbeat.h"
#include "TimerOne.h"
#include "Sensors.h"
#include "Network.h"

#include <EtherCard.h>
#include <DHT.h>

// Gotta include this because the arduino IDE does some magic including nonsense.
#include <SoftwareSerial.h>

#include "Sensors.h"
#include "Heartbeat.h"
#include "Network.h"

void setup(){
	// Start up the serial communication.
	// Find out why this cuts off the next serial print.
	Serial.begin(115200);
	for(int i = 0; i < 3; i++)
		Serial.print(F("==============="));

	Serial.println(F("\r\nWeatherSpot firmware Uno-V0.1"));

	for(int i = 0; i < 3; i++)
		Serial.print(F("==============="));
	
	// Startup a heartbeat LED.
	Heartbeat::start();

	Sensors sensors;
	sensors.init_DH11(2);

	// Startup our network.
	Network network;
	network.init_wireless("OpenWrt", "castle2004");

	// And do our loop for sending and recieving data.
	while(true){
		Sensor_data sensor_data = sensors.read_sensors();
		Serial.print(F("Sensor humidity: "));
		Serial.println(sensor_data.humidity);
		Serial.print(F("Sensor temperature: "));
		Serial.println(sensor_data.temperature_f);
		Serial.print(F("Sensor light reading: "));
		Serial.println(sensor_data.light);

		// network.send_packet(sensor_data);

		// Serial.println(F("Waiting 20 seconds ..."));
		// delay(20000);
		delay(500);
	}
}

void loop(){

}
