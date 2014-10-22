////////////////////////////////////////////////////////////////////////////////
//	File: RCInstruction.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Instruction codes for the messages.
//
////////////////////////////////////////////////////////////////////////////////

package rc.client;


////////////////////////////////////////
//	Message instructions
public enum RCInstruction {
	// errors
	SUCCESS				(0),
	ERROR_UNKNOWN		(1),
	ERROR_PASSWORD		(2),
	ERROR_TIMEOUT		(3),
	ERROR_INVALID_DATA	(4),
	
	// manage servo outputs
	ADD_SERVO			(5),
	RM_SERVO			(6),

	SET_SMOOTHING		(7),
	SET_MINWIDTH		(8),
	SET_MAXWIDTH		(9),
	SET_STEERING		(10),
	SET_DEFAULT_STEERING(15),

	// communication
	SET_TIMEOUT			(11),
	KEEP_ALIVE			(12),
	AUTHENTICATE		(13),
	RESET				(16),
	QUIT				(14);
	
	public final int value;
	private RCInstruction(int value) {
		this.value = value;
	}
	public static RCInstruction fromInt(int value) {
		if (value<=14)
			return RCInstruction.values()[value];
		else
			return null;
	}
}