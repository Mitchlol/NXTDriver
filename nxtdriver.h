#ifndef NXTDRIVER
#define NXTDRIVER

#include <unistd.h>

#include "Connection.cc"
#include "USBConnection.cc"
//#include "BTConnection.cc"

#include <libplayercore/playercore.h>


////////////////////////////////////////////////////////////////////////////////
// The class for the driver
class NXTDriver : public ThreadedDriver
{
  public:
    
    // Constructor; need that
    NXTDriver(ConfigFile* cf, int section);

    // Must implement the following methods.
    //virtual int Setup();
    //virtual int Shutdown();

    // This method will be invoked on each incoming message
    virtual int ProcessMessage(QueuePointer &resp_queue, 
                               player_msghdr * hdr,
                               void * data);

  private:

    // Main function for device thread.
    virtual void Main();
    virtual int MainSetup();
    virtual void MainQuit();
    virtual void UpdateSonars();
    virtual void UpdateLightSensors();
    virtual void UpdateSoundSensors();
    virtual void UpdateBumpers();
	
	player_devaddr_t position_addr; ///< Address of the position device (wheels odometry)
	player_devaddr_t sonar_addr; ///< Address of the sonar device
	player_devaddr_t light_addr; ///< Address of the light device
	player_devaddr_t sound_addr; ///< Address of the sound device
	player_devaddr_t bumper_addr; ///< Address of the bumper device
	player_devaddr_t gripper_addr; ///< Address of the bumper device
	
    int foop;
    bool diffdrive;
    int rightwheel;
    int leftwheel;
    int sonarcount;
    int *sonars;
    int lightsensorcount;
    int *lightsensors;
    int soundsensorcount;
    int *soundsensors;
    int bumpercount;
    int *bumpers;
    int gripperport;
    int gripperangle;

    Connection *myConnection;
};

Driver* NXTDriver_Init(ConfigFile *cf, int section);

void NXTDriver_Register(DriverTable *table);

#endif /* NXTDRIVER */
