//!USBConnection class is the child of the Connection class used for USB communitcation.
/*!
 *This is the USB version of the Connection class. It uses libusb to send direct commands to NXT over a usb connection.
 *It should be 100% functional when it is cast to a connection or vice versa....
 *@see Connection
 *@see BTConnection	
 */



#include <unistd.h>
#include <string.h>

#include <usb.h>

#include <libplayercore/playercore.h>

#include "Connection.cc"

class USBConnection : public Connection{

//////////////////////
//USB Connect Stuff//
////////////////////
private:
	static const int NXT_VENDOR_ID = 0x0694;
	static const int NXT_PRODUCT_ID = 0x0002;
	
	usb_dev_handle *pUSBHandle;
	unsigned char ucEpOut;
	unsigned char ucEpIn;
	static const int TIMEOUT = 0x0BB8;

	static struct usb_device *DevInit(void)
	{
		struct usb_bus *usb_bus;
		struct usb_device *dev;

		usb_init();
		usb_find_busses();
		usb_find_devices();

		for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next)
		{
			for (dev = usb_bus->devices; dev; dev = dev->next)
			{
				if ((dev->descriptor.idVendor == NXT_VENDOR_ID) && (dev->descriptor.idProduct == NXT_PRODUCT_ID))
					return dev;
			}
		}
		return 0;
	}

//!Open USB connection to the nxt
/*!
 *USB function to link to the NXT, this should not be called directly. Instead use Connect(). This was coppied from NXTPP.
 *
 *@return
 *	true for connected, false for error.
 *
 *@see nxt_connect() 
 */
	bool DevOpen()
	{
		struct usb_device *usb_dev;
		int nEp = 0;

		usb_dev = DevInit();

		if (usb_dev == 0)
		{
			//std::cerr << "Device not found!" << std::endl;
			puts("Device not found!");
			return 0;
		}

		pUSBHandle = usb_open(usb_dev);

		if (pUSBHandle == 0)
		{
			//std::cerr << "Not able to claim the USB device" << std::endl;
			puts("Not able to claim the USB device");
			return 0;
		}
		else
		{
			if (usb_dev->config)
			{
				if (usb_dev->config->interface)
				{
					if (usb_dev->config->interface->altsetting)
					{
						usb_claim_interface(pUSBHandle, usb_dev->config->interface->altsetting->bInterfaceNumber);
						if ((nEp = usb_dev->config->interface->altsetting->bNumEndpoints))
						{
							if (usb_dev->config->interface->altsetting->endpoint)
							{
								ucEpIn = (unsigned char) (usb_dev->config->interface->altsetting->endpoint[0].bEndpointAddress);
								if (nEp > 1)
									ucEpOut = (unsigned char) (usb_dev->config->interface->altsetting->endpoint[1].bEndpointAddress);

							}
						}
					}
				}
			}
		}
		return 1;
	}
public:
////////////////
//konztuktzor//
//////////////
	USBConnection(){

		pUSBHandle = 0;
		ucEpOut = 0;
		ucEpIn = 0;
	};
/////////////////////
//Connect Function//
///////////////////

//!@copybrief Connection::nxt_connect()
/*!@copydetails Connection::nxt_connect()
 *@section Implementation
 *	This connect function creates a usb_device object, calls the DevOpen function, and checks if the usb connection is working.
 *
 *@see DevOpen()
 */

	bool nxt_connect(){
		struct usb_device *DevInit();
		DevOpen();
		if (pUSBHandle == 0){
			puts("pUSBHandle = 0");
			return false;
		}else{
			puts("pUSBHandle != 0");
			return true;
		}
	};

//////////////////////
//Generic Functions//
////////////////////

