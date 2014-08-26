////////////////////////////////////////////////////////////////////////////////
//	File:   spinlock.h
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Mutex object using spinlocks.
//	
//	Works the same as the STL std::mutex. See cppreference.com for documentation.
//	IMPORTANT: this spinlock does NOT care about a threads's identity. It can be
//	unlocked from a thread other than the locking thread. Be careful!
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atomic>

#define SPINLOCK_NS servo_drive

namespace SPINLOCK_NS {

class mutex {
	public:
		mutex();
		
		void lock();
		bool try_lock();
		void unlock();
	private:	
		std::atomic<bool> lock_var;	
};


} // namespace

