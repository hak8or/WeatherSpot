#include "Arduino.h"
#include "Heartbeat.h"
#include "TimerOne.h"
#include "Sensors.h"
#include "Network.h"

#include <EtherCard.h>
#include <DHT.h>

// Gotta include this because the arduino IDE does some magic including nonsense.
#include <SoftwareSerial.h>

// Size of the tcp/ip send and receive buffer.
//
// This has to be global because who knows why. :/
// Make sure any changes to this are manually changed also in
// Network::Network since sizeof doesn't work because somehow
// arduino makes this not be global (Wtf?!).
byte Ethernet::buffer[500];

// If we should use wireless or wired ethernet.
const bool use_wired = false;

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
	if (use_wired)
		network.init_wired();
	else
		network.init_wireless();

	while(true){
		Sensor_data sensor_data = sensors.read_sensors();
		Serial.print(F("Sensor humidity: "));
		Serial.println(sensor_data.humidity);
		Serial.print(F("Sensor temperature: "));
		Serial.println(sensor_data.temperature_f);

		// network.send_packet(Network::Wireless, sensor_data);

		Serial.println(F("Waiting 20 seconds ..."));
		delay(20000);
	}
}

void loop(){

}