//!@copybrief Connection::closeSensorOnPort()
/*!@copydetails Connection::closeSensorOnPort()
 *@section Implementation
 *	Sends the SetInputMode direct command to the NXT, see NXT documentation for more details.
 */

	void closeSensorOnPort(unsigned char port){
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
		directCommandBuffer[1] = 0x05;//!byte[1] = 0x05 --direct command number 5 for SetInputMode
		directCommandBuffer[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
		directCommandBuffer[3] = 0x00;//!byte[3] = 0x00 --sensor type -- no sensor
		directCommandBuffer[4] = 0x00;//!byte[4] = 0x00 --sensor mode -- raw mode (this is irelevent as there is no sensor)
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}

////////////////////
//Drive Functions//
//////////////////

//!@copybrief Connection::nxt_set_motor()
/*!@copydetails Connection::nxt_set_motor()
 *@section Implementation
 *	Sends the SetOutputState direct command to the nxt. This uses the simplest method for controlling the motors.
 * It sends a power value and motor number pair to the NXT. Which will cause the motor to run at that power untill instructed otherwise.  
 */
	void nxt_set_motor(int speed, int motor)
	{

		if(motor > 2){
			return;
		}
	
		char directCommandBuffer[256];

			directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
			directCommandBuffer[1] = 0x04;//!byte[1] = 0x04 --set ouput state command
			directCommandBuffer[2] = motor;//!byte[2] = port --integer port value sent to function
			directCommandBuffer[3] = speed;//!byte[3] = power --integer power value sent to function(not checked fror validity)
			directCommandBuffer[4] = 0x01;//!byte[4] = 0x01 --mode - motor on
			directCommandBuffer[5] = 0x00;//!byte[5] = 0x00 --regulation mode - none
			directCommandBuffer[6] = 0x00;//!byte[6] = 0x00 --turn ratio - none (only used in motor sync mode)
			directCommandBuffer[7] = 0x20;//!byte[7] = 0x20 --run state - running
			directCommandBuffer[8] = 0x00;/*!byte[8] = 0x00 --tach state - run forever (all the tach state variables are
							combined to one large number that limits the degrees the motor will rotate)*/
			directCommandBuffer[9] = 0x00;//!byte[9] = 0x00 --tach state - run forever
			directCommandBuffer[10] = 0x00;//!byte[10] = 0x00 --tach state - run forever
			directCommandBuffer[11] = 0x00;//!byte[11] = 0x00 --tach state - run forever
			directCommandBuffer[12] = 0x00;//!byte[12] = 0x00 --tach state - run forever

		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 13, 0x0BB8);
		return;
	}

//!@copybrief Connection::nxt_set_motor_with_limit()
/*!@copydetails Connection::nxt_set_motor_with_limit()
 *@section Implementation
 *	This function is the same as the regular set motor function with the exception that it allows you to set a tachometer limit.
 * This prevents the wheel from rotating past a specified number of degrees. It was written with the intent of using it to controll a gripper or arm,
 *that rotates a set number of degrees back and forth. It dosnt work very exactly and in order to make it work better call nxtsetmotor(100) instantly
 *followed by this, to prevent tach limit confusion when changing directions.
 */
	void nxt_set_motor_with_limit(int speed, int limit, int motor)
	{

		if(motor > 2){
			return;
		}
	
		char directCommandBuffer[256];

			directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
			directCommandBuffer[1] = 0x04;//!byte[1] = 0x04 --set ouput state command
			directCommandBuffer[2] = motor;//!byte[2] = port --integer port value sent to function
			directCommandBuffer[3] = speed;//!byte[3] = power --integer power value sent to function(not checked fror validity)
			directCommandBuffer[4] = 0x01;//!byte[4] = 0x01 --mode - motor on
			directCommandBuffer[5] = 0x00;//!byte[5] = 0x00 --regulation mode - none
			directCommandBuffer[6] = 0x00;//!byte[6] = 0x00 --turn ratio - none (only used in motor sync mode)
			directCommandBuffer[7] = 0x20;//!byte[7] = 0x20 --run state - running
			directCommandBuffer[8] = limit;/*!byte[8] = limit --tach state - integer limit value sent to function (all the tach 
						state variables are combined to one large number that limits the degrees the motor 
						will rotate. This function only implements the first byte, if you need more fix the code :-P)*/
			directCommandBuffer[9] = 0x00;//!byte[9] = 0x00 --tach state - run forever
			directCommandBuffer[10] = 0x00;//!byte[10] = 0x00 --tach state - run forever
			directCommandBuffer[11] = 0x00;//!byte[11] = 0x00 --tach state - run forever
			directCommandBuffer[12] = 0x00;//!byte[12] = 0x00 --tach state - run forever

		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 13, 0x0BB8);
		return;
	}

////////////////////
//Sonar Functions//
//////////////////

