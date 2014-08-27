////////////////////////////////////////////////////////////////////////////////
//	File: driver.cpp
//	
//	Author: Péter Kardos
//	Created: 2013. july 01.
//		Last Changed: ---
////////////////////////////////////////////////////////////////////////////////
//	Content:
//		Accurate timer for the servo driver. Uses the System Time register.
//		Only works on the ARM based BCM2835 SoC!
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <cstdint>
#include <exception>


////////////////////////////////////////////////////////////////////////////////
//	Exception thrown on construction failure
class TimerException : std::exception {
	friend class Timer;
private:
	enum ERROR {
		ERROR_UNKNOWN,
		ERROR_FILE,
		ERROR_MMAP
	} err;
public:
	TimerException(ERROR err=ERROR_UNKNOWN) noexcept {
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
//	The Timer object
class Timer {
public:
	// constructor
	Timer();
	~Timer();
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	// get time
	/* implement these maybe
	void start();
	void pause();
	void reset();
	uint64_t elapsed(bool reset=true);
	*/
	inline uint64_t absolute() {
		uint64_t curTime;
		curTime = *higher;
		curTime << 32;
		curTime |= *lower;
		return curTime;
	}
	inline uint32_t low() {return *lower;}
	inline uint32_t high() {return *higher;}


private:
	// store state
	uint64_t startTime;
	bool isRunning;

	// register
	void* ptr;
	int mem_fd;
	volatile uint32_t* lower;
	volatile uint32_t* higher;
};

