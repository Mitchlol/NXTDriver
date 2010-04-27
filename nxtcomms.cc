//#include <iostream>
//libusb
#include <usb.h>

// Socket, used for Bluetooth rfcomm 
#include <sys/socket.h>
#include <sys/types.h>

// Bluetooth headers
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <unistd.h>
#include <string.h>

//#include <libplayercore/playercore.h>

class Connection{
	public:
/////////////////////
//Connect Function//
///////////////////
		virtual bool nxt_connect(){return false;};
//////////////////////
//Generic Functions//
////////////////////		
		virtual void closeSensorOnPort(unsigned char port){return;};
////////////////////
//Drive Functions//
//////////////////
		virtual void nxt_set_motor(int speed, int motor){return;};
		virtual void nxt_set_motor_with_limit(int speed, int limit, int motor){return;};
////////////////////
//Sonar Functions//
//////////////////
		virtual void openSonarOnPort(int port){return;};
		virtual void setSonorContinuousReadMode(unsigned char port){return;};
		virtual int getSonarValue(int port){return -1;};
///////////////////////////
//Light Sensor Functions//
/////////////////////////
		virtual void openLightActiveOnPort(unsigned char port){return;};
		virtual void openLightPassiveOnPort(unsigned char port){return;};
		virtual int getLightValue(unsigned char port){return -1;};
///////////////////////////
//Sound Sensor Functions//
/////////////////////////
		virtual void openSoundOnPort(unsigned char port){return;};
		virtual int getSoundValue(unsigned char port){return -1;};
/////////////////////
//Bumper Functions//
///////////////////
		virtual void openBumperOnPort(int port){return;};
		virtual int getBumperValue(unsigned char port){return -1;};
};

class USBConnection : public Connection{
public:

//////////////////////
//USB Connect Stuff//
////////////////////

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

	USBConnection(){

		pUSBHandle = 0;
		ucEpOut = 0;
		ucEpIn = 0;
	};
/////////////////////
//Connect Function//
///////////////////
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

	void closeSensorOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no return
		directCommandBuffer[1] = 0x05;//command
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x00;//sensor type -- no sensor
		directCommandBuffer[4] = 0x00;//sensor mode
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}

////////////////////
//Drive Functions//
//////////////////

	void nxt_set_motor(int speed, int motor)
	{

		if(motor > 2){
			return;
		}
	
		char directCommandBuffer[256];

			directCommandBuffer[0] = 0x80;// no return
			directCommandBuffer[1] = 0x04;//set ouput state command
			directCommandBuffer[2] = motor;//port
			directCommandBuffer[3] = speed;//power
			directCommandBuffer[4] = 0x01;//mode - motor on
			directCommandBuffer[5] = 0x00;//regulation mode - none
			directCommandBuffer[6] = 0x00;//turn ratio
			directCommandBuffer[7] = 0x20;//run state - running
			directCommandBuffer[8] = 0x00;//tach state - run forever
			directCommandBuffer[9] = 0x00;//tach state - run forever
			directCommandBuffer[10] = 0x00;//tach state - run forever
			directCommandBuffer[11] = 0x00;//tach state - run forever
			directCommandBuffer[12] = 0x00;//tach state - run forever

		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 13, 0x0BB8);
		return;
	}

	void nxt_set_motor_with_limit(int speed, int limit, int motor)
	{

		if(motor > 2){
			return;
		}
	
		char directCommandBuffer[256];

			directCommandBuffer[0] = 0x80;// no return
			directCommandBuffer[1] = 0x04;//set ouput state command
			directCommandBuffer[2] = motor;//port
			directCommandBuffer[3] = speed;//power
			directCommandBuffer[4] = 0x01;//mode - motor on
			directCommandBuffer[5] = 0x00;//regulation mode - none
			directCommandBuffer[6] = 0x00;//turn ratio
			directCommandBuffer[7] = 0x20;//run state - running
			directCommandBuffer[8] = limit;//tach state - run forever
			directCommandBuffer[9] = 0x00;//tach state - run forever
			directCommandBuffer[10] = 0x00;//tach state - run forever
			directCommandBuffer[11] = 0x00;//tach state - run forever
			directCommandBuffer[12] = 0x00;//tach state - run forever

		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 13, 0x0BB8);
		return;
	}