//!@copybrief Connection::openSonarOnPort()
/*!@copydetails Connection::openSonarOnPort()
 *@section Implementation
 *	sends SetInputMode command configured for sonar to the nxt on the specified port.
 */
	void openSonarOnPort(int port){
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
		directCommandBuffer[1] = 0x05;//!byte[1] = 0x05 --direct command number 5 for SetInputMode
		directCommandBuffer[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
		directCommandBuffer[3] = 0x0B;//!byte[3] = 0x0B --sensor type - LOWSPEED_V9
		directCommandBuffer[4] = 0x00;//!byte[4] = 0x00 --sensor mode - raw mode
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	
		return;
	}

//!@copybrief Connection::setSonorContinuousReadMode()
/*!@copydetails Connection::setSonorContinuousReadMode()
 *@section Implementation
 *	sends a lswrite command to the NXT with a package that will be sent to the sonar module which contains the sonar command
 *to go to continous read mode. For me details see both the direct command, and the sonar documentation.
 */
	//this function is allready set on sonar by defualt so it isnt needed, is just here in case of "whatever"
	void setSonorContinuousReadMode(unsigned char port){
		unsigned char directCommandBuffer2[256];
		directCommandBuffer2[0] = 0x80;//!byte[0] = 0x80 --no return
		directCommandBuffer2[1] = 0x0f;//!byte[1] = 0x0F --command - lswrite
		directCommandBuffer2[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
		directCommandBuffer2[3] = 0x03;//!byte[3] = 0x03 --outgoing package size of 3 (from nxt to sonar, not from me to nxt)
		directCommandBuffer2[4] = 0x00;//!byte[4] = 0x00 --returning package size of 0 (from sonar to nxt, not from nxt to me)
		directCommandBuffer2[5] = 0x02;//!byte[5] = 0x02 --package 1 - from sonar docs - set continus mode
		directCommandBuffer2[6] = 0x41;//!byte[6] = 0x41 --package 2 - from sonar docs - set continus mode
		directCommandBuffer2[7] = 0x02;//!byte[7] = 0x02 --package 3 - from sonar docs - set continus mode

		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer2), 8, 0x0BB8);
		//unsigned char directReadBuffer2[256];
		//usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer2), 3, 0x0BB8);
		
	}

//!@copybrief Connection::getSonarValue()
/*!@copydetails Connection::getSonarValue()
 *@section Implementation
 *	This function reads from the nxt it keeps sending a read command to to sonar and cheking if the data is ready,
 * once the data is ready it reads and returns it.
 */
	int getSonarValue(int port){
		int bytesRead = 0;
		do{
			//!FIRST COMMAND lswrite, send a packet from the nxt to the sonar telling you want to read a value.
			unsigned char directCommandBuffer21[256];
			directCommandBuffer21[0] = 0x80;//!byte[0] = 0x80 --no return
			directCommandBuffer21[1] = 0x0F;//!byte[1] = 0x0F --command - lswrite
			directCommandBuffer21[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
			directCommandBuffer21[3] = 0x02;//!byte[3] = 0x02 --outgoing package size of 2 (from nxt to sonar, not from me to nxt)
			directCommandBuffer21[4] = 0x01;//!byte[4] = 0x01 --returning package size of 1 (from sonar to nxt, not from nxt to me)
			directCommandBuffer21[5] = 0x02;//!byte[5] = 0x02 --package 1 - from sonar docs - get value
			directCommandBuffer21[6] = 0x42;//!byte[6] = 0x42 --package 1 - from sonar docs - get value
		
			usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer21), 7, 0x0BB8);
		
			//!SECOND COMMAND LsGetStatus, check if sonar value is ready to be read.
			unsigned char directCommandBuffer211[256];
			directCommandBuffer211[0] = 0x00;//!byte[0] = 0x00 --return a value
			directCommandBuffer211[1] = 0x0E;//!byte[1] = 0x0E --command - lsget status
			directCommandBuffer211[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
		
			usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer211), 3, 0x0BB8);
		
			unsigned char directReadBuffer211[256];
			usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer211), 4, 0x0BB8);
		
			bytesRead = (int)directReadBuffer211[3];
		
		}while(bytesRead < 1);
		//!EXIT LOOP, FINAL COMMAND lsread.
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//!byte[0] = 0x00 --return a value
		directCommandBuffer22[1] = 0x10;//!byte[1] = 0x10 --command - lsread
		directCommandBuffer22[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
	
		unsigned char directReadBuffer22[256];
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 20, 0x0BB8);
		//std::cout << "bit #" << 4 << " contents = " << std::hex << (int)directReadBuffer22[4] << std::endl;
		return (int)directReadBuffer22[4];
	}

///////////////////////////
//Light Sensor Functions//
/////////////////////////

