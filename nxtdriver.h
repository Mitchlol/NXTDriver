//!The NXT player Stage Driver
/*!This is the main part of the driver, it is based off the example driver included in the player stage source.
 */

#ifndef NXTDRIVER
#define NXTDRIVER

#include <unistd.h>

#include "Connection.cc"
#include "USBConnection.cc"
#include "BTConnection.cc"

#include <libplayercore/playercore.h>


////////////////////////////////////////////////////////////////////////////////
//! The class for the driver
class NXTDriver : public ThreadedDriver
{
  public:
    
    //! Constructor, all the config file parsing is done here.
    NXTDriver(ConfigFile* cf, int section);

    // Must implement the following methods.
    //virtual int Setup();
    //virtual int Shutdown();

    //! This method will be invoked on each incoming message
    virtual int ProcessMessage(QueuePointer &resp_queue, 
                               player_msghdr * hdr,
                               void * data);

  private:

    //! Main function for device thread.
    virtual void Main();
    //! Setup Function.
    virtual int MainSetup();
    //! Quit function, should probably be implemented....
    virtual void MainQuit();
//why are these 4 virtual?????? must remove and test....
    //! Function to update the robots Sonar data
    virtual void UpdateSonars();
    //! Function to update the robots Light Sensor data
    virtual void UpdateLightSensors();
    //! Function to update the robots Sound Sensor data
    virtual void UpdateSoundSensors();
    //! Function to update the robots Bumper data
    virtual void UpdateBumpers();
	
	player_devaddr_t position_addr; ///< Address of the position device (wheels odometry)
	player_devaddr_t sonar_addr; ///< Address of the sonar device
	player_devaddr_t light_addr; ///< Address of the light device
	player_devaddr_t sound_addr; ///< Address of the sound device
	player_devaddr_t bumper_addr; ///< Address of the bumper device
	player_devaddr_t gripper_addr; ///< Address of the bumper device
	
    int foop; 
    bool diffdrive; 		//!< Differential Drive flag, other drives may be implemented at some point.
    int rightwheel; 		//!< Which port is the right wheel (for diff drive).
    int leftwheel;		//!< Which port is the left wheel (for diff drive).
    int sonarcount;		//!< Numper of Sonars.
    int *sonars;		//!< An array of sonar port numbers.
    int lightsensorcount;	//!< Number of Light Sensors.
    int *lightsensors;		//!< An array of Light Sensor port numbers.
    int soundsensorcount;	//!< Number of Sound Sensors.
    int *soundsensors;		//!< An array of Sound Sensor port numbers.
    int bumpercount;		//!< Number of Bumpers.
    int *bumpers;		//!< An array of Bumper Port numbers.
    int gripperport;		//!< Port number for the gripper.
    int gripperangle;		//!< the number of degrees to rotate the gripper back and forth for open and close.
    char *btaddress;		//!< the BT address of the NXT we want to use.

    Connection *myConnection; //!< Connection object used to talk to the NXT it is cast to USB or BT Connection.
};

Driver* NXTDriver_Init(ConfigFile *cf, int section);

void NXTDriver_Register(DriverTable *table);

#endif /* NXTDRIVER */
