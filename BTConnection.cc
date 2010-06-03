//!BTConnection class is the child of the Connection class used for Bluetooth communitcation.
/*!
 *This is the Bluetooth version of the Connection class. It uses the same Direct messages as USBConnection.
 *Except that it first sends a 2 byte packet containing the length of the packet about to be sent.
 *See the USBConnection class for more documentation.
 *
 *It should be 100% functional when it is cast to a connection or vice versa....
 *@see Connection
 *@see USBConnection	
 */



// Socket, used for Bluetooth rfcomm 
#include <sys/socket.h>
#include <sys/types.h>

// Bluetooth headers
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

//#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Connection.cc"



class BTConnection : public Connection{

/////////////////////
//BT Connect Stuff//
///////////////////
private:
	int nxtSocket;
	char *btAddress;
//!Bluetooth Connection Function
/*!This is a function to open a bluetooth connection. Coppied from Vikram Aggarwal's BT sample code.
 */
	int init_bluetooth(char *btAddress){
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

public:
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
	}
//////////////////////
//Generic Functions//
////////////////////

	void closeSensorOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;

		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 5;
			btLength[1]= 0x00;
		//message
		char directCommandBuffer[256];
			directCommandBuffer[0] = 0x80;//no return
			directCommandBuffer[1] = 0x05;//command
			directCommandBuffer[2] = port;//port
			directCommandBuffer[3] = 0x00;//sensor type -- no sensor
			directCommandBuffer[4] = 0x00;//sensor mode
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
		return;
	}

////////////////////
//Drive Functions//
//////////////////

	void nxt_set_motor(int speed, int motor)
	{

		if(motor > 2){
			return;
		}
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 13;
			btLength[1]= 0x00;
		//message
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
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
		return;
	}

	void nxt_set_motor_with_limit(int speed, int limit, int motor)
	{

		if(motor > 2){
			return;
		}
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 13;
			btLength[1]= 0x00;
		//message
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
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
		return;
	}
////////////////////
//Sonar Functions//
//////////////////

	void openSonarOnPort(int port){
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 5;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no return
		directCommandBuffer[1] = 0x05;//set input mode command
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x0B;//sensor type - LOWSPEED_V9
		directCommandBuffer[4] = 0x00;//sensor mode - raw mode
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
		return;
	}

	//this function is allready set on sonar by defualt so it isnt needed, is just here in case of "whatever"
	void setSonorContinuousReadMode(unsigned char port){
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 8;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
		directCommandBuffer[0] = 0x80;//no return
		directCommandBuffer[1] = 0x0f;//command - lswrite
		directCommandBuffer[2] = port;//port
		directCommandBuffer[3] = 0x03;//outgoing package size
		directCommandBuffer[4] = 0x00;//returning package size (from sonar to nxt, not from nxt to me)
		directCommandBuffer[5] = 0x02;//package 1 - from sonar docs - set continus mode
		directCommandBuffer[6] = 0x41;//package 2
		directCommandBuffer[7] = 0x02;//package 3
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
	}

	int getSonarValue(int port){
		int bytesRead = 0;
		do{
			// Bluetooth length
			unsigned char btLength1[2];
				btLength1[0]= 7;
				btLength1[1]= 0x00;
			//message
			unsigned char directCommandBuffer1[256];
				directCommandBuffer1[0] = 0x80;//no return
				directCommandBuffer1[1] = 0x0F;//command - lsread
				directCommandBuffer1[2] = port;//port
				directCommandBuffer1[3] = 0x02;//send size
				directCommandBuffer1[4] = 0x01;//return size
				directCommandBuffer1[5] = 0x02;//send 1
				directCommandBuffer1[6] = 0x42;//send 2
			//write		
			write(nxtSocket, btLength1, 2);
			write(nxtSocket, directCommandBuffer1, btLength1[0]);
		
			// Bluetooth length
			unsigned char btLength2[2];
				btLength2[0]= 3;
				btLength2[1]= 0x00;
			//message
			unsigned char directCommandBuffer2[256];
				directCommandBuffer2[0] = 0x00;//return
				directCommandBuffer2[1] = 0x0E;//command - lsget status
				directCommandBuffer2[2] = port;//port
			//write		
			write(nxtSocket, btLength2, 2);
			write(nxtSocket, directCommandBuffer2, btLength2[0]);
		
			unsigned char directReadBuffer1[256];
			read(nxtSocket, directReadBuffer1, 2);
			read(nxtSocket, directReadBuffer1, 4);
		
			bytesRead = (int)directReadBuffer1[3];
			
		
		}while(bytesRead < 1);
		// Bluetooth length
		unsigned char btLength3[2];
			btLength3[0]= 3;
			btLength3[1]= 0x00;
		//message
		unsigned char directCommandBuffer3[256];
			directCommandBuffer3[0] = 0x00;//return
			directCommandBuffer3[1] = 0x10;//command - lsread
			directCommandBuffer3[2] = port;//port
	
		//write		
		write(nxtSocket, btLength3, 2);
		write(nxtSocket, directCommandBuffer3, btLength3[0]);
	
		unsigned char directReadBuffer2[256];
		read(nxtSocket, directReadBuffer2, 2);
		read(nxtSocket, directReadBuffer2, 20);
		return (int)directReadBuffer2[4];
	}
