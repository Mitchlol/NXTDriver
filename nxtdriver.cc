/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2003  
 *     Brian Gerkey
 *                      
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * A simple example of how to write a driver that will be built as a
 * shared object.
 */

// ONLY if you need something that was #define'd as a result of configure 
// (e.g., HAVE_CFMAKERAW), then #include <config.h>, like so:


#include "nxtdriver.h"
#include "nxtcomms.cc"



#include <unistd.h>
#include <string.h>

#include <libplayercore/playercore.h>



// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* 
NXTDriver_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return((Driver*)(new NXTDriver(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void NXTDriver_Register(DriverTable* table)
{
  table->AddDriver("NXTDriver", NXTDriver_Init);
}

////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
NXTDriver::NXTDriver(ConfigFile* cf, int section)
    : Driver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{
	
	memset(&this->position_addr, 0, sizeof(player_devaddr_t));
	
	// Create a position?
	if (cf->ReadDeviceAddr(&(this->position_addr), section, "provides", PLAYER_POSITION2D_CODE, -1, NULL) == 0)
	{
		if (this->AddInterface(this->position_addr) != 0)
		{
			PLAYER_ERROR("Could not add Position2D interface for NXT");
			this->SetError(-1);
			return;
		}
		puts("added Position2D interface to NXT");
      }
	
	
	// Read an option from the configuration file
	this->foop = cf->ReadInt(section, "foo", 0);

	// Message for checking status:
	puts("Constructor is done!");

	return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int NXTDriver::Setup()
{   
  puts("NXT driver initialising");

  // Here you do whatever is necessary to setup the device, like open and
  // configure a serial port.
	struct usb_device *DevInit();
	DevOpen();
	if (pUSBHandle == 0){
		puts("pUSBHandle = 0");
		return -1;
	}
	
	if(pUSBHandle){
		puts("got pUSBHandle");
		printf("Was foo option given in config file? %d\n", this->foop);
	    
		puts("nxt driver ready");

		// Start the device thread; spawns a new thread and executes
		// NXTDriver::Main(), which contains the main loop for the driver.
		StartThread();

		return(0);
	}
	puts("some sort of usb error");
	return -1;

}


////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
int NXTDriver::Shutdown()
{
  puts("Shutting NXT driver down");

  // Stop and join the driver thread
  StopThread();

  // Here you would shut the device down by, for example, closing a
  // serial port.

  puts("nxt driver has been shutdown");

  return(0);
}

int NXTDriver::ProcessMessage(QueuePointer & resp_queue, 
                                  player_msghdr * hdr,
                                  void * data)
{
  // Process messages here.  Send a response if necessary, using Publish().
  // If you handle the message successfully, return 0.  Otherwise,
  // return -1, and a NACK will be sent for you, if a response is required.
  
  
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL, this->position_addr))
	{
		puts("MITCH: I'm Matching Message");

		// position motor command
		player_position2d_cmd_vel_t position_cmd;
		position_cmd = *(player_position2d_cmd_vel_t *) data;
		PLAYER_MSG2(2,"sending motor commands %f %f", position_cmd.vel.px, position_cmd.vel.pa);
		/*
		if (!srv1_set_speed(this->srvdev, position_cmd.vel.px, position_cmd.vel.pa))
		{
			PLAYER_ERROR("failed to set speed on SRV-1");
		}
		*/
		nxt_set_motor(position_cmd.vel.px);
		return 0;
	}
  
  
  
  return(0);
}



////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void NXTDriver::Main() 
{
  // The main loop; interact with the device here
  for(;;)
  {
    // test if we are supposed to cancel
    pthread_testcancel();

    // Process incoming messages.  NXTDriver::ProcessMessage() is
    // called on each message.
    ProcessMessages();

    // Interact with the device, and push out the resulting data, using
    // Driver::Publish()

    // Sleep (you might, for example, block on a read() instead)
    usleep(100000);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C" {
  int player_driver_init(DriverTable* table)
  {
    puts("NXT driver initializing");
    NXTDriver_Register(table);
    puts("NXT driver done");
    return(0);
  }
}

