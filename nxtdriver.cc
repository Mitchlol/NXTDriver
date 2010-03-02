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
    : ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{
	
	memset(&this->position_addr, 0, sizeof(player_devaddr_t));
	memset(&this->sonar_addr, 0, sizeof(player_devaddr_t));
	memset(&this->light_addr, 0, sizeof(player_devaddr_t));
	memset(&this->sound_addr, 0, sizeof(player_devaddr_t));
	memset(&this->bumper_addr, 0, sizeof(player_devaddr_t));
	memset(&this->gripper_addr, 0, sizeof(player_devaddr_t));
	
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
      
	// Do we create a sonar interface?
	if(cf->ReadDeviceAddr(&(this->sonar_addr), section, "provides", PLAYER_SONAR_CODE, -1, NULL) == 0)
	{
		if(this->AddInterface(this->sonar_addr) != 0)
		{
			PLAYER_ERROR("Could not add Sonar interface for NXT");	    	
		 	this->SetError(-1);
		 	return;
	    }
	    puts("added Sonar interface to NXT");
	}
	
	
	// Do we create a light ranger interface?
	//example "light:::ranger:0" ("ranger:0" WILL NOT WORK!)
	if(cf->ReadDeviceAddr(&(this->light_addr), section, "provides", PLAYER_RANGER_CODE, -1, "light" ) == 0)
	{
		if(this->AddInterface(this->light_addr) != 0)
		{
			PLAYER_ERROR("Could not add Light interface for NXT");	    	
		 	this->SetError(-1);
		 	return;
	    }
	    puts("added Light interface to NXT");
	}
	
	
	// Do we create a sound sound interface?
	//example "sound:::ranger:0" ("ranger:0" WILL NOT WORK!)
	if(cf->ReadDeviceAddr(&(this->sound_addr), section, "provides", PLAYER_RANGER_CODE, -1,  "sound" ) == 0)
	{
		if(this->AddInterface(this->sound_addr) != 0)
		{
			PLAYER_ERROR("Could not add Sound interface for NXT");	    	
		 	this->SetError(-1);
		 	return;
	    }
	    puts("added Sound interface to NXT");
	}
	
	//do we create a bumper interface?
	if(cf->ReadDeviceAddr(&(this->bumper_addr), section, "provides", PLAYER_BUMPER_CODE, -1, NULL) == 0)
	{

		if(this->AddInterface(this->bumper_addr) != 0)
		{
			PLAYER_ERROR("Could not add Bumper interface for NXT");	
			this->SetError(-1);
			return;
		}
  		puts("added Bumper interface to NXT");
	}
	
	// Do we create a gripper interface?
	if(cf->ReadDeviceAddr(&(this->gripper_addr), section, "provides", PLAYER_GRIPPER_CODE, -1, NULL) == 0)
	{
		if(this->AddInterface(this->gripper_addr) != 0)
		{
			PLAYER_ERROR("Could not add Gripper interface for NXT");
			this->SetError(-1);
			return;
		}
		puts("added Gripper interface to NXT");
	}
	
	
	// Read an option from the configuration file
	this->foop = cf->ReadInt(section, "foo", 0);
	//check for diofferential drive
	const char* drive = cf->ReadString(section, "drive", "none");
	if(strcmp(drive,"none") == 0){
		this->diffdrive = false;
		puts("no drive mode");
	}else if(strcmp(drive,"differential") == 0){
		this->diffdrive = true;
		puts("diff drive mode");
		
		//read wheels
		//rightwheel
		const char* rwheel = cf->ReadString(section, "rightwheel", "none");
		if(strcmp(rwheel,"none") == 0){
			this->rightwheel = -1;
			puts("no right wheel");
		}else if(strcmp(rwheel,"A") == 0){
			this->rightwheel = 0;
			puts("right wheel connected to port A");
		}else if(strcmp(rwheel,"B") == 0){
			this->rightwheel = 1;
			puts("right wheel connected to port B");
		}else if(strcmp(rwheel,"C") == 0){
			this->rightwheel = 2;
			puts("right wheel connected to port C");
		}else{
			this->rightwheel = -1;
		}
		//leftwheel
		const char* lwheel = cf->ReadString(section, "leftwheel", "none");
		if(strcmp(lwheel,"none") == 0){
			this->leftwheel = -1;
			puts("no right wheel");
		}else if(strcmp(lwheel,"A") == 0){
			this->leftwheel = 0;
			puts("right wheel connected to port A");
		}else if(strcmp(lwheel,"B") == 0){
			this->leftwheel = 1;
			puts("right wheel connected to port B");
		}else if(strcmp(lwheel,"C") == 0){
			this->leftwheel = 2;
			puts("right wheel connected to port C");
		}else{
			this->leftwheel = -1;
		}
		
	}else{
		this->diffdrive = false;
	}
	
	
	
	
	//check for sonars
	this->sonarcount = cf->ReadInt(section, "sonarcount", 0);
	if(this->sonarcount == 0){
		puts("no sonars");
	}else if(this->sonarcount > 0){
		printf("%d sonars\n", this->sonarcount);
		this->sonars = new int[sonarcount];
		//int portscluster = cf->ReadInt(section, "sonarcount", 0);
		//int multiple = 0;
		for(int i = 0; i < sonarcount; i++){
			this->sonars[i] = cf->ReadTupleInt(section, "sonarports", i, 0);
			printf("sonars #%d", i);
			printf(" in port #%d\n", this->sonars[i]);
			//decremnt sonar port# to make it 0 based
			this->sonars[i]--;
		}
		
	}
	
	//check for light sensors
	this->lightsensorcount = cf->ReadInt(section, "lightsensorcount", 0);
	if(this->lightsensorcount == 0){
		puts("no light sensors");
	}else if(this->lightsensorcount > 0){
		printf("%d light sensors\n", this->lightsensorcount);
		
		this->lightsensors = new int[lightsensorcount];
		for(int i = 0; i < lightsensorcount; i++){
			this->lightsensors[i] = cf->ReadTupleInt(section, "lightsensorports", i, 0);
			printf("lightsensor #%d", i);
			printf(" in port #%d\n", this->lightsensors[i]);
			//decremnt lightsensor port# to make it 0 based
			this->lightsensors[i]--;
		}
		
	}
	
	//check for sound sensors
	this->soundsensorcount = cf->ReadInt(section, "soundsensorcount", 0);
	if(this->soundsensorcount == 0){
		puts("no sound");
	}else if(this->soundsensorcount > 0){
		printf("%d sound sensors\n", this->soundsensorcount);
		this->soundsensors = new int[soundsensorcount];
		for(int i = 0; i < soundsensorcount; i++){
			this->soundsensors[i] = cf->ReadTupleInt(section, "soundsensorports", i, 0);
			printf("soundsensors #%d", i);
			printf(" in port #%d\n", this->soundsensors[i]);
			//decremnt soundsensor port# to make it 0 based
			this->soundsensors[i]--;
		}
		
	}
	
	//check for bumper sensors
	this->bumpercount = cf->ReadInt(section, "bumpercount", 0);
	if(this->bumpercount == 0){
		puts("no bumper");
	}else if(this->bumpercount > 0){
		printf("%d bumpers\n", this->bumpercount);
		this->bumpers = new int[bumpercount];
		for(int i = 0; i < bumpercount; i++){
			this->bumpers[i] = cf->ReadTupleInt(section, "bumperports", i, 0);
			printf("bumpers #%d", i);
			printf(" in port #%d\n", this->bumpers[i]);
			//decremnt soundsensor port# to make it 0 based
			this->bumpers[i]--;
		}
		
	}
	
	//check for gripper values
	const char* gripperportchar = cf->ReadString(section, "gripperport", "none");
		if(strcmp(gripperportchar,"none") == 0){
			this->gripperport = -1;
			puts("no gripper");
		}else if(strcmp(gripperportchar,"A") == 0){
			this->gripperport = 0;
			puts("gripper connected to port A");
		}else if(strcmp(gripperportchar,"B") == 0){
			this->gripperport = 1;
			puts("gripper connected to port B");
		}else if(strcmp(gripperportchar,"C") == 0){
			this->gripperport = 2;
			puts("gripper connected to port C");
		}else{
			this->gripperport = -1;
		}
	this->gripperangle = cf->ReadInt(section, "gripperangle", 0);
	if(this->gripperangle == 0){
		puts("no gripper angle, no gripper....");
		this->gripperport = -1;
	}else if(this->gripperangle > 0){
		printf("Gripper Angle = %d\n", this->gripperangle);
	}
	
	
	// Message for checking status:
	puts("done parsing config");
	return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int NXTDriver::MainSetup()
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
		//tell nxt which sonars are where
		//if sonar count is 0 this loop will have 0 iterations :D
		for(int i = 0; i < sonarcount; i++){
			openSonarOnPort(this->sonars[i]);
		}
		//same for light sensors
		for(int i = 0; i < lightsensorcount; i++){
			openLightActiveOnPort(this->lightsensors[i]);
		}
		//same for sound sensors
		for(int i = 0; i < soundsensorcount; i++){
			openSoundOnPort(this->soundsensors[i]);
		}
		//same for bumpers
		for(int i = 0; i < bumpercount; i++){
			openBumperOnPort(this->bumpers[i]);
		}
	    
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
void NXTDriver::MainQuit()
{
  puts("Shutting NXT driver down");

  // Stop and join the driver thread
  //StopThread();

  // Here you would shut the device down by, for example, closing a
  // serial port.

  puts("nxt driver has been shutdown");

  return;
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

		//nxt_set_motor(position_cmd.vel.px, 0);
		if(this->diffdrive && this->rightwheel != -1 && this->leftwheel != -1){
			if(abs(position_cmd.vel.px) <= 100 && abs(position_cmd.vel.pa) <= 100){
				//int x = (int)(position_cmd.vel.px * 20 * 10);
				//int a = (int)(position_cmd.vel.pa * 20 * 10);
				int x = position_cmd.vel.px;
				int a = position_cmd.vel.pa;
				//char* temp = a;
				//puts(a);
				//case 1 and 2
				//go foward or back if angle == 0
				if(a == 0){
					nxt_set_motor(x, rightwheel);
					nxt_set_motor(x, leftwheel);
				}
				//case 3 and 4
				//go left or right
				else if(x == 0){
					//if angle is > 0 go right
					if(a > 0){
						nxt_set_motor(-abs(a), rightwheel);
						nxt_set_motor(abs(a), leftwheel);
					}
					//if angle is < 0 go left
					if(a < 0){
						nxt_set_motor(abs(a), rightwheel);
						nxt_set_motor(-abs(a), leftwheel);
					}
				}
				//case 5 6 7 8 go diagnol
				//case 5 go foward right
				else if(x > 0 && a > 0){
					nxt_set_motor(x - (x-a), rightwheel);
					nxt_set_motor(x - (x-(a/2)), leftwheel);
				}
				//case 6 go foward left
				else if(x > 0 && a < 0){
					nxt_set_motor(x - (x-(a/2)), rightwheel);
					nxt_set_motor(x - (x-a), leftwheel);
				}
				//case 7 go back right
				else if(x > 0 && a > 0){
					nxt_set_motor(x + (x-(a/2)), rightwheel);
					nxt_set_motor(x + (x-a), leftwheel);
				}
				//case 8 go back left
				else if(x > 0 && a > 0){
					nxt_set_motor(x + (x-a), rightwheel);
					nxt_set_motor(x + (x-(a/2)), leftwheel);
				}
			}
		}
		
		////////////////////////////
		// Update position2d data;
		player_position2d_data_t posdata;
		memset(&posdata, 0, sizeof(posdata));

		//this should update from a get motor state command not from the p2dsetmorotr 
		posdata.vel.px = position_cmd.vel.px;
		posdata.vel.pa = position_cmd.vel.pa;

		this->Publish(this->position_addr, PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE, (void*) &posdata, sizeof(posdata), NULL);
		//         printf("\nCARLOS: after Publishing()\n");
		
		return 0;
	}
	
	// Messages from the ranger interface
	// Power config request
	if (Message::MatchMessage (hdr, PLAYER_MSGTYPE_REQ, PLAYER_RANGER_REQ_POWER, device_addr))
	{
		//maybe should check id device_addr is equal to lightaddr, dont wanna make light passive on request to turn off sound sensnors
		int state = reinterpret_cast<player_ranger_power_config_t*> (data)->state;
		//printf("Power State Request = %d\n", reinterpret_cast<player_ranger_power_config_t*> (data)->state);
		if(state == 1){
			for(int i = 0; i < lightsensorcount; i++){
				openLightActiveOnPort(this->lightsensors[i]);
			}
		}else if(state == 0){
			for(int i = 0; i < lightsensorcount; i++){
				openLightPassiveOnPort(this->lightsensors[i]);
			}
		}
		//not sure if im publishing the right message, but it works so????
		//bleah this code is lame...
		//will anyone ever read this comment anyway?
		this->Publish(this->light_addr, PLAYER_MSGTYPE_RESP_ACK, PLAYER_RANGER_REQ_POWER, NULL, 0, NULL);
		return 0;
	}
	
	//Match messages for gripper interface!
	if (Message::MatchMessage (hdr, PLAYER_MSGTYPE_CMD, PLAYER_GRIPPER_CMD_OPEN, this->gripper_addr))
	{
		if(gripperangle > 0){
			nxt_set_motor(100, gripperport);
			nxt_set_motor_with_limit(100, gripperangle, gripperport);
		}else{
			nxt_set_motor(-100, gripperport);
			nxt_set_motor_with_limit(-100, -gripperangle, gripperport);
		}
		player_gripper_data_t data2;
		// put gripper data
  		this->Publish(this->gripper_addr, PLAYER_MSGTYPE_DATA, PLAYER_GRIPPER_DATA_STATE, (void*)&(data2), sizeof(player_gripper_data_t), NULL);
		return 0;
	}
	if (Message::MatchMessage (hdr, PLAYER_MSGTYPE_CMD, PLAYER_GRIPPER_CMD_CLOSE, this->gripper_addr))
	{
		if(gripperangle > 0){
			nxt_set_motor(-100, gripperport);
			nxt_set_motor_with_limit(-100, gripperangle, gripperport);
		}else{
			nxt_set_motor(100, gripperport);
			nxt_set_motor_with_limit(100, -gripperangle, gripperport);
		}
		player_gripper_data_t data2;
		// put gripper data
  		this->Publish(this->gripper_addr, PLAYER_MSGTYPE_DATA, PLAYER_GRIPPER_DATA_STATE, (void*)&(data2), sizeof(player_gripper_data_t), NULL);
		
		return 0;
	}
  
  
  return(0);
}

