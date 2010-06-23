/*!\mainpage
 *This is the Player Stage Plugin Driver for the Lego NXT robot.
 *
 *Developed by Metrobotics.
 *\url http://bots.sci.brooklyn.cuny.edu/metrobotics/
 *
 *the NXT driver config file
 *
 *For diff drive (the only currently supported drive mode) \n
 *1)add ""position2d:0"" to the provides section \n
 *2)add "drive "differential"" \n
 *3)add "rightwheel "portletter"" where portletter is A,B, or C \n
 *4)add "leftwheel "portletter"" where portletter is A,B, or C 
 *
 *For sonar sensors \n
 *1)add ""sonar:0"" to the provides section \n
 *2)add "sonarcount "numsonars"" where numsonars is the number of sonars \n
 *3)add "sonarports ["4""2"]" if sonars are in ports 2 and 4 \n
 * 
 *For light sensors \n
 *1)add ""light:::ranger:0"" to the provides section you need to increment 0 for multiple rangers(sound, light, etc..) \n
 *2)add "lightsensorcount "numlights"" where numlights is the number of lights \n
 *3)add "lightsensorports ["1""2"]" if sonars are in ports 1 and 2 \n
 *
 *For sound sensors \n
 *1)add ""sound:::ranger:0"" to the provides section you need to increment 0 for multiple rangers(sound, light, etc..) \n
 *2)add "soundsensorcount "numsound"" where numsound is the number of sounds \n
 *3)add "soundsensorports ["1""2""3""4"]" if sonars are in ports 1,2,3, and 4 \n
 *
 *For bumper sensors \n
 *1)add ""bumper:0"" to the provides section you need to increment 0 for multiple rangers(sound, light, etc..) \n
 *2)add "bumpercount "numbumper"" where numbumper is the number of bumpers \n
 *3)add "bumperports ["4""2""3""1"]" if sonars are in ports 1,2,3, and 4 \n
 *
 *in case you didnt notice order of sensors dosnt matter player will recognise them in the order you list them here...
 *
 *For Gripper \n
 *1)add "gripperport "portletter"" where portletter is A,B, or C  \n
 *2)add "gripperangle "integer"" where integer represents how many degrees the motor needs to turn to open or close the gripper \n
 *if open and close are reversed, use a negative angle or recunstruct gripper. (open and close can reverse based on how you build your gripper!)
 *
 *For Bluetooth \n
 *1)add "btaddress "btaddress"" \n
 *for mitchxt it is "btaddress "00:16:53:02:D5:C8"" \n
 *without a btaddress feild the driver will default to usb \n
 *
 */



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
