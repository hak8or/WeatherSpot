#include "Heartbeat.h"
#include "Arduino.h"
#include "TimerOne.h"

/**
 * @brief ISR for toggling the heartbeat LED.
 */
void Heartbeat::toggle(void){
	// This doesn't have a toggle register, so
	// we have to do this manually.
	PORTB = 1 << 5 ^ PORTB;
}

/**
 * @brief Starts the heartbeat LED on PB5 using a timer.
 */
void Heartbeat::start(void){
	// Make sure pin is output.
	DDRB = 1 << 5 | DDRB;

	// Make a timer that runs every 500,000 milliseconds.
	Timer1.initialize(500000);

	// And attach a static toggle pin ISR.
	Timer1.attachInterrupt(Heartbeat::toggle);
}

