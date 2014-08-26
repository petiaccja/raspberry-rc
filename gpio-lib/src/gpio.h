////////////////////////////////////////////////////////////////////////////////
//	File: main.cpp
//	Author: Péter Kardos
//
////////////////////////////////////////////////////////////////////////////////
//	C++ library for interacting with BCM2835 SoC GPIOs.
//	Main header file.
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <exception>
#include <cassert>
#include <cstddef> // size_t

////////////////////////////////////////////////////////////////////////////////
//	Constants
#define GPIO_REG_BASE		0x20200000
#define GPIO_OFFSET_FSEL		(0x00)
#define GPIO_OFFSET_OUT_SET		(0x1C)
#define GPIO_OFFSET_OUT_CLR		(0x28)
#define GPIO_OFFSET_IN_LVL		(0x34)




////////////////////////////////////////////////////////////////////////////////
//	Enums

//	Function select
enum GPIO_FSEL : unsigned {
	GPIO_FSEL_IN	= 0x00,
	GPIO_FSEL_OUT	= 0x01,
	GPIO_FSEL_ALT0	= 0x04,
	GPIO_FSEL_ALT1	= 0x05,
	GPIO_FSEL_ALT2	= 0x06,
	GPIO_FSEL_ALT3	= 0x07,
	GPIO_FSEL_ALT4	= 0x03,
	GPIO_FSEL_ALT5	= 0x02			
};

//	Raspberry Pi P1 and P5 header pins and their GPIO numbers
//	This enum is from the BCM2835 Library by Mike McCauley -> investigate copyright issues
//		BCM2835 Library version 1.25
//		Date: 2013. July 8.
enum RPI_GPIO_PIN : unsigned {
    RPI_GPIO_P1_03        =  0,  ///< Version 1, Pin P1-03
    RPI_GPIO_P1_05        =  1,  ///< Version 1, Pin P1-05
    RPI_GPIO_P1_07        =  4,  ///< Version 1, Pin P1-07
    RPI_GPIO_P1_08        = 14,  ///< Version 1, Pin P1-08, defaults to alt function 0 UART0_TXD
    RPI_GPIO_P1_10        = 15,  ///< Version 1, Pin P1-10, defaults to alt function 0 UART0_RXD
    RPI_GPIO_P1_11        = 17,  ///< Version 1, Pin P1-11
    RPI_GPIO_P1_12        = 18,  ///< Version 1, Pin P1-12
    RPI_GPIO_P1_13        = 21,  ///< Version 1, Pin P1-13
    RPI_GPIO_P1_15        = 22,  ///< Version 1, Pin P1-15
    RPI_GPIO_P1_16        = 23,  ///< Version 1, Pin P1-16
    RPI_GPIO_P1_18        = 24,  ///< Version 1, Pin P1-18
    RPI_GPIO_P1_19        = 10,  ///< Version 1, Pin P1-19, MOSI when SPI0 in use
    RPI_GPIO_P1_21        =  9,  ///< Version 1, Pin P1-21, MISO when SPI0 in use
    RPI_GPIO_P1_22        = 25,  ///< Version 1, Pin P1-22
    RPI_GPIO_P1_23        = 11,  ///< Version 1, Pin P1-23, CLK when SPI0 in use
    RPI_GPIO_P1_24        =  8,  ///< Version 1, Pin P1-24, CE0 when SPI0 in use
    RPI_GPIO_P1_26        =  7,  ///< Version 1, Pin P1-26, CE1 when SPI0 in use

    // RPi Version 2
    RPI_V2_GPIO_P1_03     =  2,  ///< Version 2, Pin P1-03
    RPI_V2_GPIO_P1_05     =  3,  ///< Version 2, Pin P1-05
    RPI_V2_GPIO_P1_07     =  4,  ///< Version 2, Pin P1-07
    RPI_V2_GPIO_P1_08     = 14,  ///< Version 2, Pin P1-08, defaults to alt function 0 UART0_TXD
    RPI_V2_GPIO_P1_10     = 15,  ///< Version 2, Pin P1-10, defaults to alt function 0 UART0_RXD
    RPI_V2_GPIO_P1_11     = 17,  ///< Version 2, Pin P1-11
    RPI_V2_GPIO_P1_12     = 18,  ///< Version 2, Pin P1-12
    RPI_V2_GPIO_P1_13     = 27,  ///< Version 2, Pin P1-13
    RPI_V2_GPIO_P1_15     = 22,  ///< Version 2, Pin P1-15
    RPI_V2_GPIO_P1_16     = 23,  ///< Version 2, Pin P1-16
    RPI_V2_GPIO_P1_18     = 24,  ///< Version 2, Pin P1-18
    RPI_V2_GPIO_P1_19     = 10,  ///< Version 2, Pin P1-19, MOSI when SPI0 in use
    RPI_V2_GPIO_P1_21     =  9,  ///< Version 2, Pin P1-21, MISO when SPI0 in use
    RPI_V2_GPIO_P1_22     = 25,  ///< Version 2, Pin P1-22
    RPI_V2_GPIO_P1_23     = 11,  ///< Version 2, Pin P1-23, CLK when SPI0 in use
    RPI_V2_GPIO_P1_24     =  8,  ///< Version 2, Pin P1-24, CE0 when SPI0 in use
    RPI_V2_GPIO_P1_26     =  7,  ///< Version 2, Pin P1-26, CE1 when SPI0 in use

    // RPi Version 2, new plug P5
    RPI_V2_GPIO_P5_03     = 28,  ///< Version 2, Pin P5-03
    RPI_V2_GPIO_P5_04     = 29,  ///< Version 2, Pin P5-04
    RPI_V2_GPIO_P5_05     = 30,  ///< Version 2, Pin P5-05
    RPI_V2_GPIO_P5_06     = 31,  ///< Version 2, Pin P5-06
};


////////////////////////////////////////////////////////////////////////////////
//	Exceptions
class GPIOException : public std::exception {
	friend class GPIO;
private:
	enum ERROR {
		ERROR_UNKNOWN,
		ERROR_FILE,
		ERROR_MMAP,
	} err;
public:
	GPIOException(ERROR err=ERROR_UNKNOWN) noexcept {
		this->err = err;
	}
	virtual const char* what() const noexcept {
		switch (err) {
		case ERROR_FILE:
			return "Failed to open /dev/mem, are you root?";
		case ERROR_MMAP:
			return "Failed to mmap /dev/mem.";
		default:
			return "Unknown error.";
		}
	}
};


////////////////////////////////////////////////////////////////////////////////
//	GPIO driver class
class GPIO {
public:
	// constructor
	GPIO();
	~GPIO();
	// pin manipulation
	void fsel(unsigned pin, GPIO_FSEL func);
	
	void set(unsigned pin);
	void clear(unsigned pin);	
	void write(unsigned pin, bool value);
	
	bool read(unsigned pin);
private:
	// statics
	static size_t ref_count;	// number of GPIO class instances
	static volatile void* mem_gpio;	// mapped memory
	static int mem_fd;
};

