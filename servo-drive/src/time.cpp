////////////////////////////////////////////////////////////////////////////////
//	File: driver.cpp
//	
//	Author: Péter Kardos
//	Created: 2013. july 01.
//		Last Changed: ---
////////////////////////////////////////////////////////////////////////////////
//	Content:
//		See header.
////////////////////////////////////////////////////////////////////////////////


#include "time.h"
// glibc
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


////////////////////////////////////////
// constructor
Timer::Timer() : lower(nullptr), higher(nullptr), ptr(MAP_FAILED) {	
	// open memory
	mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (mem_fd<0) {
		throw TimerException(TimerException::ERROR_FILE);
	}
	// map memory
	ptr = mmap(nullptr,
				4*1024/*BCM2835_BLOCK_SIZE*/,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				mem_fd,
				0x20000000+0x3000/*BCM2835_ST_BASE*/);
	if (ptr==MAP_FAILED) {
		close(mem_fd);
		mem_fd = -1;
		throw TimerException(TimerException::ERROR_MMAP);
	}
	// get the registers
	lower =  (uint32_t*)(size_t(ptr)+0x04);
	higher = (uint32_t*)(size_t(ptr)+0x08);	
}
Timer::~Timer() {
	// release mmap is succeeded
	if (ptr!=MAP_FAILED)
		munmap(ptr, 4*1024/*BCM2835_BLOCK_SIZE*/);
	if (mem_fd>=0)
		close(mem_fd);
}

