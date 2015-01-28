////////////////////////////////////////////////////////////////////////////////
//	File: ServoOut.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Digital representation of the physical RC servo object.
//
////////////////////////////////////////////////////////////////////////////////

package rc.client;


/**
 * Represents a real-world RC servo.
 * You may set steering, no-signal default state and exact timings.
 * Methods won't be documented, since they are evident for those
 * familiar with radio controlled machines. Read more about servo control at 
 * http://en.wikipedia.org/wiki/Servo_control or somewhere else.
 */
public class ServoOut {
	// fields
	private float steering;
	private float paused;
	private float maxPulseWidth;
	private float minPulseWidth;
	private float smoothing;
	
	
	// construction
	public ServoOut(float steering, float minWidth, float maxWidth, float pausedSteering) {
		setSteering(steering);
		setMinWidth(minWidth);
		setMaxWidth(maxWidth);
		setPaused(pausedSteering);
		smoothing = -1.f;
	}
	public ServoOut(float steering, float minWidth, float maxWidth) {
		this(steering, minWidth, maxWidth, 0.5f);
	}
	public ServoOut(float steering) {
		this(steering, 1000.f, 2000.f, 0.5f);
	}
	public ServoOut() {
		this(0.5f, 1000.f, 2000.f, 0.5f);
	}
	public ServoOut(ServoOut other) {
		this(other.steering, other.minPulseWidth, other.maxPulseWidth, other.paused);
	}
	
	// methods
	public float getPulseWidth() {
		return (maxPulseWidth-minPulseWidth)*steering + minPulseWidth; // in micros
	}
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
	
	/**
	 * Currently does not work.
	 */
	public final void setSmoothing(float smoothing) {
		this.smoothing = smoothing;
	}
	
	// getters
	public float getSteering() {
		return steering;
	}
	public float getMinWidth() {
		return minPulseWidth;
	}
	public float getMaxWidth() {
		return maxPulseWidth;
	}
	public float getPaused() {
		return paused;
	}
	public float getSmoothing() {
		return smoothing;
	}
}