////////////////////////////////////////////////////////////////////////////////
//	File: ServoOut.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Helper class for creating ServoOut objects.
//
////////////////////////////////////////////////////////////////////////////////


package rc.client;


public class ServoBuilder {
	public ServoBuilder() {
		steering = 0.5f;
		paused = 0.5f;
		minPulseWidth = 1000.f;
		maxPulseWidth = 2000.f;
	}
	public ServoOut build() {
		return new ServoOut(steering, minPulseWidth, maxPulseWidth, paused);
	}
		
	// setter methods
	public final void setSteering(float steering) {
		if (steering<0.f)
			steering=0.f;
		else if (steering>1.f)
			steering=1.f;
		this.steering = steering;
	}
	public final void setPaused(float pausedSteering) {
		if (paused<0.f)
			paused=0.f;
		else if (paused>1.f)
			paused=1.f;
		this.paused = pausedSteering;		
	}
	public final void setMinWidth(float minWidthMicros) {
		// if smaller than zero clamp to zero
		if (minWidthMicros<0.f)
			minWidthMicros = 0.f;
		// if bigger than max extend max
		if (minWidthMicros>=maxPulseWidth) {
			maxPulseWidth = minPulseWidth+1.f;
		}
		minPulseWidth = minWidthMicros;		
	}
	public final void setMaxWidth(float maxWidthMicros) {
		// if smaller than zero clamp to 1.0
		if (maxWidthMicros<0.f)
			maxWidthMicros = 1.f;
		// if smaller than min reduce min
		if (maxWidthMicros<=minPulseWidth) {
			minPulseWidth = maxPulseWidth-1.f;
		}
		maxPulseWidth = maxWidthMicros;
	}	
	
	// fields
	private float steering;
	private float paused;
	private float maxPulseWidth;
	private float minPulseWidth;
}