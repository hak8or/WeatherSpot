// Gotta include this because the arduino IDE does some magic including nonsense.
#include <SoftwareSerial.h>
#include <Wire.h>

#include "Sensors.h"
#include "Heartbeat.h"
#include "Network.h"
#include "DHT.h"
#include "TimerOne.h"
#include "SparkFunMPL3115A2.h"

// global network object
Network network;

// global sensors object
Sensors sensors;

// global pressure sesnor object
MPL3115A2 pressure_sensor;

void setup(){
	// Start up the serial communication.
	// Find out why this cuts off the next serial print.
	Serial.begin(115200);

  // join the I2C bus
  Wire.begin(); 

  // initiate the pressure sensor
  pressure_sensor.begin();

  // configure the pressure sensor
  pressure_sensor.setModeBarometer();
  pressure_sensor.setOversampleRate(7);
  pressure_sensor.enableEventFlags();
        
	for(int i = 0; i < 3; i++)
		Serial.print(F("==============="));

	Serial.println(F("\r\nWeatherSpot firmware Uno-V0.1"));

	for(int i = 0; i < 3; i++)
		Serial.print(F("==============="));
	
	// Startup a heartbeat LED.
	Heartbeat::start();

	// initiate DH11 sesnor at pin 2
	sensors.init_DH11(2);

	// Startup our network.
	
	while (!network.init_wireless("OpenWrt", "castle2004")){
		Serial.println(F("Failed connecting to wireless network, retying in 3 seconds."));
		Heartbeat::panic();
		delay(3000);
	}

	// Make our heartbeat LED blink at a normal rate again if we were panicking earlier.
	Heartbeat::start();

}

void loop(){

  // time stamp each iteration
  Serial.print("************ Time now in millis: ");
  Serial.print(millis());
  Serial.println(" ************");
  
        // Read our sensors.
	Sensor_data sensor_data = sensors.read_sensors();
  sensor_data.pressure = pressure_sensor.readPressure();

	// Dump the info to our screen.
	Serial.print(F("Sensor humidity: "));
	Serial.println(sensor_data.humidity);
	Serial.print(F("Sensor temperature: "));
	Serial.println(sensor_data.temperature_f);
	Serial.print(F("Sensor light reading: "));
	Serial.println(sensor_data.light);
  Serial.print(F("Sensor pressure reading: "));
  Serial.println(sensor_data.pressure);

	// Keep trying to send the packet to our backend server.
	while (!network.send_packet(sensor_data)){
		Serial.println(F("Failed sending POST request to backend, retying in 3 seconds."));
		Heartbeat::panic();
		delay(3000);
	}

        // Make our heartbeat LED blink at a normal rate again if we were panicking earlier.
	Heartbeat::start();

	Serial.println(F("Waiting 20 seconds ..."));
	delay(20000);

}