////////////////////
//Sonar Functions//
//////////////////

	void openSonarOnPort(int port){
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no return
		directCommandBuffer[1] = 0x05;//set input mode command
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x0B;//sensor type - LOWSPEED_V9
		directCommandBuffer[4] = 0x00;//sensor mode - raw mode
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	
		//unsigned char directReadBuffer[256];
		//usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer), 3, 0x0BB8);
	
		return;
	}

	//this function is allready set on sonar by defualt so it isnt needed, is just here in case of "whatever"
	void setSonorContinuousReadMode(unsigned char port){
		unsigned char directCommandBuffer2[256];
		directCommandBuffer2[0] = 0x80;//no return
		directCommandBuffer2[1] = 0x0f;//command - lswrite
		directCommandBuffer2[2] = port;//port
		directCommandBuffer2[3] = 0x03;//outgoing package size
		directCommandBuffer2[4] = 0x00;//returning package size (from sonar to nxt, not from nxt to me)
		directCommandBuffer2[5] = 0x02;//package 1 - from sonar docs - set continus mode
		directCommandBuffer2[6] = 0x41;//package 2
		directCommandBuffer2[7] = 0x02;//package 3

		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer2), 8, 0x0BB8);
		//unsigned char directReadBuffer2[256];
		//usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer2), 3, 0x0BB8);
		
	}

	int getSonarValue(int port){
		int bytesRead = 0;
		do{
			unsigned char directCommandBuffer21[256];
			directCommandBuffer21[0] = 0x80;//no return
			directCommandBuffer21[1] = 0x0F;//command - lsread
			directCommandBuffer21[2] = port;//port
			directCommandBuffer21[3] = 0x02;//send size
			directCommandBuffer21[4] = 0x01;//return size
			directCommandBuffer21[5] = 0x02;//send 1
			directCommandBuffer21[6] = 0x42;//send 2
		
			usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer21), 7, 0x0BB8);
			//unsigned char directReadBuffer21[256];
			//usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer21), 3, 0x0BB8);
		
	
			unsigned char directCommandBuffer211[256];
			directCommandBuffer211[0] = 0x00;//return
			directCommandBuffer211[1] = 0x0E;//command - lsget status
			directCommandBuffer211[2] = port;//port
		
			usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer211), 3, 0x0BB8);
		
			unsigned char directReadBuffer211[256];
			usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer211), 4, 0x0BB8);
		
			bytesRead = (int)directReadBuffer211[3];
		
		}while(bytesRead < 1);
	
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//return
		directCommandBuffer22[1] = 0x10;//command - lsread
		directCommandBuffer22[2] = port;//port
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
	
		unsigned char directReadBuffer22[256];
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 20, 0x0BB8);
		//std::cout << "bit #" << 4 << " contents = " << std::hex << (int)directReadBuffer22[4] << std::endl;
		return (int)directReadBuffer22[4];
	}

///////////////////////////
//Light Sensor Functions//
/////////////////////////

	void openLightActiveOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no return
		directCommandBuffer[1] = 0x05;//command
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x05;//sensor type
		directCommandBuffer[4] = 0x80;//sensor mode
	
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}

	void openLightPassiveOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no return
		directCommandBuffer[1] = 0x05;//command
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x06;//sensor type
		directCommandBuffer[4] = 0x80;//sensor mode
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}

	int getLightValue(unsigned char port){
	
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//return
		directCommandBuffer22[1] = 0x07;//command - lsread
		directCommandBuffer22[2] = port;//port
	
		unsigned char directReadBuffer22[256];
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
		return directReadBuffer22[12];
	}

