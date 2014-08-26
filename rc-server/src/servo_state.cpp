////////////////////////////////////////////////////////////////////////////////
//	File: server.cpp
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////////


#include "servo_state.h"
#include <exception>
#include <iostream>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
//	The servo driver/message processor

// process messages and modify state accordingly
int ServoState::MessageProc(RC_INSTRUCTION msg, uint32_t pin, float value) {
	switch (msg) {
	case RCI_ADD_SERVO: {
		try {
			ServoOut* result = driver.AddServo(pin);
			if (result == nullptr) {
				return -1;
			}
			auto ins = outputs.insert(pair<uint32_t, ServoGovernor>(pin, ServoGovernor(*result)));
			if (!ins.second) {
				driver.RemoveServo(pin);
				return -1;
			}
		}
		catch (exception& e) {
			cout << e.what();
			return -1;
		}
		break;
	}
	case RCI_RM_SERVO:
		driver.RemoveServo(pin);
		outputs.erase(pin);
		break;
	case RCI_SET_MINWIDTH: {
		auto it = outputs.find(pin);
		if (it==outputs.end())
			return -1;
		it->second.servo.SetMinWidth(value);
		break;
	}
	case RCI_SET_MAXWIDTH: {
		auto it = outputs.find(pin);
		if (it==outputs.end())
			return -1;
		it->second.servo.SetMaxWidth(value);
		break;
	}
	case RCI_SET_STEERING: {
		auto it = outputs.find(pin);
		if (it==outputs.end())
			return -1;
		it->second.SetSteering(value);
		break;
	}
	case RCI_SET_SMOOTHING: {
		auto it = outputs.find(pin);
		if (it==outputs.end())
			return -1;
		it->second.SetSmoothing(value);
		break;
	}
	default:
		return -2;
	}
	return 0;
}

// set all servos to default state, called when error occurs and vehicle must be stopped
void ServoState::Suspend() {
	for (auto& it : outputs) {
		it.second.Reset();
	}
}




////////////////////////////////////////////////////////////////////////////////
//	Servo governor object

// construct and set default parameters
ServoGovernor::ServoGovernor(ServoOut& servo, float steering, float smoothing, float reset) : 
	servo(servo),
	last(0.5f),
	current(0.5f),
	reset(reset)
{
	SetSteering(steering);
	SetSmoothing(smoothing);
}

// updates the servo steering with smoothing
bool ServoGovernor::Update(float elapsed) {
	bool ret=true;
	if (enableSmoothing) {
		current += (steering-last)*elapsed/smoothing;
		float min = last<steering ? last:steering;
		float max = last>steering ? last : steering;
		if (current<min) {
			current=min;
			ret = false;
		}
		else if (current>max) {
			current=max;
			ret = false;
		}
	}
	else {
		current = steering;
		ret = false;
	}
	servo.SetSteering(current);
	return ret;
}
// set servo to neutral/default state, normally used when connection is lost
void ServoGovernor::Reset() {
	servo.SetSteering(reset);
	steering = last = current = reset;
}



// set steering target
void ServoGovernor::SetSteering(float steering) {
	this->last = this->steering;
	this->steering = steering;
	if (!enableSmoothing)
		servo.SetSteering(steering);
}
// set smoothing: the time required to fully converge steering
void ServoGovernor::SetSmoothing(float smoothing) {
	if (smoothing<=0.f)
		enableSmoothing=false;
	if (smoothing<0.01f)
		smoothing = 0.01f;
	this->smoothing = smoothing;
}