///////////////////////////
//Light Sensor Functions//
/////////////////////////

	void openLightActiveOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 5;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x80;//no return
			directCommandBuffer[1] = 0x05;//command
			directCommandBuffer[2] = port;//port
			directCommandBuffer[3] = 0x05;//sensor type
			directCommandBuffer[4] = 0x80;//sensor mode
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
	}

	void openLightPassiveOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 5;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x80;//no return
			directCommandBuffer[1] = 0x05;//command
			directCommandBuffer[2] = port;//port
			directCommandBuffer[3] = 0x06;//sensor type
			directCommandBuffer[4] = 0x80;//sensor mode
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
	}

	int getLightValue(unsigned char port){
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 3;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x00;//return
			directCommandBuffer[1] = 0x07;//command - lsread
			directCommandBuffer[2] = port;//port
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
		
		unsigned char directReadBuffer[256];
		read(nxtSocket, directReadBuffer, 2);
		read(nxtSocket, directReadBuffer, 16);
	
		return directReadBuffer[12];
	}

///////////////////////////
//Sound Sensor Functions//
/////////////////////////

	void openSoundOnPort(unsigned char port){
		//std::cout << "set sonar ports" << std::endl;
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 5;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x80;//no return
			directCommandBuffer[1] = 0x05;//command
			directCommandBuffer[2] = port;//port
			directCommandBuffer[3] = 0x05;//sensor type
			directCommandBuffer[4] = 0x80;//sensor mode
	
	
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
		return;
	}
	//this is exactly the same as get light value
	int getSoundValue(unsigned char port){
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 3;
			btLength[1]= 0x00;
		//messag
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x00;//return
			directCommandBuffer[1] = 0x07;//command - lsread
			directCommandBuffer[2] = port;//port
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);

		unsigned char directReadBuffer[256];
		read(nxtSocket, directReadBuffer, 2);
		read(nxtSocket, directReadBuffer, 16);
	
		return directReadBuffer[12];
	}


/////////////////////
//Bumper Functions//
///////////////////

	void openBumperOnPort(int port)
	{
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 5;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x80;//no nreturn
			directCommandBuffer[1] = 0x05;//set sensor command
			directCommandBuffer[2] = port;//port
			directCommandBuffer[3] = 0x01;//sensor type (switch)
			directCommandBuffer[4] = 0x20;//sensor mode (boolean)
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);
		return;
	}
	//same exact function as get light and sound functions
	int getBumperValue(unsigned char port){
		// Bluetooth length
		unsigned char btLength[2];
			btLength[0]= 3;
			btLength[1]= 0x00;
		//message
		unsigned char directCommandBuffer[256];
			directCommandBuffer[0] = 0x00;//return
			directCommandBuffer[1] = 0x07;//command - lsread
			directCommandBuffer[2] = port;//port
		//write		
		write(nxtSocket, btLength, 2);
		write(nxtSocket, directCommandBuffer, btLength[0]);

		unsigned char directReadBuffer[256];
		read(nxtSocket, directReadBuffer, 2);
		read(nxtSocket, directReadBuffer, 16);
	
		return directReadBuffer[12];
	}
};
