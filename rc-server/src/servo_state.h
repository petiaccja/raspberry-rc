////////////////////////////////////////////////////////////////////////////////
//	File: server.cpp
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Transforming the messages into server state.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "message.h"
#include <unordered_map>
#include "servo_drive.h"



////////////////////////////////////////////////////////////////////////////////
//	A wrapper object for servos. 
//
//	Incorporates smoothing of signals, so when you
//	push the pedal down to the floor, the motor will spin up slowly. Smoothing
//	is adjustable: the value in seconds specifies the time to fully reach the
//	required speed.
class ServoGovernor {
public:
	// constructor
	ServoGovernor(ServoOut& servo, float steering=0.5f, float smoothing=0.f, float reset=0.5f);

	// update and reset
	bool Update(float elapsed);
	void Reset();

	// set/get params
	void SetSteering(float steering);
	void SetSmoothing(float smoothing);

	inline float GetSteering() {return steering;}
	inline float GetSmoothing() {return smoothing;}

	// underlying servo is public: wrapper mainly for smoothing and convenience
	ServoOut& servo;
private:
	float reset; // default steering: automatically set when connection is lost

	float steering;		// target steering
	float current;		// current steering
	float last;			// last steering
	float smoothing;	// smooth param: time required to reach applied steering
	bool enableSmoothing;	// internal flag: set to false if sm<0.f smoothing is applied
};


////////////////////////////////////////////////////////////////////////////////
//	Message processor and state management.
//	
//	Messages recieved over internet are processed by this. It will add/remove,
//	servos, drive them and set values accordingly.
class ServoState {
public:
	int MessageProc(RC_INSTRUCTION msg, uint32_t pin=0xFFFFFFFF, float value=0);
	void Suspend();
private:
	// servo driver
	ServoDrive driver;
	// output structures
	std::unordered_map<uint32_t, ServoGovernor> outputs;
};