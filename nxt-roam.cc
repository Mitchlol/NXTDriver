/*
 *  roomba-roam.cc
 * 
 *  Sample code for a robot that has two front bumpers. Suitable for use with
 *  the roomba.
 *
 *  This is example0.cc, changed to (hopefully) make it easier to understand.
 *
 *  Modified: Simon Parsons
 *  Date:     10th September 2009
 *  
 */


#include <iostream>
#include <libplayerc++/playerc++.h>
//#include <cstddef.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	using namespace PlayerCc;

	// Set up proxies
	PlayerClient    robot("localhost");
	Position2dProxy pp(&robot,0);
	std::cout << "After Proxy Setup "<< std::endl;
	// Allow the program to take charge of the motors (take care now)
	pp.SetMotorEnable(true);
	
	std::cout << "After Motor Enable "<< std::endl;
	// Control loop
	while(true) {
		//std::cout << "in while "<< std::endl;
		double turnrate, speed, time;
		
		
		// What are we doing?
		//std::cout << pp << std::endl;
		//std::cout << "Speed: 1" << std::endl;
		//std::cout << "Turn rate: 0" << std::endl << std::endl;
		
		int x;
		int y;
		std::cout << "enter x value:";
		std::cin >> x;
		std::cout << "enter y value:";
		std::cin >> y;
		pp.SetSpeed(x, y);
		
		
		
		
		
		
		// Read from the proxies
		//robot.Read();
		//std::cout << "After Read "<< std::endl;
		// What does odometry tell us?
		//std::cout << pp << std::endl;
    }
  
}


