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
	SonarProxy      sp(&robot,0);
	RangerProxy	 lrp(&robot,0);
	RangerProxy	 srp(&robot,1);
	BumperProxy	 bp(&robot,0);
	GripperProxy	 gp(&robot,0);
	std::cout << "After Proxy Setup "<< std::endl;
	// Allow the program to take charge of the motors (take care now)
	pp.SetMotorEnable(true);
	
	std::cout << "After Motor Enable "<< std::endl;
	// Control loop
	int x = 0;
	while(true) {
		//std::cout << "in while "<< std::endl;
		double turnrate, speed, time;
		/*
		x--;
		if (x <= 0){
			x=100;
		}
		*/
		// What are we doing?
		//std::cout << pp << std::endl;
		//std::cout << "Speed: 1" << std::endl;
		//std::cout << "Turn rate: 0" << std::endl << std::endl;
		
		//int x;
		//int y;
		//std::cout << "enter x value:";
		//std::cin >> x;
		//std::cout << "enter y value:";
		//std::cin >> y;
		//pp.SetSpeed(x, 0);
		
		robot.Read();
		//std::cout << "loop end, x = " << x << " y = " << 0 << ", sonar = " << sp[0] << " "<< sp[1] << " "<< sp[2] << " "<< sp[3] << std::endl;
		//std::cout << "Bumper = " << bp[0] << " "<< bp[1] << " " << bp[2] << " "<< bp[3] << std::endl;
		//std::cout << "sonar = " << sp[0];
		//std::cout << " sound =  "<< srp[0];
		//std::cout << " light =  "<< lrp[0];
		std::cout << " BUMPER =  "<< bp[0] << " " << bp[1] << std::endl;
		
		if(bp[0] == 1){
			gp.Open();
		}
		if(bp[1] == 1){
			gp.Close();
		}
		
		
		
		// Read from the proxies
		//robot.Read();
		//std::cout << "After Read "<< std::endl;
		// What does odometry tell us?
		//std::cout << pp << std::endl;

    }
  
}


