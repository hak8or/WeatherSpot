// Used to turn the heartbeat LED on and off using a timer.
class Heartbeat
{
public:
	/**
	* @brief Starts the heartbeat LED on PB5 using a timer.
	*/
	static void start(void);

	/**
	 * @brief Stops the heartbeat LED and timer.
	 */
	static void stop(void);

	/**
	 * @brief ISR for toggling the heartbeat LED.
	 */
	inline static void toggle(void);
};