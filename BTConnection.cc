// Socket, used for Bluetooth rfcomm 
#include <sys/socket.h>
#include <sys/types.h>

// Bluetooth headers
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Connection.cc"

class BTConnection : public Connection{
	public:

/////////////////////
//BT Connect Stuff//
///////////////////
	int nxtSocket;
	char *btAddress;
	int init_bluetooth(char *btAddress)	{
		struct sockaddr_rc addr={0};
		int status;
 
		/*-----------------------------------------------------------
		 * SOCK_STREAM
		 *	 Provides sequenced, reliable, two-way, connection-based 
		 *	  byte streams. An out-of-band data transmission 
		 *	  mechanism may be supported.
		 *----------------------------------------------------------*/

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
