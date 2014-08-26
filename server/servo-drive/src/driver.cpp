////////////////////////////////////////////////////////////////////////////////
//	File: driver.cpp
//	
//	Author: Péter Kardos
//	Created: 2013. june 04.
//		Last Changed: ---
////////////////////////////////////////////////////////////////////////////////
//	Content:
//		Definitions for the servo driver class.
////////////////////////////////////////////////////////////////////////////////


#include "servo_drive.h"
// standard c++
#include <cassert>
#include <new>
#include <algorithm>
#include <cmath>
// glibc
#include <sys/mman.h>
// xenomai
#include <rtdk.h>
#include <native/task.h>
#include <native/timer.h>




#define GetTime() ServoDrive::timer.absolute()
#define SetPinOut(pin) ServoDrive::gpio.fsel(pin, GPIO_FSEL_OUT)
#define EnablePin(pin) ServoDrive::gpio.set(pin)
#define DisablePin(pin) ServoDrive::gpio.clear(pin)


using namespace std;


////////////////////////////////////////////////////////////////////////////////
//	Static variables.
static RT_TASK taskDummy;	// file scope -> others really shouldn't mess with this
RT_TASK& ServoDrive::signalThread(taskDummy);			// common realtime task

unordered_map<pin_t, ServoOut*> ServoDrive::servos;	// all the servos		
atomic<bool> ServoDrive::signalThreadRun(false);		// message to stop
pair<pin_t, unsigned> ServoDrive::signals[54];	// signals for one pulse
servo_drive::mutex ServoDrive::syncMutex;		// map memory access synchronization

Timer ServoDrive::timer;
GPIO ServoDrive::gpio;

#ifdef SD_USE_PROFILER
std::vector<std::pair<unsigned, unsigned>> ServoDrive::profilerData;
#endif


////////////////////////////////////////////////////////////////////////////////
//	Constructor and destructor.
ServoDrive::ServoDrive() {
	mlockall(MCL_CURRENT | MCL_FUTURE);	
	rt_print_auto_init(true);
}
ServoDrive::~ServoDrive() {
	rt_print_auto_init(false);

	// remove remaining stuff
	Reset();
}



