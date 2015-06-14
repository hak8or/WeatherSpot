#include "Heartbeat.h"
#include "TimerOne.h"

void setup(){
	// Start up the serial communication.
	Serial.begin(9600);
	Serial.println("WeatherSpot firmware Uno-V0.1");
	
	// Startup a heartbeat LED.
	Heartbeat::start();
}

void loop(){
	
}