//!@copybrief Connection::openLightActiveOnPort()
/*!@copydetails Connection::openLightActiveOnPort()
 *@section Implementation
 *	sends the SetInputMode command configured for the light sensor to the NXT. This function sends the active mode version of the command,
 * which means the LED on the sensor will be on.
 */
	void openLightActiveOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
		directCommandBuffer[1] = 0x05;//!byte[1] = 0x05 --command - SetInputMode
		directCommandBuffer[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
		directCommandBuffer[3] = 0x05;//!byte[3] = 0x05 --sensor type - LIGHT_ACTIVE
		directCommandBuffer[4] = 0x80;//!byte[4] = 0x80 --sensor mode - PCTFULLSCALEMODE (dunno why but thats what it is :D)
	
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}

//!@copybrief Connection::openLightPassiveOnPort()
/*!@copydetails Connection::openLightPassiveOnPort()
 *@section Implementation
 *	sends the SetInputMode command configured for the light sensor to the NXT. This function sends the passive mode version of the command,
 * which means the LED on the sensor will be off.
 */
	void openLightPassiveOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
		directCommandBuffer[1] = 0x05;//!byte[1] = 0x05 --command - SetInputMode
		directCommandBuffer[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
		directCommandBuffer[3] = 0x06;//!byte[3] = 0x06 --sensor type - LIGHT_INACTIVE
		directCommandBuffer[4] = 0x80;//!byte[4] = 0x80 --sensor mode - PCTFULLSCALEMODE (dunno why but thats what it is :D)
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}

//!@copybrief Connection::getLightValue()
/*!@copydetails Connection::getLightValue()
 *@section Implementation
 *	Reads a light sensor value from the NXT.
 */
	int getLightValue(unsigned char port){
	
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//!byte[0] = 0x00 --return a value
		directCommandBuffer22[1] = 0x07;//!byte[1] = 0x07 --command - GetInputValues
		directCommandBuffer22[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
	
		unsigned char directReadBuffer22[256];
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
		return directReadBuffer22[12];
	}

///////////////////////////
//Sound Sensor Functions//
/////////////////////////

//!@copybrief Connection::openSoundOnPort()
/*!@copydetails Connection::openSoundOnPort()
 *@section Implementation
 *	Sends the SetInputMode command configured for the sound sensor to the NXT.
 */
	void openSoundOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
		directCommandBuffer[1] = 0x05;//!byte[1] = 0x05 --command - SetInputMode
		directCommandBuffer[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
		directCommandBuffer[3] = 0x05;//!byte[3] = 0x05 --sensor type - LIGHT_INACTIVE (is this a bug??? maybe should be SOUND_DB or something?)
		directCommandBuffer[4] = 0x80;//!byte[4] = 0x80 --sensor mode - PCTFULLSCALEMODE (dunno why but thats what it is :D)
	
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}

//!@copybrief Connection::getSoundValue()
/*!@copydetails Connection::getSoundValue()
 *@section Implementation
 *	Reads a sound sensor value from the NXT.
 */
	//this is exactly the same as get light value
	int getSoundValue(unsigned char port){
	
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//!byte[0] = 0x00 --return a value
		directCommandBuffer22[1] = 0x07;//!byte[1] = 0x07 --command - GetInputValues
		directCommandBuffer22[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
	
		unsigned char directReadBuffer22[256];
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
		return directReadBuffer22[12];
	}

/////////////////////
//Bumper Functions//
///////////////////

//!@copybrief Connection::openBumperOnPort()
/*!@copydetails Connection::openBumperOnPort()
 *@section Implementation
 *	Sends the SetInputMode command configured for the bumper sensor to the NXT.
 */
	void openBumperOnPort(int port)
	{
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x80;//!byte[0] = 0x80 --no return
			directCommandBuffer[1] = 0x05;//!byte[1] = 0x05 --command - SetInputMode
			directCommandBuffer[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
			directCommandBuffer[3] = 0x01;//!byte[3] = 0x01 --sensor type - switch
			directCommandBuffer[4] = 0x20;//!byte[4] = 0x20 --sensor mode - boolean
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
		return;
	}

//!@copybrief Connection::getBumperValue()
/*!@copydetails Connection::getBumperValue()
 *@section Implementation
 *	Reads a bumper sensor value from the NXT.
 */
	//same exact function as get light and sound functions
	int getBumperValue(unsigned char port){
	
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//!byte[0] = 0x00 --return a value
		directCommandBuffer22[1] = 0x07;//!byte[1] = 0x07 --command - GetInputValues
		directCommandBuffer22[2] = port;//!byte[2] = port --integer port value sent to function (not checked for validity)
	
		unsigned char directReadBuffer22[256];
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
		return directReadBuffer22[12];
	}
};
