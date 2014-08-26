////////////////////////////////////////////////////////////////////////////////
//	File: msg_handler.h
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Common definitions to deal with messages.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include <cstdint>
#include <ostream>
#include <cstring>


//	What to do when a packet is recieved
enum RC_INSTRUCTION : uint32_t {
	// errors
	RCI_SUCCESS = 0u,
	RCI_ERROR_UNKNOWN,
	RCI_ERROR_PASSWORD,
	RCI_ERROR_TIMEOUT,
	RCI_ERROR_INVALID_DATA,
	
	// manage servo outputs
	RCI_ADD_SERVO,
	RCI_RM_SERVO,

	RCI_SET_SMOOTHING,
	RCI_SET_MINWIDTH,
	RCI_SET_MAXWIDTH,
	RCI_SET_STEERING,

	// communication
	RCI_SET_TIMEOUT,
	RCI_KEEP_ALIVE,		// regular connection lost is too slow
	RCI_AUTHENTICATE,
	RCI_QUIT,
};


//	Contains a message in easily processible form
union fpuint {
	float fp;
	uint32_t u;
	operator float() const {return fp;}
	operator uint32_t() const {return u;}
	fpuint& operator=(float f) {fp=f; return *this;}
	fpuint& operator=(uint32_t u) {this->u=u; return *this;}
	fpuint() : u(0u) {}
	explicit fpuint(float f) : fp(f) {}
	explicit fpuint(double f) : fp((float)f) {}
	fpuint(uint32_t u) : u(u) {}
};



struct Message {
	RC_INSTRUCTION instruction;
	fpuint param1;
	fpuint param2;
	Message(RC_INSTRUCTION instr=RCI_ERROR_UNKNOWN, fpuint p1=0u, fpuint p2=0u) : 
		instruction(instr),
		param1(p1),
		param2(p2) {return;}
};
template <int i=0>
std::ostream& operator<<(std::ostream& os, const Message& msg) {
	os << "(" << msg.instruction << ":"
		<< std::hex << msg.param1.u << ":" << msg.param2.u << ")" << std::dec;
	return os;
}



struct SerializedMessage {
	uint8_t binary[12u];
	static size_t size() {return 12u;}

	SerializedMessage() {		
		for (auto& a : binary) {a=0u;}		
	}
	SerializedMessage(const Message& m) {
		from_message(m);
	}
	SerializedMessage& operator=(const Message& m) {
		from_message(m);
		return *this;
	}
	operator Message() const {
		Message m;
		
		uint32_t val = 0u;
		val += binary[0]<<24;
		val += binary[1]<<16;
		val += binary[2]<<8;
		val += binary[3]<<0;
		m.instruction = (RC_INSTRUCTION)val;

		val = 0u;
		val += binary[0+4]<<24;
		val += binary[1+4]<<16;
		val += binary[2+4]<<8;
		val += binary[3+4]<<0;
		m.param1 = val;

		val = 0u;
		val += binary[0+8]<<24;
		val += binary[1+8]<<16;
		val += binary[2+8]<<8;
		val += binary[3+8]<<0;
		m.param2 = val;
		
		return m;
	}
	operator uint8_t*() {
		return binary;
	}
	operator const uint8_t*() const {
		return binary;
	}
	uint8_t& operator[](size_t index) {
		return binary[index];
	}
	uint8_t operator[](size_t index) const {
		return binary[index];
	}
private:
	void from_message(const Message& m) {
		uint32_t val;
		memset(binary, 0, size());
		
		val = (uint32_t)m.instruction;
		binary[0] = (uint8_t)(val>>24);
		binary[1] = (uint8_t)(val>>16);
		binary[2] = (uint8_t)(val>>8);
		binary[3] = (uint8_t)(val>>0);

		val = (uint32_t)m.param1;
		binary[0+4] = (uint8_t)(val>>24);
		binary[1+4] = (uint8_t)(val>>16);
		binary[2+4] = (uint8_t)(val>>8);
		binary[3+4] = (uint8_t)(val>>0);

		val = (uint32_t)m.param2;
		binary[0+8] = (uint8_t)(val>>24);
		binary[1+8] = (uint8_t)(val>>16);
		binary[2+8] = (uint8_t)(val>>8);
		binary[3+8] = (uint8_t)(val>>0);
	}
};
template <int dummy=0> // don't want it in .cpp
std::ostream& operator<<(std::ostream& os, const SerializedMessage& msg) {
	os << std::hex;
	for (unsigned i=0; i<msg.size(); i++) {
		os << (unsigned)msg.binary[i];
		if (i==3 || i==7)
			os << " ";
	}
	os << std::endl;
	os << std::dec;
	return os;
}