////////////////////////////////////////////////////////////////////////////////
//	Signal thread function.
//
//	Real-time xenomai thread.
//
//	This function generates the PWM that controls the servos. The servo states
//	and pin numbers are red from the static database. The thread turns on all
//	GPIO pins, then actively spins and checks the time till the last one is
//	switched off.
void SignalThreadFunc(void* pArg) {
	rt_printf("Thread running.\n");
	
	// get timer properties: using jiffies or ns
	RT_TIMER_INFO timerInfo;
	rt_timer_inquire(&timerInfo);
	SRTIME (*to_delay_time)(SRTIME nanoseconds);
	if (timerInfo.period==TM_ONESHOT) {
		to_delay_time = [](SRTIME time) {return time;};
	}
	else {
		to_delay_time = rt_timer_ns2ticks;
	}

	/**** PROFILING ****/
#ifdef SD_USE_PROFILER
	// this is to monitor timing results
	unsigned timeResults[54];
#endif
	
	// stores the time points when a particular servo is turned off
	unsigned long long endTimes[54];

	
	// loop until thread told to quit
	while (ServoDrive::signalThreadRun) {
		uint64_t timeStart = GetTime();
		
		//////////////////////////////////////////
		// stage 1:
		// prepare the servo data for fast repeated access.
		unsigned nServos=0;

		// lock mutex
		while (!ServoDrive::syncMutex.try_lock()) {
			rt_task_sleep(to_delay_time(100ll*1000ll)); // sleep 100 microseconds
		}

		// extract data
		for (auto it=ServoDrive::servos.begin();
			it!=ServoDrive::servos.end();
			it++)
		{
			ServoDrive::signals[nServos].first = it->first;
			ServoDrive::signals[nServos].second = it->second->GetPulseWidth();
			nServos++;
		}

		// unlock mutex
		ServoDrive::syncMutex.unlock();

		// sort the array for even more accurate timing
		qsort(ServoDrive::signals, nServos, sizeof(pair<pin_t, unsigned>),
				[](const void* p1, const void* p2) -> int {
					 auto *s1=(pair<pin_t, unsigned>*)p1, 
						  *s2=(pair<pin_t, unsigned>*)p2;
					 if (s1->second < s2->second) return -1;
					 else if (s1->second == s2->second) return 0;
					 else return 1;
				}
		);


		//////////////////////////////////////////
		// stage 2:
		// start sending the signals
		unsigned idxCur = 0;

		// enable pins, get end time
		for (unsigned i=0; i<nServos; i++) {
			// waste like 2 microseconds or a very small interval
			// this is needed to avoid 'time-collision' when many servos have the same pulse-lenght
			// also avoids fractional microseconds
			// cost of this: about 50 microsec with 17 servos -- negligible
			// note: *it works like a charm* -- don't delete this
			uint64_t t = GetTime() + 3ull;
			while (GetTime()<t) {/* empty */}

			// calculate time when pin should be disabled
			endTimes[i] = GetTime()+ServoDrive::signals[i].second;
			// turn pin high
			EnablePin(ServoDrive::signals[i].first);
		}


		// disable the pins in order
		uint64_t curTime;
		int64_t  timeRemaining;
		while (idxCur<nServos) {
			// sleep if possible
			timeRemaining = (int64_t)endTimes[idxCur]-(int64_t)GetTime();
			if (timeRemaining > 4000ll) {
				rt_printf("Sleep error!");
			}
			else if (timeRemaining > 60ll) {
				rt_task_sleep(to_delay_time((timeRemaining-60ll)*1000ll));
			}
			// burn cpu cycles till pre-calculated time point
			while (GetTime() < endTimes[idxCur]) {/* empty */}
#ifdef SD_USE_PROFILER
			// collect profiler data
			timeResults[idxCur] = (unsigned)(ServoDrive::signals[idxCur].second + GetTime()-endTimes[idxCur]);
#endif
			// disable pin
			DisablePin(ServoDrive::signals[idxCur].first);
			// next round, next servo
			idxCur++;
		}

		
		//////////////////////////////////////////
		// stage 3:
		// prepare for next round
		
		/**** PROFILING ****/
#ifdef SD_USE_PROFILER
		// collect profiler data
		for (unsigned i=0; i<nServos; i++) {
			ServoDrive::profilerData.push_back(
					pair<unsigned, unsigned>(
						timeResults[i],
						ServoDrive::signals[i].second
					)
			);
		}
#endif

		// sleep until the next update (~18500 us @ 50Hz)
		long long sleepTime = (20000ll-(int64_t)(GetTime()-timeStart)); // microseconds
		rt_task_sleep(to_delay_time((sleepTime>100?sleepTime:100)*1000));
	}

	
	rt_printf("Thread stopped.\n");
	
	// delete the current task
	rt_task_delete(nullptr);
}



////////////////////////////////////////////////////////////////////////////////
//	Manage attached servo pins.
ServoOut* ServoDrive::AddServo(pin_t pin, float steering, float minWidthMicros, float maxWidthMicros) {
	ServoOut* ret = nullptr;
	
	// lock mutex
	syncMutex.lock();

	// get initial number of servos
	size_t nServosInitial = servos.size();
	
	// add new element
	auto mapInsertResult = servos.insert(pair<pin_t, ServoOut*>(pin, nullptr));

	// on succesful insertion
	if (mapInsertResult.second) {
		ServoOut* newServo;
		try {
			newServo = new ServoOut(steering, minWidthMicros, maxWidthMicros); // allocate the servo object
		}
		catch (bad_alloc& e) {
			servos.erase(mapInsertResult.first);
			syncMutex.unlock();
			return nullptr;
		}
		mapInsertResult.first->second = newServo;
		
		// add pin to list	of pins owned by this instance
		auto pinListResult = attachedPins.insert(pin);
		if (!pinListResult.second) {
			delete mapInsertResult.first->second; // newly allocated servo
			servos.erase(mapInsertResult.first);
			syncMutex.unlock();
			return nullptr;
		}
		
		
		SetPinOut(pin);
		lastError = SE_SUCCESS;
		
		// launch a real-time task if this is the first servo
		if (nServosInitial==0) {
			signalThreadRun = true;
			int r = rt_task_spawn(&signalThread, "sd_signal_thread", 0, 99, T_FPU, SignalThreadFunc, nullptr);
			if (r!=0) {
				delete mapInsertResult.first->second; // newly allocated servo
				servos.erase(mapInsertResult.first);
				attachedPins.erase(pin);
				syncMutex.unlock();
				throw ServoDriveException(ServoDriveException::ERROR_TASK);
				return nullptr;
			}
		}

		// set return value
		ret = mapInsertResult.first->second;
	}
	// on insertion failure
	else {
		lastError = SE_IN_USE;	// set last error to "pin already in use"
	}
	
	// unlock mutex
	syncMutex.unlock();
	return ret;	
}
int ServoDrive::RemoveServo(pin_t pin) {
	// lock mutex
	syncMutex.lock();
	
	// check if pin is already attached to *this
	auto result = attachedPins.erase(pin);
	
	// when it is attached
	if (result>0) {
		// remove from common servo list and free servo object
		auto it = servos.find(pin);
		delete it->second;
		servos.erase(it);
		
		// delete real-time thread if no more active servos
		if (servos.size()==0) {
			// the thread should be terminated normally
			// termination is determined by polling (not very elegant)
			// side effects: might hog the cpu for at max 20 ms
			signalThreadRun = false;
			RT_TASK_INFO taskInfoDummy;
			while (0==rt_task_inquire(&signalThread, &taskInfoDummy)) {
				DampenCPUHog();
			}
		}

		// set last error
		lastError = SE_SUCCESS;
	}
	else {
		lastError = SE_NOT_FOUND;
	}
	
	// unlock mutex
	syncMutex.unlock();
	
	return lastError;
}
void ServoDrive::Reset() {
	// lock mutex
	syncMutex.lock();

	// remove all pins
	for (auto it=attachedPins.begin(); it!=attachedPins.end(); it++) {
		auto servo = servos.find(*it);
		delete servo->second;
		servos.erase(servo);
		// make sure pin is left disabled
		gpio.clear(*it);
	}
	attachedPins.clear();
	
	// delete real-time thread if no more active servos
	size_t nServos = servos.size();
	if (nServos==0) {
		// the thread should be terminated normally
		// termination is determined by polling (not very elegant)
		// side effects: might hog the cpu for at max 20 ms
		signalThreadRun = false;
		RT_TASK_INFO taskInfoDummy;
		while (0==rt_task_inquire(&signalThread, &taskInfoDummy)) {
			DampenCPUHog();
		}
	}	
	// unlock mutex
	syncMutex.unlock();
}