///////////////////////////
//Sound Sensor Functions//
/////////////////////////

	void openSoundOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no return
		directCommandBuffer[1] = 0x05;//command
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x05;//sensor type
		directCommandBuffer[4] = 0x80;//sensor mode
	
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	}
	//this is exactly the same as get light value
	int getSoundValue(unsigned char port){
	
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//return
		directCommandBuffer22[1] = 0x07;//command - lsread
		directCommandBuffer22[2] = port;//port
	
		unsigned char directReadBuffer22[256];
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
		return directReadBuffer22[12];
	}

/////////////////////
//Bumper Functions//
///////////////////

	void openBumperOnPort(int port)
	{
		//ViUInt8 directCommandBuffer[] = { 0x05, port, 0x01, 0x20 };
		//Comm::SendDirectCommand( false, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ), NULL, 0);
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x80;//no nreturn
			directCommandBuffer[1] = 0x05;//set sensor command
			directCommandBuffer[2] = port;//port
			directCommandBuffer[3] = 0x01;//sensor type (switch)
			directCommandBuffer[4] = 0x20;//sensor mode (boolean)
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
		return;
	}
	//same exact function as get light and sound functions
	int getBumperValue(unsigned char port){
	
		unsigned char directCommandBuffer22[256];
		directCommandBuffer22[0] = 0x00;//return
		directCommandBuffer22[1] = 0x07;//command - lsread
		directCommandBuffer22[2] = port;//port
	
		unsigned char directReadBuffer22[256];
	
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
		return directReadBuffer22[12];
	}
};

class BTConnection : public Connection{
	public:
//////////////////////
//USB Connect Stuff//
////////////////////
	int nxtSocket;
	char *btAddress;
	int init_bluetooth(char *btAddress)	{
		struct sockaddr_rc addr={0};
		int status;

		// Allocate a socket
		nxtSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

		// Set what type and who to connect to
		addr.rc_family = AF_BLUETOOTH;
		addr.rc_channel = (uint8_t) 1;
		str2ba(btAddress, &addr.rc_bdaddr);

		// Connect to the NXT
		status = connect(nxtSocket, (struct sockaddr *)&addr, sizeof(addr) );

		if (status < 0)	{
			perror("Error connecting Bluetooth");
			return status;
		}

		return 0;
	}
////////////////
//konztuktzor//
//////////////
	BTConnection(char *address){
		btAddress = address;
	}
/////////////////////
//Connect Function//
///////////////////
	bool nxt_connect(){
		if(init_bluetooth(btAddress) <0){
			close(nxtSocket);
			return false;
		}else{
			return true;
		}
	};
};

