#ifndef CONNECTION
#define CONNECTION

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

#endif /* CONNECTION */
