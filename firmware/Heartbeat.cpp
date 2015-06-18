#include "Heartbeat.h"
#include "Arduino.h"
#include "TimerOne.h"

/**
 * @brief ISR for toggling the heartbeat LED.
 */
void Heartbeat::toggle(void){
	#if defined(__MK20DX256__) || defined(__MK20DX128__)
		// The teensy does have a toggle register!
		GPIOC_PTOR = 1 << 5;
	#else
		// This doesn't have a toggle register, so
		// we have to do this manually.
		PORTB = 1 << 5 ^ PORTB;
	#endif
}

/**
 * @brief Starts the heartbeat LED on PB5 using a timer.
 */
void Heartbeat::start(void){
	// Make sure pin is output.
	#if defined(__MK20DX256__) || defined(__MK20DX128__)
		pinMode(13, OUTPUT);
	#else
		DDRB = 1 << 5 | DDRB;
	#endif

	// Start a timer.
	Timer1.initialize();

	// Attach a static toggle pin ISR to said timer every 500,000 microseconds.
	Timer1.attachInterrupt(Heartbeat::toggle, 500000);
}

/**
 * @brief Stops the heartbeat LED on PB5 using a timer.
 */
void Heartbeat::stop(void){
	// Disable interrupts for our timer.
	Timer1.detachInterrupt();
}

/**
 * @brief Blinks the LED much quicker to show that we are in an error state.
 */
void Heartbeat::panic(void){
	// Start a timer.
	Timer1.initialize();

	// Attach a static toggle pin ISR to said timer every 50,000 microseconds.
	Timer1.attachInterrupt(Heartbeat::toggle, 50000);
}
