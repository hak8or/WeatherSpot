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

	// initiate DH11 sesnor at pin 2
	sensors.init_DH11(2);

  // Setup the pressure sensor.
  sensors.init_MPL3115A2();

  // set pinmode for the CH_PD
  pinMode(12, OUTPUT);

}

void loop(){

  // turn on the module
  digitalWrite(12, HIGH);

  delay(2000); // let it turn on
  
  // Startup our network.
  while (!network.init_wireless("TRYME", "Damian126No#ash!")){
    Serial.println(F("Failed connecting to wireless network, retying in 3 seconds."));
    Heartbeat::panic();
    delay(3000);
  }
  delay(3000);
  
  // time stamp each iteration
  Serial.print("************ Time now in millis: ");
  Serial.print(millis());
  Serial.println(" ************");
  
        // Read our sensors.
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

  // wake up the led after the measurements
  Heartbeat::start();

  // Make our heartbeat LED blink at a normal rate again if we were panicking earlier.
  Heartbeat::start();

  // try to send 5 times, if 5 fail restart the module
  for(int i = 0; i < 5; i++){
    Serial.print("Trying to send data: ");
    Serial.println(i);
    
    if(!network.send_packet(sensor_data)){
      if(i == 2){
        // we have reached the limit of our patience, restart the module and continue
        network.send_command("AT+RST", "OK", 2, 5000);
        
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

  // close the TCP connection 
  network.send_command("AT+CIPCLOSE", "OK", 2, 1500);
  // disconnect from the AP to conserve energy
  network.send_command("AT+CWQAP", "OK", 2, 1500);

  // turn off the module
  digitalWrite(12, LOW);

	Serial.println(F("Waiting 5 min... - sleep time"));
  delay(10); // necessary in order to get the text out the serial 

  // stop the led
  Heartbeat::stop();

  // sleep for 5 min = 10 sec * 30 
  for(int i = 0; i < 1; i++){
    EnableWatchDog(0b100001); // sleep for 8 seconds
    digitalWrite(13, HIGH);
    EnableWatchDog(0b000110); // sleep for 1 second
    digitalWrite(13, LOW);
    EnableWatchDog(0b000110); // sleep for 1 second
  }
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

