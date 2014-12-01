////////////////////////////////////////////////////////////////////////////////
//	File: ServoManager.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Provides a layer of communication between the UserInterface and Networking.
//	
////////////////////////////////////////////////////////////////////////////////

package rc.client;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.ArrayList;


////////////////////////////////////////////////////////////////////////////////
/**
 * Stores all servos and other necessary information about the remote physical machine.
 * You can add or remove servos, set their values. All data will be sent and
 * synchronized with the server as soon as you do something.
 * If a network error happens, you should call 'rebuild'.
 */
public class RcClient {
	////////////////////////////////////
	// vars
	private Map<Integer, ServoOut> outputs;
	private ConnectionManager connection;
	
	////////////////////////////////////
	// constructor
	public RcClient() {
		outputs = new HashMap<>();
		connection = new ConnectionManager();
	}
	
	////////////////////////////////////
	// adding/removing servos
	public void addServo(int pin, ServoOut servo) throws IOException, IllegalStateException {
		outputs.put(pin, new ServoOut(servo));

		Message[] msg = new Message[4];
		msg[0] = new Message(RCInstruction.ADD_SERVO, pin);
		msg[1] = new Message(RCInstruction.SET_MINWIDTH, pin, servo.getMinWidth());
		msg[2] = new Message(RCInstruction.SET_MAXWIDTH, pin, servo.getMaxWidth());
		msg[3] = new Message(RCInstruction.SET_STEERING, pin, servo.getSteering());
		
		for (int i = 0; i<msg.length; i++) {
			connection.send(msg[i]);
		}
	}
	public void addServo(int pin) throws IOException, IllegalStateException {
		outputs.put(pin, new ServoOut());
		connection.send(new Message(RCInstruction.ADD_SERVO, pin));
	}
	public void removeServo(int pin) throws IOException, IllegalStateException {
		if (outputs.containsKey(pin)) {
			outputs.remove(pin);
			connection.send(new Message(RCInstruction.RM_SERVO, pin, 0));
		}
	}
	
	////////////////////////////////////
	// set servo parameters
	public void setSteering(int pin, float steering) throws IOException, IllegalStateException {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setSteering(steering);
			connection.send(new Message(RCInstruction.SET_STEERING, pin, servo.getSteering()));
		}		
	}
	public void setMinWidth(int pin, float minWidthMicros) throws IOException, IllegalStateException {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setMinWidth(minWidthMicros);
			connection.send(new Message(RCInstruction.SET_MINWIDTH, pin, servo.getMinWidth()));
		}
	}
	public void setMaxWidth(int pin, float maxWidthMicros) throws IOException, IllegalStateException {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setMaxWidth(maxWidthMicros);
			connection.send(new Message(RCInstruction.SET_MAXWIDTH, pin, servo.getMaxWidth()));
		}
	}
	public void setDefaultSteering(int pin, float pausedSteering) throws IOException, IllegalStateException {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setPaused(pausedSteering);
			connection.send(new Message(RCInstruction.SET_DEFAULT_STEERING, pin, servo.getPaused()));
		}
	}
	public void setSmoothing(int pin, float smoothing) throws IOException, IllegalStateException {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			servo.setSmoothing(smoothing);
			connection.send(new Message(RCInstruction.SET_SMOOTHING, pin, smoothing));
		}
	}
	
	////////////////////////////////////
	// other
	
	/**
	 * Repairs network errors and synchronizes with server.
	 * Inconsistencies may occur after a network error, when an IOException is thrown.
	 * Rebuild attempts to solve them by reseting the server, and resending all information.
	 * You should always call rebuild after an error. It is not called autamatically,
	 * since you might decide to drop the connection alltogether.
	 * @throws IOException Rebuilding was not successful due to network error, recall it.
	 */
	public void rebuild() throws IOException, IllegalStateException {
		List<Message> messageList = new ArrayList<>();
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
		Message[] messages = (Message[])messageList.toArray();
		
		connection.send(new Message(RCInstruction.RESET, 0, 0));
		for (Message m : messages) {
			connection.send(m);
		}
	}
	
	/**
	 * Remove all servos, but keep connection.
	 */
	public void reset() throws IOException, IllegalStateException {
		connection.send(new Message(RCInstruction.RESET, 0, 0));
		outputs.clear();
	}
	
	////////////////////////////////////
	// network
	
	/**
	 * Closes connection with the server.
	 */
	public void close() {
		connection.close();
	}
	
	public void connect(String address, int port, byte[] password) 
			throws InvalidPasswordException, HostRejectedException, IllegalStateException, IOException
	{
		connection.connect(address, port, password, 5000);
	}
	
	public boolean isConnected() {
		return connection.isConnected();
	}
	
	
	////////////////////////////////////
	// query
	
	/**
	 * Get a reference to a servo. You cannot change values here.
	 */
	public ServoOut getServo(int pin) {
		ServoOut servo = outputs.get(new Integer(pin));
		if (servo!=null) {
			return new ServoOut(servo);
		}
		else {
			return null;
		}
	}
	
	/**
	 * Get number of servos.
	 * @return Number of servos.
	 */
	public int getNumServos() {
		return outputs.size();
	}
}


