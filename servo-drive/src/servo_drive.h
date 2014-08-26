////////////////////////////////////////////////////////////////////////////////
//	File: servo_drive.h
//
//	Author: Péter Kardos
//	Created: 2013. june 04.
//		Last Changed: 2013. july 10.
////////////////////////////////////////////////////////////////////////////////
//	Content:
//		Main class used for servo control and other assiciated classes.
//	Description:
//		The servo driver class facilitates the generation of traditional servo
//		signals. Servos can be associated to GPIO pins, and signals are generated
//		according to the settings of the individual servos.
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "spinlock.h"
#include "time.h"
#include "gpio.h"
// standard c++
#include <unordered_map>
#include <set>
#include <atomic>
#include <exception>
#include <string>
#include <sstream>



#ifdef SD_USE_PROFILER
#include <vector> // only for profiler
#include <map> // only for profiler
#endif

// pins are actually numbers, but since its not safe, might be replaced in the future
typedef unsigned pin_t;

// all this mess is to avoid including <native/task.h> in this header
// this way end-users don't get headaches because of the missing xenomai library
// might cause problems if underlying xenomai code is changed
struct rt_task_placeholder;
typedef struct rt_task_placeholder RT_TASK_PLACEHOLDER;
typedef RT_TASK_PLACEHOLDER RT_TASK;

// error codes used by the servo driver class and its methods
// end-user never needs them, error returns nullptr
enum SERVODRIVE_ERR : unsigned {
	SE_SUCCESS = 0,
	SE_IN_USE,
	SE_NOT_FOUND
};

////////////////////////////////////////////////////////////////////////////////
//	Class: CServoOut
//	Purpose: Manages information about the state and operation of a single servo.
class ServoOut {
public:
	// constructor
	ServoOut(float steering=0.5f, float minWidthMicros=1000.f, float maxWidthMicros=2000.f);		

	// functions
	unsigned GetPulseWidth() const; // calculates the pulse width for current state

	void SetSteering(float steering); // in range 0.0 ... 1.0 (normally 1.0 ms to 2.0 ms)
	void SetMinWidth(float minWidthMicros); // smallest pulse width in microseconds (usually 1000 us)
	void SetMaxWidth(float maxWidthMicros); // maximum pulse width in microseconds (usually 2000 us)

	float GetSteering() const {return steering;}
	float GetMinWidth() const {return minPulseWidth;}
	float GetMaxWidth() const {return maxPulseWidth;}
private:
	// state
	float steering; // in range 0.0 ... 1.0 (0.5=neutral, normally 1.0 ms to 2.0 ms)
	float minPulseWidth; // smallest pulse width in microseconds (usually 1000 us)
	float maxPulseWidth; // maximum pulse width in microseconds (usually 2000 us)
};

////////////////////////////////////////////////////////////////////////////////
//	Class: ServoDriveException
class ServoDriveException : public std::exception {
	friend class ServoDrive;
private:
	std::string file;
	enum ERROR {
		ERROR_UNKNOWN,
		ERROR_TASK,
	} err;
public:
	ServoDriveException(ERROR err=ERROR_UNKNOWN, const char* file=nullptr, const int line=0) noexcept {
		this->err = err;
		std::stringstream ss;
		if (file)
			ss << file;
		if (line)
			ss << ":" << line;
		this->file = ss.str();
	}
	virtual const char* what() const noexcept {
		switch (err) {
		case ERROR_TASK:
			return "Failed to create real-time xenomai task.";
		default:
			return "Unknown error.";
		}
	}
	virtual const char* get_file() const noexcept {
		return file.size()>0 ? file.c_str():"No information.";
	}
};


////////////////////////////////////////////////////////////////////////////////
//	Class: CServoDrive
//	Purpose: The actual servo driver object. Create an instance and drive your
//			 servos.
class ServoDrive {
	friend void SignalThreadFunc(void* pArg);
public:
	// constructors and destructor
	ServoDrive();
	~ServoDrive();

	// attach servos to pins
	ServoOut* AddServo(pin_t pin, 
						float steering=0.5f, 
						float minWidthMicros=1000.f, 
						float maxWidthMicros=2000.f); // add a new servo to specified pin
	ServoOut* GetServo(pin_t pin);
	int RemoveServo(pin_t pin); // remove a servo by pin
	void Reset(); // reset -- remove all servos on this driver instance
	SERVODRIVE_ERR GetLastError() const {return lastError;}
private:
	// prevent copy, copy construct
	ServoDrive& operator=(const ServoDrive& other) = delete;
	ServoDrive(const ServoDrive& other) = delete;

	// private members
	std::set<pin_t> attachedPins;	// list of pins on driver instance
	SERVODRIVE_ERR lastError;

	// busy wait
	// When releasing the rt-thread, a busy wait occurs.
	// This function is also called at each cycle, with the purpose of
	// reducing CPU usage. 
	// Tip: in this function, set a rescheduling of this thread
	//     - std::this_thread::yield();
	//     - rt_task_yield();
	//     - pthread_yield();
	//     - ... other implementation-dependent stuff.
	// Tip: you might also want to sleep the thread, if you don't care about timing.
	inline virtual void DampenCPUHog() {
	}

	// common private statics
	static std::unordered_map<pin_t, ServoOut*> servos;// all the servos
	static std::pair<pin_t, unsigned> signals[54];		// signals for one pulse batch (pin & pulse width)

	static RT_TASK& signalThread;						// common realtime task
	static std::atomic<bool> signalThreadRun;				// thread exits loop if set to false
	static servo_drive::mutex syncMutex;				// synchronize accesses to static unordered_map

	static Timer timer;									// system timer
	static GPIO gpio;									// gpio driver

	// profiler
#ifdef SD_USE_PROFILER
	private: static std::vector<std::pair<unsigned, unsigned>> profilerData;
	public:	static int AnalyzeProfiler(unsigned& max, float& avg, std::map<int,unsigned>& errorDistribution);
#endif

};