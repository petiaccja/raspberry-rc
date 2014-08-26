////////////////////////////////////////////////////////////////////////////////
//	File: main.cpp
//	
//	Author: Péter Kardos
//	Created: 2013. june 04.
//		Last changed: ---
////////////////////////////////////////////////////////////////////////////////
//	Content:
//		Contains code for testing of the driver.
////////////////////////////////////////////////////////////////////////////////

int main() {}
/*
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include "servo_drive.h"
#include <random>

#include <cmath>

using namespace std;


ServoDrive driver;
ServoOut* servo;

Timer timer;
unsigned long long time1;

volatile bool hog=true;
unsigned count=0u;

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
 
int getch() {
  struct termios oldt,
                 newt;
  int            ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}



////////////////////////////////////////////////////////////////////////////////
// tester threads
static unsigned pinTable[] {
    RPI_V2_GPIO_P1_03,
    RPI_V2_GPIO_P1_05,
    RPI_V2_GPIO_P1_07,
    RPI_V2_GPIO_P1_08,
    RPI_V2_GPIO_P1_10,
    RPI_V2_GPIO_P1_11,
    RPI_V2_GPIO_P1_12,
    RPI_V2_GPIO_P1_13,
    RPI_V2_GPIO_P1_15,
    RPI_V2_GPIO_P1_16,
    RPI_V2_GPIO_P1_18,
    RPI_V2_GPIO_P1_19,
    RPI_V2_GPIO_P1_21,
    RPI_V2_GPIO_P1_22,
    RPI_V2_GPIO_P1_23,
    RPI_V2_GPIO_P1_24,
    RPI_V2_GPIO_P1_26,

    // RPi Version 2, new plug P5
    RPI_V2_GPIO_P5_03,
    RPI_V2_GPIO_P5_04,
    RPI_V2_GPIO_P5_05,
    RPI_V2_GPIO_P5_06
};

////////////////////////////////////////////////////////////////////////////////
// main
int main(int argc, char* argv[]) {
    servo = driver.AddServo(4);
	
	char c;
	do {
		c=getch();
		switch (c) {
			case 'a':
				servo->SetSteering(0.f);
				break;
			case 'd':
				servo->SetSteering(1.f);
				break;
			default:
				servo->SetSteering(0.5f);
		}
	}
	while (c!='q');
	
		
	// say goodbye
	cout << "Exit!\n";
	cout.flush();
	return 0;	
};

//*/