/*
static const int NXT_VENDOR_ID = 0x0694;
static const int NXT_PRODUCT_ID = 0x0002;

static usb_dev_handle *pUSBHandle = 0;
static unsigned char ucEpOut = 0;
static unsigned char ucEpIn = 0;
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



static bool DevOpen()
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

//////////////////////
//Generic Functions//
////////////////////

static void closeSensorOnPort(unsigned char port){
	//std::cout << "set sonar ports" << std::endl;
	unsigned char directCommandBuffer[256];
	directCommandBuffer[0] = 0x80;//no return
	directCommandBuffer[1] = 0x05;//command
	directCommandBuffer[2] = port;//port
	directCommandBuffer[3] = 0x00;//sensor type -- no sensor
	directCommandBuffer[4] = 0x00;//sensor mode
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
}

////////////////////
//Drive Functions//
//////////////////

static void nxt_set_motor(int speed, int motor)
{

	if(motor > 2){
		return;
	}
	
	char directCommandBuffer[256];

		directCommandBuffer[0] = 0x80;// no return
		directCommandBuffer[1] = 0x04;//set ouput state command
		directCommandBuffer[2] = motor;//port
		directCommandBuffer[3] = speed;//power
		directCommandBuffer[4] = 0x01;//mode - motor on
		directCommandBuffer[5] = 0x00;//regulation mode - none
		directCommandBuffer[6] = 0x00;//turn ratio
		directCommandBuffer[7] = 0x20;//run state - running
		directCommandBuffer[8] = 0x00;//tach state - run forever
		directCommandBuffer[9] = 0x00;//tach state - run forever
		directCommandBuffer[10] = 0x00;//tach state - run forever
		directCommandBuffer[11] = 0x00;//tach state - run forever
		directCommandBuffer[12] = 0x00;//tach state - run forever

	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 13, 0x0BB8);
	return;
}

static void nxt_set_motor_with_limit(int speed, int limit, int motor)
{

	if(motor > 2){
		return;
	}
	
	char directCommandBuffer[256];

		directCommandBuffer[0] = 0x80;// no return
		directCommandBuffer[1] = 0x04;//set ouput state command
		directCommandBuffer[2] = motor;//port
		directCommandBuffer[3] = speed;//power
		directCommandBuffer[4] = 0x01;//mode - motor on
		directCommandBuffer[5] = 0x00;//regulation mode - none
		directCommandBuffer[6] = 0x00;//turn ratio
		directCommandBuffer[7] = 0x20;//run state - running
		directCommandBuffer[8] = limit;//tach state - run forever
		directCommandBuffer[9] = 0x00;//tach state - run forever
		directCommandBuffer[10] = 0x00;//tach state - run forever
		directCommandBuffer[11] = 0x00;//tach state - run forever
		directCommandBuffer[12] = 0x00;//tach state - run forever

	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 13, 0x0BB8);
	return;
}

////////////////////
//Sonar Functions//
//////////////////

static void openSonarOnPort(int port){
	unsigned char directCommandBuffer[256];
	directCommandBuffer[0] = 0x80;//no return
	directCommandBuffer[1] = 0x05;//set input mode command
	directCommandBuffer[2] = port;//port
	directCommandBuffer[3] = 0x0B;//sensor type - LOWSPEED_V9
	directCommandBuffer[4] = 0x00;//sensor mode - raw mode
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	
	//unsigned char directReadBuffer[256];
	//usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer), 3, 0x0BB8);
	
	return;
}

//this function is allready set on sonar by defualt so it isnt needed, is just here in case of "whatever"
static void setSonorContinuousReadMode(unsigned char port){
	unsigned char directCommandBuffer2[256];
	directCommandBuffer2[0] = 0x80;//no return
	directCommandBuffer2[1] = 0x0f;//command - lswrite
	directCommandBuffer2[2] = port;//port
	directCommandBuffer2[3] = 0x03;//outgoing package size
	directCommandBuffer2[4] = 0x00;//returning package size (from sonar to nxt, not from nxt to me)
	directCommandBuffer2[5] = 0x02;//package 1 - from sonar docs - set continus mode
	directCommandBuffer2[6] = 0x41;//package 2
	directCommandBuffer2[7] = 0x02;//package 3

	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer2), 8, 0x0BB8);
	//unsigned char directReadBuffer2[256];
	//usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer2), 3, 0x0BB8);
		
}

static int getSonarValue(int port){
	int bytesRead = 0;
	do{
		unsigned char directCommandBuffer21[256];
		directCommandBuffer21[0] = 0x80;//no return
		directCommandBuffer21[1] = 0x0F;//command - lsread
		directCommandBuffer21[2] = port;//port
		directCommandBuffer21[3] = 0x02;//send size
		directCommandBuffer21[4] = 0x01;//return size
		directCommandBuffer21[5] = 0x02;//send 1
		directCommandBuffer21[6] = 0x42;//send 2
		
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer21), 7, 0x0BB8);
		//unsigned char directReadBuffer21[256];
		//usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer21), 3, 0x0BB8);
		
	
		unsigned char directCommandBuffer211[256];
		directCommandBuffer211[0] = 0x00;//return
		directCommandBuffer211[1] = 0x0E;//command - lsget status
		directCommandBuffer211[2] = port;//port
		
		usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer211), 3, 0x0BB8);
		
		unsigned char directReadBuffer211[256];
		usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer211), 4, 0x0BB8);
		
		bytesRead = (int)directReadBuffer211[3];
		
	}while(bytesRead < 1);
	
	unsigned char directCommandBuffer22[256];
	directCommandBuffer22[0] = 0x00;//return
	directCommandBuffer22[1] = 0x10;//command - lsread
	directCommandBuffer22[2] = port;//port
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
	
	unsigned char directReadBuffer22[256];
	usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 20, 0x0BB8);
	//std::cout << "bit #" << 4 << " contents = " << std::hex << (int)directReadBuffer22[4] << std::endl;
	return (int)directReadBuffer22[4];
}

///////////////////////////
//Light Sensor Functions//
/////////////////////////

static void openLightActiveOnPort(unsigned char port){
	//std::cout << "set sonar ports" << std::endl;
	unsigned char directCommandBuffer[256];
	directCommandBuffer[0] = 0x80;//no return
	directCommandBuffer[1] = 0x05;//command
	directCommandBuffer[2] = port;//port
	directCommandBuffer[3] = 0x05;//sensor type
	directCommandBuffer[4] = 0x80;//sensor mode
	
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
}

static void openLightPassiveOnPort(unsigned char port){
	//std::cout << "set sonar ports" << std::endl;
	unsigned char directCommandBuffer[256];
	directCommandBuffer[0] = 0x80;//no return
	directCommandBuffer[1] = 0x05;//command
	directCommandBuffer[2] = port;//port
	directCommandBuffer[3] = 0x06;//sensor type
	directCommandBuffer[4] = 0x80;//sensor mode
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
}

static int getLightValue(unsigned char port){
	
	unsigned char directCommandBuffer22[256];
	directCommandBuffer22[0] = 0x00;//return
	directCommandBuffer22[1] = 0x07;//command - lsread
	directCommandBuffer22[2] = port;//port
	
	unsigned char directReadBuffer22[256];
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
	usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
	return directReadBuffer22[12];
}

///////////////////////////
//Sound Sensor Functions//
/////////////////////////

static void openSoundOnPort(unsigned char port){
	//std::cout << "set sonar ports" << std::endl;
	unsigned char directCommandBuffer[256];
	directCommandBuffer[0] = 0x80;//no return
	directCommandBuffer[1] = 0x05;//command
	directCommandBuffer[2] = port;//port
	directCommandBuffer[3] = 0x05;//sensor type
	directCommandBuffer[4] = 0x80;//sensor mode
	
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
}
//this is exactly the same as get light value
static int getSoundValue(unsigned char port){
	
	unsigned char directCommandBuffer22[256];
	directCommandBuffer22[0] = 0x00;//return
	directCommandBuffer22[1] = 0x07;//command - lsread
	directCommandBuffer22[2] = port;//port
	
	unsigned char directReadBuffer22[256];
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
	usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
	return directReadBuffer22[12];
}

/////////////////////
//Bumper Functions//
///////////////////

static void openBumperOnPort(int port)
{
	//ViUInt8 directCommandBuffer[] = { 0x05, port, 0x01, 0x20 };
	//Comm::SendDirectCommand( false, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ), NULL, 0);
	unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no nreturn
		directCommandBuffer[1] = 0x05;//set sensor command
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x01;//sensor type (switch)
		directCommandBuffer[4] = 0x20;//sensor mode (boolean)
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 5, 0x0BB8);
	return;
}
//same exact function as get light and sound functions
static int getBumperValue(unsigned char port){
	
	unsigned char directCommandBuffer22[256];
	directCommandBuffer22[0] = 0x00;//return
	directCommandBuffer22[1] = 0x07;//command - lsread
	directCommandBuffer22[2] = port;//port
	
	unsigned char directReadBuffer22[256];
	
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer22), 3, 0x0BB8);
	usb_bulk_read(pUSBHandle, ucEpOut, reinterpret_cast < char *>(directReadBuffer22), 16, 0x0BB8);
	
	return directReadBuffer22[12];
}
*/
