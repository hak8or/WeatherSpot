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

#define SSID "OpenWrt"
#define password "castle2004"

/**
 * @brief The main setup loop for our sensors and WIFI.
 */
void setup(){
	// Start up the serial communication.
	// Find out why this cuts off the next serial print.
	Serial.begin(115200);
        
	for(int i = 0; i < 3; i++)
		Serial.print(F("==============="));

	Serial.println(F("\r\nWeatherSpot firmware Uno-V0.1"));

	for(int i = 0; i < 3; i++)
		Serial.print(F("==============="));

  // set pinmode for the CH_PD
  pinMode(12, OUTPUT);
  delay(3000);
  
	 // Startup our network.
  while (!network.init_wireless(SSID, password)){
    Serial.println(F("Failed connecting to wireless network, retying in 3 seconds."));
    Heartbeat::panic();
    delay(3000);
  }
  delay(3000);
  
	// Startup a heartbeat LED.
	Heartbeat::start();

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
  Serial.print("************ Time now in millis: ");
  Serial.print(millis());
  Serial.println(" ************");
  
        // Read our s.
	Sensor_data sensor_data = sensors.read_sensors();

	// Dump the info to our screen.
	Serial.print(F("Sensor humidity: "));
	Serial.println(sensor_data.humidity);
	Serial.print(F("Sensor temperature: "));
	Serial.println(sensor_data.temperature_f);
	Serial.print(F("Sensor light reading: "));
	Serial.println(sensor_data.light);
  Serial.print(F("Sensor pressure reading: "));
  Serial.println(sensor_data.pressure);

  // Make our heartbeat LED blink at a normal rate again if we were panicking earlier
  // or it was shut off.
  Heartbeat::start();

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
    }
    else{
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

