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
const bool use_wired = true;

void setup(){
	// Start up the serial communication.
	// Find out why this cuts off the next serial print.
	Serial.begin(115200);
	Serial.print("WeatherSpot firmware Uno-V0.1\n");
	
	// Startup a heartbeat LED.
	// Heartbeat::start();

	// Startup our network.
	// Find out why network disables our heartbeat LED.
	Network network;
	if (use_wired)
		network.init_wired();
	else
		network.init_wireless();

	Sensors sensors;
	sensors.init_DH11(2);
	Sensor_data sensor_data = sensors.read_sensors();

	Serial.print("Sensor humidity: ");
	Serial.println(sensor_data.humidity);
	Serial.print("Sensor temperature: ");
	Serial.println(sensor_data.temperature_f);

	while(1){
		delay(5000);
		network.send_packet(Network::Wired);
	}
	
}

void loop(){

}
