////////////////////////////////////////////////////////////////////////////////
//	File: Messenger.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Allows non-blocking throttle and steering value setting, while sending
//	the message to the RCClient as soon as possible. Also keeps sending 
//	keep-alive messages whilst there's no change to steering and/or throttle.	
////////////////////////////////////////////////////////////////////////////////

package rccontroller.android;

import java.io.IOException;
import rc.client.RCClient;
import rc.client.Message;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import rc.client.RCInstruction;


////////////////////////////////////////////////////////////////////////////////
//	Shaering of Messenger among activities
class SharedMessenger {
	static Messenger messenger;
}


////////////////////////////////////////////////////////////////////////////////
//	Messenger: translates servo position to network message via RCClient.
public class Messenger {
	// variables
	private final RCClient rcClient;
	LinkedBlockingQueue<Command> commandBuffer;
	private final int pinSteering, pinThrottle;
	Thread thread;
	MessageThread threadObject;
	
	// constructor
	public Messenger(RCClient rcClient, int pinSteering, int pinThrottle) {
		this.rcClient = rcClient;
		this.pinSteering = pinSteering;
		this.pinThrottle = pinThrottle;
		commandBuffer = new LinkedBlockingQueue<Command>();
	}
	
	// start/close
	public void start() {
		if (thread!=null)
			throw new IllegalStateException("already running");
		threadObject = new MessageThread();
		thread = new Thread(threadObject);
		thread.start();
	}
	public void close() {
		if (thread==null)
			return;
		threadObject.kill();
		thread.interrupt();
		try {
			thread.join();
		}
		catch (InterruptedException e) {
			/* empty */
		}
		finally {
			thread = null;
			threadObject = null;
		}
	}
	
	// set throttle/steering, range is [0,1]
	public void setThrottle(float t) {
		commandBuffer.add(new Command(Command.THROTTLE, t));
	}
	public void setSteering(float s) {
		commandBuffer.add(new Command(Command.STEERING, s));
	}
	// set timeout
	public void setTimeout(int timeout) {
		threadObject.setTimeout(timeout);
	}
	
	// thread function
	private class MessageThread implements Runnable {
		private AtomicBoolean isAlive;
		private AtomicInteger timeout;
		public MessageThread() {
			isAlive = new AtomicBoolean(true);
			timeout = new AtomicInteger(200);
		}
		@Override
		public void run() {
			
			while (isAlive.get()==true) {
				try {
					// get the changes
					Command command;
					command = commandBuffer.poll(timeout.get(), TimeUnit.MILLISECONDS);
					// send a keep-alive on timeout
					if (command==null) {
						rcClient.send(new Message(RCInstruction.KEEP_ALIVE));
						continue;
					}
					// send actual message
					switch (command.command) {
						case Command.STEERING:
							rcClient.send(new Message(RCInstruction.SET_STEERING, pinSteering, command.value));
							break;
						case Command.THROTTLE:
							rcClient.send(new Message(RCInstruction.SET_STEERING, pinThrottle, command.value));
							break;
					}
				}
				catch (InterruptedException e) {
					break;
				}
				catch (IOException e) {
					// empty //
				}
				catch (IllegalStateException e) {
					// when the rcClient is not even connected //
				}
			}
		}
		public void kill() {
			isAlive.set(false);
		}
		public void setTimeout(int timeout) {
			this.timeout.set(timeout);
		}
	}
	
	// queue command items
	private class Command {
		Command(int command, float value) {
			this.command = command;
			this.value = value;
		}
		public static final int STEERING = 1;
		public static final int THROTTLE = 2;
		public int command;
		public float value;
	}
}