void NXTDriver::UpdateSonars()
{
	// put sonar data
	player_sonar_data_t sonardata;
	memset(&sonardata, 0, sizeof(sonardata));
	
	float * ranges = new float[sonarcount];
	for(int i = 0; i < sonarcount; i++){
		ranges[i] = getSonarValue(this->sonars[i]);
	}
	sonardata.ranges_count = sonarcount;
	sonardata.ranges = ranges;
	
	this->Publish(this->sonar_addr, PLAYER_MSGTYPE_DATA, PLAYER_SONAR_DATA_RANGES, (void*)&sonardata, sizeof(sonardata), NULL);
	
}

void NXTDriver::UpdateLightSensors()
{
	// put light data
	player_ranger_data_range_t lightdata;
	memset(&lightdata, 0, sizeof(lightdata));
	
	double * ranges = new double[lightsensorcount];
	for(int i = 0; i < lightsensorcount; i++){
		ranges[i] = getLightValue(this->lightsensors[i]);
	}
	lightdata.ranges_count = lightsensorcount;
	lightdata.ranges = ranges;
	
	this->Publish(this->light_addr, PLAYER_MSGTYPE_DATA, PLAYER_RANGER_DATA_RANGE, reinterpret_cast<void*>(&lightdata), sizeof(lightdata), NULL);
	
}

