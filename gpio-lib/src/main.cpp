////////////////////////////////////////////////////////////////////////////////
//	File: main.cpp
//	Author: Péter Kardos
//
////////////////////////////////////////////////////////////////////////////////
//	C++ library for interacting with BCM2835 SoC GPIOs.
//	Test file.
//	Test results:
//		Blinking all the pins didn't freeze the system, guess it works.
//		I don't have the hardware to check actual pin levels.
//		Pins above GPIO 31 were not tested.
////////////////////////////////////////////////////////////////////////////////


/*
#include "gpio.h"
#include <unistd.h>
#include <exception>
#include <iostream>
#include <cassert>
#include <thread>

using namespace std;


#define BLINK(pin)	gpio.fsel(pin, GPIO_FSEL_OUT);\
					gpio.set(pin);\
					gpio.clear(pin)


int main(int argc, char** argv) {	
	try {
		GPIO gpio;

		// blink all the pins on the RPi
		BLINK(RPI_V2_GPIO_P1_03);
		BLINK(RPI_V2_GPIO_P1_05);
		BLINK(RPI_V2_GPIO_P1_07);
		BLINK(RPI_V2_GPIO_P1_08);
		BLINK(RPI_V2_GPIO_P1_10);
		BLINK(RPI_V2_GPIO_P1_11);
		BLINK(RPI_V2_GPIO_P1_12);
		BLINK(RPI_V2_GPIO_P1_13);
		BLINK(RPI_V2_GPIO_P1_15);
		BLINK(RPI_V2_GPIO_P1_16);
		BLINK(RPI_V2_GPIO_P1_18);
		BLINK(RPI_V2_GPIO_P1_19);
		BLINK(RPI_V2_GPIO_P1_21);
		BLINK(RPI_V2_GPIO_P1_22);
		BLINK(RPI_V2_GPIO_P1_23);
		BLINK(RPI_V2_GPIO_P1_24);
		BLINK(RPI_V2_GPIO_P1_26);

		// RPi Version 2, new plug P5
		BLINK(RPI_V2_GPIO_P5_03);
		BLINK(RPI_V2_GPIO_P5_04);
		BLINK(RPI_V2_GPIO_P5_05);
		BLINK(RPI_V2_GPIO_P5_06);
		
	}
	catch (exception& e) {
		cout << e.what();
	}
	catch (...) {
		cout << "Error!";
	}
	

	return 0;
}

//*/