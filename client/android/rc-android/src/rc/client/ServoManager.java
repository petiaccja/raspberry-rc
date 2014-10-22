////////////////////////////////////////////////////////////////////////////////
//	File: ServoManager.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Provides a layer of communication between the UserInterface and Networking.
//	
////////////////////////////////////////////////////////////////////////////////

package rc.client;

import java.util.HashMap;
import java.util.Map;
import java.util.Iterator;
import java.util.ArrayList;


////////////////////////////////////////////////////////////////////////////////
//	Stores internal state, generates messages to be sent.
public class ServoManager {
	////////////////////////////////////
	// adding/removing servos
	public Message[] addServo(int pin, ServoOut servo) {
		try {
			outputs.put(new Integer(pin), (ServoOut)servo.clone());
		}
		catch (CloneNotSupportedException e) {
			return null;
		}
		Message[] ret = new Message[4];
		ret[0] = new Message(RCInstruction.ADD_SERVO, pin);
		ret[1] = new Message(RCInstruction.SET_MINWIDTH, pin, servo.getMinWidth());
		ret[2] = new Message(RCInstruction.SET_MAXWIDTH, pin, servo.getMaxWidth());
		ret[3] = new Message(RCInstruction.SET_STEERING, pin, servo.getSteering());
		return ret;
	}
	public Message addServo(int pin) {
		outputs.put(new Integer(pin), new ServoOut());
		return new Message(RCInstruction.ADD_SERVO, pin);
	}
	public Message removeServo(int pin) {
		if (outputs.containsKey(new Integer(pin))) {
			outputs.remove(new Integer(pin));
			return new Message(RCInstruction.RM_SERVO, pin, 0);
		}
		else {
			return null;
		}
	}
	
	////////////////////////////////////
	// set servo parameters
	public Message setSteering(int pin, float steering) {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setSteering(steering);
			return new Message(RCInstruction.SET_STEERING, pin, servo.getSteering());
		}
		else {
			return null;
		}
		
	}
	public Message setMinWidth(int pin, float minWidthMicros) {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setMinWidth(minWidthMicros);
			return new Message(RCInstruction.SET_MINWIDTH, pin, servo.getMinWidth());
		}
		else {
			return null;
		}
	}
	public Message setMaxWidth(int pin, float maxWidthMicros) {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setMaxWidth(maxWidthMicros);
			return new Message(RCInstruction.SET_MAXWIDTH, pin, servo.getMaxWidth());
		}
		else {
			return null;
		}
	}
	public Message setDefaultSteering(int pin, float pausedSteering) {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setPaused(pausedSteering);
			return new Message(RCInstruction.SET_DEFAULT_STEERING, pin, servo.getPaused());
		}
		else {
			return null;
		}		
	}
	public Message setSmoothing(int pin, float smoothing) {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setSmoothing(smoothing);
			return new Message(RCInstruction.SET_SMOOTHING, pin, smoothing);
		}
		else {
			return null;
		}
	}
	
	////////////////////////////////////
	// other
	public Message[] rebuild() {
		ArrayList<Message> messageList = new ArrayList<Message>();
		for (Map.Entry<Integer, ServoOut> entry : outputs.entrySet()) {
			int pin = entry.getKey();
			ServoOut servo = entry.getValue();
			// add servo
			messageList.add(new Message(RCInstruction.ADD_SERVO, pin));
			// set values
			messageList.add(new Message(RCInstruction.SET_MINWIDTH, pin, servo.getMinWidth()));
			messageList.add(new Message(RCInstruction.SET_MAXWIDTH, pin, servo.getMaxWidth()));
			messageList.add(new Message(RCInstruction.SET_STEERING, pin, servo.getSteering()));
			messageList.add(new Message(RCInstruction.SET_DEFAULT_STEERING, pin, servo.getPaused()));
			messageList.add(new Message(RCInstruction.SET_SMOOTHING, pin, servo.getSmoothing()));
		}
		return (Message[])messageList.toArray();
	}
	public void reset() {
		outputs.clear();
	}
	
	
	////////////////////////////////////
	// query
	public ServoOut getServo(int pin) {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			try {
				return servo.clone();
			}
			catch (CloneNotSupportedException e) {
				return null;
			}
		}
		else {
			return null;
		}
	}
	public int size() {
		return outputs.size();
	}
	
	
	////////////////////////////////////
	// internal state
	private HashMap<Integer, ServoOut> outputs;	
}


