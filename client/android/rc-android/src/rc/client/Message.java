////////////////////////////////////////////////////////////////////////////////
//	File: Message.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Simple message system for network communication with the server.
//
////////////////////////////////////////////////////////////////////////////////


package rc.client;



////////////////////////////////////////////////////////////////////////////////
//	Message system

////////////////////////////////////////
//	Message coninience struct
public class Message {
	public RCInstruction instruction;
	public int param1;
	public int param2;
	public Message(RCInstruction i, int param1, int param2) {
		instruction = i;
		this.param1 = param1;
		this.param2 = param2;
	}
	public Message(RCInstruction i, int param1, float param2) {
		this(i,param1,Float.floatToIntBits(param2));
	}
	public Message(RCInstruction i, int param1) {
		this(i,param1,0);
	}
	public Message(RCInstruction i) {
		this(i,0,0);
	}
	public Message() {
		this(RCInstruction.ERROR_UNKNOWN, 0,0);
	}
}
////////////////////////////////////////
//	Serialization of Messages for networking
class SerializedMessage {
	public final byte[] binary;
	public int size() {
		if (binary==null)
			return 0;
		else
			return binary.length;
	};
	
	public SerializedMessage() {
		binary = new byte[12];
		for (int i=0; i<binary.length; ++i) {
			binary[i] = 0;
		}
	}
	public SerializedMessage(Message m) {
		binary = new byte[12];
		fromMessage(m);
	}
	public final SerializedMessage fromMessage(Message m) {
		int val;
		val = m.instruction.value;
		binary[0] = (byte)(val>>24);
		binary[1] = (byte)(val>>16);
		binary[2] = (byte)(val>>8);
		binary[3] = (byte)(val);

		val = m.param1;
		binary[0+4] = (byte)(val>>24);
		binary[1+4] = (byte)(val>>16);
		binary[2+4] = (byte)(val>>8);
		binary[3+4] = (byte)(val);

		val = m.param2;
		binary[0+8] = (byte)(val>>24);
		binary[1+8] = (byte)(val>>16);
		binary[2+8] = (byte)(val>>8);
		binary[3+8] = (byte)(val);
		
		return this;
	}
	public Message toMessage() {
		int i=0, p1=0, p2=0;
		
		i += binary[0]<<24;
		i += binary[1]<<16;
		i += binary[2]<<8;
		i += binary[3];

		p1 += binary[0+4]<<24;
		p1 += binary[1+4]<<16;
		p1 += binary[2+4]<<8;
		p1 += binary[3+4];

		p2 += binary[0+8]<<24;
		p2 += binary[1+8]<<16;
		p2 += binary[2+8]<<8;
		p2 += binary[3+8];
		
		return new Message(RCInstruction.fromInt(i), p1, p2);
	}
}