// Gotta include this because the arduino IDE does some magic including nonsense.
#include <SoftwareSerial.h>
#include <Wire.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

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

  // Startup our network.
  while (!network.init_wireless("TRYME", "Damian126No#ash!")){
    Serial.println(F("Failed connecting to wireless network, retying in 3 seconds."));
    Heartbeat::panic();
    delay(3000);
  }

  // Make our heartbeat LED blink at a normal rate again if we were panicking earlier.
  Heartbeat::start();

  // try to send 3 times, if 3 fail restart the module
  for(int i = 0; i < 3; i++){
    Serial.print("Trying to send data: ");
    Serial.println(i);
    
    if(!network.send_packet(sensor_data)){
      if(i == 2){
        // we have reached the limit of our patience, restart the module and continue
        network.send_command("AT+RST", "OK", 2, 1500);
        
        // start panic mode
        Heartbeat::panic();
        break;
      }
    }
    else{
      // Make our heartbeat LED blink at a normal rate again if we were panicking earlier.
      Heartbeat::start();
      break;
    }
  }
 
  // disconnect from the AP to conserve energy
  network.send_command("AT+CWQAP", "OK", 2, 1500);

	Serial.println(F("Waiting 5 min... - sleep time"));
  delay(10); // necessary in order to get the text out the serial 

  // stop the led
  Heartbeat::stop();

  // sleep for 5 min = 8 sec * 37
  for(int i = 0; i < 37; i++){
    EnableWatchDog(0b100001); // sleep for 8 seconds
  }

  // wake up little arduino
  Heartbeat::start();
}

// disable watchdog interrupt
ISR(WDT_vect){
  // disable the watchdog
  wdt_disable();
}

void EnableWatchDog(const byte time_to_sleep){
  MCUSR = 0;                         
  WDTCSR |= 0b00011000;              
  WDTCSR =  0b01000000 | time_to_sleep;   

  wdt_reset();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN); 
  // sleep until we are interrupted
  sleep_mode();           
}

