////////////////////////////////////////////////////////////////////////////////
//	File: servo.cpp
//	
//	Author: Péter Kardos
//	Created: 2013. june 4.
//		Last Changed: ---
////////////////////////////////////////////////////////////////////////////////
//	Content:
//		Definitions for the CServoOut class.
////////////////////////////////////////////////////////////////////////////////


#include "servo_drive.h"


////////////////////////////////////////////////////////////////////////////////
//	Constructor
//	Initializes values to default normal values.
ServoOut::ServoOut(float steering, float minWidthMicros, float maxWidthMicros) :
	minPulseWidth(minWidthMicros),
	maxPulseWidth(maxWidthMicros),
	steering(steering)
{
	return;
}


////////////////////////////////////////////////////////////////////////////////
//	Calculatin of actual pulse width at current state.
unsigned ServoOut::GetPulseWidth() const {
	return (unsigned)((maxPulseWidth-minPulseWidth)*steering + minPulseWidth + 0.5f);
}


////////////////////////////////////////////////////////////////////////////////
//	Manage servo state. User errors' handling included.
void ServoOut::SetSteering(float steering) {
	if (steering<0.f)
		steering=0.f;
	else if (steering>1.f)
		steering=1.f;
	this->steering = steering;
}
void ServoOut::SetMinWidth(float minWidthMicros) {
	// if smaller than zero clamp to zero
	if (minWidthMicros<0.f)
		minWidthMicros = 0.f;
	// if bigger than max extend max
	if (minWidthMicros>=maxPulseWidth) {
		maxPulseWidth = minPulseWidth+1.f;
	}
	minPulseWidth = minWidthMicros;
}
void ServoOut::SetMaxWidth(float maxWidthMicros) {
	// if smaller than zero clamp to 1.0
	if (maxWidthMicros<0.f)
		maxWidthMicros = 1.f;
	// if smaller than min reduce min
	if (maxWidthMicros<=minPulseWidth) {
		minPulseWidth = maxPulseWidth-1.f;
	}
	maxPulseWidth = maxWidthMicros;
}