////////////////////////////////////////////////////////////////////////////////
//	File: main.cpp
//	Author: Péter Kardos
//
////////////////////////////////////////////////////////////////////////////////
//	C++ library for interacting with BCM2835 SoC GPIOs.
//	Main source file.
////////////////////////////////////////////////////////////////////////////////

#include "gpio.h"
// standard c++
#include <cstdint>
#include <cassert>
// glibc
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
//	statics
volatile void* GPIO::mem_gpio = MAP_FAILED;
size_t GPIO::ref_count = 0u;
int GPIO::mem_fd = -1;


////////////////////////////////////////////////////////////////////////////////
//	constructor:
//		increase reference count, initialize as needed
//	WARNING: never call these explicity!
GPIO::GPIO() {
	if (ref_count==0u) {
		// memory map gpio registers
		mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
		if (mem_fd<0) {
			throw GPIOException(GPIOException::ERROR_FILE);
		}
		mem_gpio = mmap(nullptr, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO_REG_BASE);
		if (mem_gpio==MAP_FAILED) {
			close(mem_fd);
			mem_fd = -1;
			throw GPIOException(GPIOException::ERROR_MMAP);
		}
	}
	ref_count++;
}
//	destructor:
//		deinitialize as needed, decrease reference count
GPIO::~GPIO() {
	ref_count--;
	if (ref_count==0u) {
		if (mem_gpio!=MAP_FAILED) {
			munmap((void*)mem_gpio, 4096);
			mem_gpio = MAP_FAILED;
		}
		if (mem_fd>=0) {
			close(mem_fd);
			mem_fd = -1;
		}
	}
}






////////////////////////////////////////////////////////////////////////////////
//	manage pins

//	function selection
void GPIO::fsel(unsigned pin, GPIO_FSEL func) {
	// calculate register address
	volatile uint32_t* reg 	= reinterpret_cast<volatile uint32_t*>(4u*(pin/10u) + GPIO_OFFSET_FSEL);
	assert(size_t(reg) >= GPIO_OFFSET_FSEL && size_t(reg) <= (GPIO_OFFSET_FSEL+0x14));
	reg = (volatile uint32_t*)((size_t)reg + (size_t)mem_gpio); // add ptr to address
	
	uint32_t shift = pin%10u*3u;
	uint32_t mask = (7u << shift);
	*reg &= ~mask;
	*reg &= ~mask;
	*reg |= (func << shift);
}


// set pin to high or low
void GPIO::set(unsigned pin) {
	assert(pin<54);
	
	// address of the register
	// if pin>=32 + 0x04 is added
	// calculation as follows:
	//		shifted right by 5 -- same as division by 32
	//		miltuplied by 4, so 4 or 0 in the end
	volatile uint32_t* addr 
		= reinterpret_cast<volatile uint32_t*>((pin>>5u)*4u + GPIO_OFFSET_OUT_SET + (size_t)mem_gpio);	
	// just assert if above code produces invalid register
	assert(size_t(addr)==GPIO_OFFSET_OUT_SET+(size_t)mem_gpio || 
			size_t(addr)==GPIO_OFFSET_OUT_SET+0x04+(size_t)mem_gpio);
	
	// which bit to set
	pin%=32; // hopefully the compiler optimizes this to bitwise ops
	
	// set the bit
	*addr = (1u<<pin);
	*addr = (1u<<pin);
}
void GPIO::clear(unsigned pin) {
	assert(pin<54);
	// address of the register
	//		explanation in 'void set(unsigned pin)'
	volatile uint32_t* addr 
		= reinterpret_cast<volatile uint32_t*>((pin>>5u)*4u + GPIO_OFFSET_OUT_CLR + (size_t)mem_gpio);	
	// just assert if above code produces invalid register
	assert(size_t(addr)==GPIO_OFFSET_OUT_CLR+(size_t)mem_gpio
			|| size_t(addr)==GPIO_OFFSET_OUT_CLR+0x04+(size_t)mem_gpio);
	
	// which bit to set
	pin%=32; // hopefully the compiler optimizes this to bitwise ops
	
	// set the bit
	*addr = (1u<<pin);
	*addr = (1u<<pin);
}


void GPIO::write(unsigned pin, bool value) {
	if (value)
		set(pin);
	else
		clear(pin);
}


// read pin
bool GPIO::read(unsigned pin) {
	assert(pin<54);
	// address of the register
	//		explanation in 'void set(unsigned pin)'
	volatile uint32_t* addr 
		= reinterpret_cast<volatile uint32_t*>((pin>>5u)*4u + GPIO_OFFSET_IN_LVL + (size_t)mem_gpio);	
	// just assert if above code produces invalid register
	assert(size_t(addr)==GPIO_OFFSET_IN_LVL+(size_t)mem_gpio
			|| size_t(addr)==GPIO_OFFSET_IN_LVL+0x04+(size_t)mem_gpio);
	
	// which bit to read
	pin%=32;
	
	// read bit
	uint32_t reg_val = *addr;
	reg_val = *addr;
	return (reg_val & (1u<<pin))!=0;
}




