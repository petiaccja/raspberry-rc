////////////////////////////////////////////////////////////////////////////////
//	File: net_server.h
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that provides the layer between the ServoState and the networking.
//
////////////////////////////////////////////////////////////////////////////////
//	TODO:
//		Nothing.
////////////////////////////////////////////////////////////////////////////////
//	DOCUMENTATION:
//	Methods:
//		Accept: Waits for, and accept the first and only client. Establishes TCP.
//		Authenticate: Performs a password checking, then gives the client the
//					  privileges to send messages.
//		ReadMessage: Pops exactly one Message from the socket input stream.
//		Respond: Sends Messages to the client, practical usage is to inform the
//				 client about errors, such as connection timeouts.
//		Cancel: Cancels a running network operation. Beware that when there is
//				not any pending operation, or the currently running is about to
//				return, the next operation is skipped.
//		Reset: Clears the effect of a previous Cancel call. Should be called
//			   when a Cancelled operation does not return with 'operation aborted',
//			   or when you think it's necessary because of imperfect 
//			   synchronization.
//			   Call it anytime you want, it won't cause harm.
//		Close: Shuts down the connection, and get back to accepting incoming 
//			   connections.
//	Thread-safety:
//		Distinct objects: thread-safe.
//		Single object: not thread-safe.
//			Thread-safe methods:
//			-Cancel
//			-Reset


#pragma once

#define _WIN32_WINNT 0x0600

#include "spinlock.h"

#include <boost/asio.hpp>

#include <chrono>
#include <cstdint>
#include <exception>
#include <string>
#include <atomic>
#include <mutex>

struct Message;


//	Errors
enum class RCS_ERROR : int {
	SUCCESS=0,
	UNKNOWN,
	TIMEOUT,
	ABORTED,
	INVALID_DATA,
	INVALID_CALL,
	WRONG_PASSWORD
};

////////////////////////////////////////////////////////////////////////////////
//	Performs accepting clients, authentication and further communication.
class RCServer {
	// private non-important or internal stuff
	class FLAG {
		friend class RCServer;

		std::atomic<unsigned> val;
		spinlock lk;
		
		FLAG() : val(0u) {}
		enum VALUE : unsigned {
			CANCEL	= 0x0001,
			WORKING	= 0x0002,	
		};
		inline void operator()(FLAG::VALUE flag, bool enable) {
			std::lock_guard<spinlock> guard(lk);
			if (enable) {
				val|=((unsigned)flag);
			}
			else {
				val&=(~(unsigned)flag);
			}
		}
		bool operator()(FLAG::VALUE flag) {
			std::lock_guard<spinlock> guard(lk);
			return (val & (unsigned short)flag)!=0;
		}
	};
	// only used for the two statics that specify IP version, boilerplate but nice for users
	class protocol {
		friend class RCServer;
		bool operator==(const protocol& other) {return val==other.val;}
		bool operator!=(const protocol& other) {return val!=other.val;}
		enum version : int {
			v4 = 4,
			v6 = 6
		};
		protocol(version val=v4) : val(val) {};
		version val;
	};
	// keeping status prevents user from doing shit
	enum class STATUS : unsigned short {
		STOPPED=0u,
		CONNECTED,
		AUTHENTICATED,
	};
public:
	// constructor
	RCServer(uint16_t port, protocol version=IPv4);
	~RCServer();

	// interface
	void Accept();
	void Authenticate(const char* password);
	Message ReadMessage();
	void Respond(Message message);
	void Cancel();
	void Reset();
	void Close();

	// protocol version args
	static const protocol IPv4;
	static const protocol IPv6;
private:
	// internal variables
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket clientSocket;
	STATUS status;
	FLAG flags;
	spinlock lk;
};


////////////////////////////////////////////////////////////////////////////////
//	Exception classes

class RCServerException : public std::runtime_error {
public:
	RCServerException()
		: std::runtime_error("Unknown error.") {}
	explicit RCServerException(const char* message)
		: std::runtime_error(message) {}
	explicit RCServerException(const std::string& message)
		: std::runtime_error(message) {}
};

template <int id, const char* defaultMessage>
class RCServerBaseException : public RCServerException {
public:
	RCServerBaseException()
		: RCServerException(defaultMessage) {}
	explicit RCServerBaseException(const char* message) 
		: RCServerException(message) {}
	explicit RCServerBaseException(const std::string& message) 
		: RCServerException(message) {}
};


// timeout
extern const char timedOutMessage[];
typedef RCServerBaseException<1, timedOutMessage>			TimedOut;

// aborted
extern const char operationAbortedMessage[];
typedef RCServerBaseException<2, operationAbortedMessage>	OperationAborted;

// invalid data
extern const char invalidDataMessage[];
typedef RCServerBaseException<3, invalidDataMessage>		InvalidData;

// invalid call
extern const char invalidCallMessage[];
typedef RCServerBaseException<4, invalidCallMessage>		InvalidCall;

// wrong password
extern const char invalidPasswordMessage[];
typedef RCServerBaseException<5, invalidPasswordMessage>	InvalidPassword;
