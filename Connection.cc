//!Connection class is the parent of all connections.
/*!
 *This is a generic connection class from which USBConnection and BTConnection are derived from.
 *This class should never be used, it should allwas be cast to a more advanced connection.
 *It holds dummy functions for all the communications that the driver supports.
 */

#ifndef CONNECTION
#define CONNECTION

class Connection{
	public:



/////////////////////
//Connect Function//
///////////////////

//!Connect to NXT
/*!
 *Connection function to link to the NXT, this must be called before any other communication can happen.
 *In Connection class this function allways returns false or not connected.
 *
 *@return
 *	1 for connected, 0 for error.
 */
		virtual bool nxt_connect(){return false;};



//////////////////////
//Generic Functions//
////////////////////
	
//!Close any sensor on the NXT
/*!
 *Close any type of sensor at the specified port on the NXT.
 *In the Connection class this does nothing.
 *
 *@param port
 *	The port to close the NXT on.
 */	
		virtual void closeSensorOnPort(unsigned char port){return;};



////////////////////
//Drive Functions//
//////////////////

//!Set the speed of a given motor
/*!
 *Send a speed value to a specified motor that will set that motor to that speed till a new value is recieved.
 *In the Connection class this does nothing.
 *
 *@param speed
 *	The speed value to send to the motor (0-100).
 *@param motor
 *	The motor port #(0,1,2 or FF for all 3).
 */
		virtual void nxt_set_motor(int speed, int motor){return;};
//!Set the speed of a given motor for a given time
/*!
 *Send a speed value to a specified motor that will set that motor to that speed for the specifed number of rotations.
 *In the Connection class this does nothing.
 *
 *@param speed
 *	The speed value to send to the motor (0-100).
 *@param limit
 *	The number of degrees to turn before the wheel stops (360 = 1 rotation)(0-FFFF).
 *@param motor
 *	The motor port #(0,1,2 or FF for all 3).
 */	
		virtual void nxt_set_motor_with_limit(int speed, int limit, int motor){return;};



////////////////////
//Sonar Functions//
//////////////////

//!Add a sonar sensor to NXT
/*!Tell the NXT there is a sonar sensor on the specified port.
 *In the Connection class this does nothing.
 *
 *@param port
 *	The port to add the sonar to.
 */
		virtual void openSonarOnPort(int port){return;};
//!Set the Sonar to read continuously
/*!
 *Tell the NXT to allways read sonar values while it is running, instead of only when a new value is requested.
 *In the Connection class this does nothing.
 *
 *@param port
 *	The port to set continous read mode on.
 */
		virtual void setSonorContinuousReadMode(unsigned char port){return;};
//!Retrieve a Distance value from the sonar
/*!
 *Tell the NXT to return the newest sonar value from the sonar at the specified port.
 *In the Connection class this does nothing.
 *
 *@param port
 *	The port to read from.
 *
 *@return
 *	returns sonar value(0-100), in the Connection class this is allways -1.
 */
		virtual int getSonarValue(int port){return -1;};



///////////////////////////
//Light Sensor Functions//
/////////////////////////

//!Add a active light sensor to NXT 
/*!
 *Tell the NXT there is a light sensor on the specified port and to turn the sensor's led on.
 *In the Connection class this does nothing
 *
 *@param port
 *	The port to add the light sensor to
 */
		virtual void openLightActiveOnPort(unsigned char port){return;};
//!Add a passive light sensor to NXT 
/*!
 *Tell the NXT there is a light sensor on the specified port and to turn the sensor's led off.
 *In the Connection class this does nothing
 *
 *@param port
 *	The port to add the light sensor to
 */
		virtual void openLightPassiveOnPort(unsigned char port){return;};
//!Retrieve an ambient light value from the NXT
/*!
 *Tell the NXT to return the newest ambient light value from the light sensor at the specified port.
 *In the Connection class this does nothing
 *
 *@param port
 *	The port to read from
 *
 *@return
 *	returns light value(0-100), in the Connection class this is allways -1.
 */
		virtual int getLightValue(unsigned char port){return -1;};



///////////////////////////
//Sound Sensor Functions//
/////////////////////////

//!Add a sound sensor to NXT
/*!
 *Tell the NXT there is a sound sensor on the specified port.
 *In the Connection class this does nothing
 * 
 *@param port
 *	The port to add the sound sensor to
 */
		virtual void openSoundOnPort(unsigned char port){return;};
//!Retrieve an sound volume value from the NXT
/*!
 *Tell the NXT to return the newest sound volume value from the sound sensor at the specified port.
 *In the Connection class this does nothing
 *
 *@param port
 *	The port to read from
 *
 *@return
 *	returns sound value(0-100), in the Connection class this is allways -1.
 */
		virtual int getSoundValue(unsigned char port){return -1;};



/////////////////////
//Bumper Functions//
///////////////////

//!Add a bumper sensor to NXT
/*!
 *Tell the NXT there is a bumper sensor on the specified port.
 *In the Connection class this does nothing
 *
 *@param port
 *	The port to add the bumper sensor to
 */
		virtual void openBumperOnPort(int port){return;};
//!Retrieve an bumper value from the NXT
/*!
 *Tell the NXT to return the newest bumper value from the bumper sensor at the specified port.
 *In the Connection class this does nothing
 *
 *@param port
 *	The port to read from
 *
 *@return
 *	returns bumper value(1,0), in the Connection class this is allways -1.
 */
		virtual int getBumperValue(unsigned char port){return -1;};
};

#endif /* CONNECTION */