void NXTDriver::UpdateSoundSensors()
{
	// put light data
	player_ranger_data_range_t sounddata;
	memset(&sounddata, 0, sizeof(sounddata));
	
	double * ranges = new double[soundsensorcount];
	for(int i = 0; i < soundsensorcount; i++){
		ranges[i] = getSoundValue(this->soundsensors[i]);
	}
	sounddata.ranges_count = soundsensorcount;
	sounddata.ranges = ranges;
	
	this->Publish(this->sound_addr, PLAYER_MSGTYPE_DATA, PLAYER_RANGER_DATA_RANGE, reinterpret_cast<void*>(&sounddata), sizeof(sounddata), NULL);
	
}

void NXTDriver::UpdateBumpers()
{
     player_bumper_data_t bumperdata;
     memset(&bumperdata,0,sizeof(bumperdata));	
	uint8_t * bumpervalues = new uint8_t[bumpercount];
	for(int i = 0; i < bumpercount; i++){
		bumpervalues[i] = getBumperValue(this->bumpers[i]);
	}
     
     bumperdata.bumpers_count = bumpercount;
	bumperdata.bumpers =  bumpervalues;

	this->Publish(this->bumper_addr, PLAYER_MSGTYPE_DATA, PLAYER_BUMPER_DATA_STATE, (void*)&bumperdata);	
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
	if(sonarcount > 0){
		UpdateSonars();
	}
	if(lightsensorcount > 0){
		UpdateLightSensors();
	}
	if(soundsensorcount > 0){
		UpdateSoundSensors();
	}
	if(bumpercount > 0){
		UpdateBumpers();
	}
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