////////////////////////////////////////
//	Get servo by pin number
ServoOut* ServoDrive::GetServo(pin_t pin) {
	// see if the servo belongs to *this
	if (attachedPins.count(pin)>0) {
		syncMutex.lock();
		ServoOut* servo;
		auto it = servos.find(pin);
		if (it==servos.end())
			throw ServoDriveException(ServoDriveException::ERROR_UNKNOWN, __FILE__, __LINE__);
		syncMutex.unlock();
	}
	else {
		return nullptr;
	}
}





////////////////////////////////////////////////////////////////////////////////
//	Gather performance information.
#ifdef SD_USE_PROFILER
int ServoDrive::AnalyzeProfiler(unsigned& max, float& avg, map<int,unsigned>& errorDistribution) {
	if (servos.size()!=0)
		return -1;
	
	size_t nRecords = profilerData.size();
	unsigned* diffs = new unsigned[nRecords];
	
	max=0u;
	double sum=0.0;
	for (size_t i=0; i<nRecords; i++) {
		int diff = (long long)profilerData[i].first - (long long)profilerData[i].second;
		auto r = errorDistribution.insert(pair<int, unsigned>(diff, 0u));
		r.first->second++;
		diffs[i] = (unsigned)abs(diff);
		max = diffs[i]>max ? diffs[i]:max;
		sum+=(double)diffs[i];
	}
	sum/=(double)nRecords;
	avg = (float)sum;
	
	delete[] diffs;	
	return (int)nRecords;
}
#endif




////////////////////////////////////////////////////////////////////////////////
//	Obsolete code pieces here!

////////////////////////////////////////
//	from: void SignalThreadFunc(void* pArg)
//	date: 2013. july 07.
//	why: superseded by a more accurate code

		/**** THIS IS WORKING WELL ****/
		/*
		// start sending the signals
		unsigned idxCur=0;							// which servo are we using now
		uint64_t timeStart = GetTime();	// get the starting time
		uint64_t timeCur=timeStart;
		uint64_t elapsed=0;
		
		// enable all pin
		for (unsigned i=0; i<nServos; i++) {
			EnablePin(ServoDrive::signals[i].first);
		}

		
		// disable the pins in order
		while (idxCur<nServos) {
			// burn cpu cycles for enough time
			while (elapsed<ServoDrive::signals[idxCur].second) {
				timeCur = GetTime();
				elapsed = timeCur-timeStart; // CALCULATE ELAPSED TIME
			}
			// write debug
			timeResults[idxCur] = (unsigned)elapsed;
			// disable the pin
			DisablePin(ServoDrive::signals[idxCur].first);
			// increase index
			idxCur++;
		}
		//*/






