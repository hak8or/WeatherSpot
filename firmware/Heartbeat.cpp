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

	// Start a timer.
	Timer1.initialize();

	// Make sure we are not using any other ISR's.
	Timer1.detachInterrupt();

	// Attach a static toggle pin ISR to said timer every 1,000,000 microseconds.
	Timer1.attachInterrupt(Heartbeat::toggle, 1000000);
}

/**
 * @brief Stops the heartbeat LED on PB5 using a timer.
 */
void Heartbeat::stop(void){
	// turn off the led
  	PORTB = 0 << 5 & PORTB;

	// Disable interrupts for our timer.
	Timer1.detachInterrupt();
}

/**
 * @brief Blinks the LED much quicker to show that we are in an error state.
 */
void Heartbeat::panic(void){
	// Make sure we got rid of the old heartbeat ISR.
	Timer1.detachInterrupt();

	// Attach a static toggle pin ISR to said timer every 50,000 microseconds.
	Timer1.attachInterrupt(Heartbeat::toggle, 50000);
}
