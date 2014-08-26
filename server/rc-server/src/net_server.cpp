////////////////////////////////////////////////////////////////////////////////
//	File: net_server.h
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header.
//
////////////////////////////////////////////////////////////////////////////////

#include "net_server.h"
#include "message.h"

#include <boost/array.hpp>

#include <future>
#include <functional>
#include <iostream>

#include <thread>

#define ATOMIC_BLOCK_BEGIN	lk.lock()
#define ATOMIC_BLOCK_END	lk.unlock()
#define CLEAR_CANCEL		flags(FLAG::CANCEL, false)
#define CLEAR_WORKING		flags(FLAG::WORKING, false)
//#define CHECK_CANCEL		if (flags(FLAG::CANCEL)) {CLEAR_CANCEL; ATOMIC_BLOCK_END; throw RCS_ERROR::ABORTED;}

#define __DEBUG_SLEEP__		this_thread::sleep_for(chrono::milliseconds(1000))


using namespace std;
using boost::asio::ip::tcp;
using boost::asio::async_read;
using boost::asio::async_write;
using boost::asio::buffer;


////////////////////////////////////////
//	Simple helper that calls a function when it's destroyed
template <class>
class at_scope_exit;

template <class Func>
class at_scope_exit<Func()> : public std::function<Func()> {
public:
	template <class...ConstrArgs>
	at_scope_exit(ConstrArgs&&...args) 
		: std::function<Func()>(args...)
	{
		return;
	}
	~at_scope_exit() {
		this->operator()();
	}
};

////////////////////////////////////////
//	This code piece is useful to disallow cancels
//	when no function is running.
//	Problems:
//		<Thread 1>						<Thread 2>
//		CALL RCServer
//		PUSH EBP						CALL Cancel
//		MOV EBP, ESP					if (flags & FLAG::WORKING) {
//		NOP								OR flags, FLAG::CANCEL
//		NOP								}
//		OR flags, FLAGS::WORKING		NOP
//	Thread 2 is interrupting Thread 2, however the interrupt will be
//	rejected since Thread 1 didn't reach the point when it's set 
//	to running state.
/*
	// clear working and cancel flag at scope exit
	flags(FLAG::WORKING, true);
	at_scope_exit<void()> cleanFlags =
		[this]() mutable {
			CLEAR_CANCEL;
			CLEAR_WORKING;
		};
*/

////////////////////////////////////////////////////////////////////////////////
//	RCServer class' methods  |
//							 V

////////////////////////////////////////
//	Protocols
const RCServer::protocol RCServer::IPv4(RCServer::protocol::v4);
const RCServer::protocol RCServer::IPv6(RCServer::protocol::v6);

////////////////////////////////////////
//	Constructor
//		Exceptions:
//			RCServerExeption on unexpected networking failure.
RCServer::RCServer(uint16_t port, protocol version) 
	: 
	ioService(),
	acceptor(ioService),
	clientSocket(ioService),
	status(STATUS::STOPPED)
{
	try {
		tcp::endpoint endpoint(version==IPv4?tcp::v4():tcp::v6(), port);
		acceptor.open(endpoint.protocol());
		acceptor.bind(endpoint);
		acceptor.listen();
	}
	catch (boost::system::system_error& e) {
		throw RCServerException(string("Network error, ") + e.what());
	}
}
RCServer::~RCServer() {
	clientSocket.close();
	acceptor.close();
}

////////////////////////////////////////
//	Connection flow


//	Wait for incoming connections
//		Exceptions:
//			RCServerException on unexpected networking failure.
void RCServer::Accept() {
	// used correctly?
	if (status!=STATUS::STOPPED)
		throw InvalidCall("Already connected.");

	// clear cancel flags
	at_scope_exit<void()> atExit([this](){CLEAR_CANCEL;});

	// result of the asynchronous operation
	boost::system::error_code ec;
	// check for cancel flag and accept
	ATOMIC_BLOCK_BEGIN;
	if (flags(FLAG::CANCEL)) {
		ATOMIC_BLOCK_END;
		throw OperationAborted("Listening was aborted.");
	}
	// accept one client
	acceptor.async_accept(
		clientSocket,
		[&ec](const boost::system::error_code& error) mutable -> void {
			ec = error;
		}
	);
	ATOMIC_BLOCK_END;

	ioService.reset();
	ioService.run();

	// handle errors
	if (ec==boost::system::errc::success) {
		// success!
		status = STATUS::CONNECTED;
	}
	else if (ec==boost::asio::error::operation_aborted) {
		throw OperationAborted("Listening was aborted.");
	}
	else {
		throw RCServerException(string("Network error, ") + boost::system::system_error(ec).what());
	}
}

//	Perform authentication process.
//		Exceptions:
//			RCServerException on unexpected networking failure.
//		Returns:
//			the error names are descriptive enough
void RCServer::Authenticate(const char* password) {
	// password check
	if (password && strlen(password)==0u)
		password = nullptr;

	// used correctly?
	if (status!=STATUS::CONNECTED) {
		const char* extraInfo = nullptr;
		switch (status) {
			case STATUS::STOPPED:
				extraInfo = "Not connected.";
				break;
			case STATUS::AUTHENTICATED:
				extraInfo = "Already authenticated.";
				break;
			default:
				extraInfo = nullptr;
		}
		throw InvalidCall(extraInfo);
	}

	// clear cancel flags
	at_scope_exit<void()> atExit([this](){CLEAR_CANCEL;});

	// variables used in function
	Message message, reply1, reply2;
	SerializedMessage smessage;
	//cout << smessage << endl;
	vector<char> passwordBuffer;
	boost::system::error_code ec;
	
	// read authentication message
	ATOMIC_BLOCK_BEGIN;
	if (flags(FLAG::CANCEL)) {
		ATOMIC_BLOCK_END;
		throw OperationAborted("Reading authentication request was aborted.");
	}
	async_read(
		clientSocket,
		buffer((void*)smessage.binary, smessage.size()),
		[&](const boost::system::error_code& error, size_t nBytesRead) {
			ec = error;
		}
	);
	ATOMIC_BLOCK_END;

	ioService.reset();
	ioService.run();
	message = smessage;
	if (ec==boost::asio::error::operation_aborted)
		throw OperationAborted("Reading authentication request was aborted.");
	else if (ec!=boost::system::errc::success)
		throw RCServerException(string("Network error,") + boost::system::system_error(ec).what());

	// bad authentication message
	if (message.instruction!=RCI_AUTHENTICATE || message.param1.u>=256) {		
		reply1 = Message(RCI_ERROR_INVALID_DATA,0,0);
	}
	// good authentication message
	else {
		reply1 = Message(RCI_SUCCESS, 0,0);
	}

	// send a reply
	ATOMIC_BLOCK_BEGIN;
	if (flags(FLAG::CANCEL)) {
		ATOMIC_BLOCK_END;
		throw OperationAborted("Sending reply #1 was aborted.");
	}
	smessage = reply1;
	async_write(
		clientSocket,
		buffer(smessage.binary, smessage.size()),
		[&](const boost::system::error_code& error, size_t nBytesRead) {
			ec = error;
		}
	);
	ATOMIC_BLOCK_END;
	ioService.reset();
	ioService.run();
	if (ec==boost::asio::error::operation_aborted)
		throw OperationAborted("Sending reply #1 was aborted.");
	else if (ec != boost::system::errc::success)
		throw RCServerException(string("Network error, ") + boost::system::system_error(ec).what());

	// bad authentication message -> cancel
	if (reply1.instruction!=RCI_SUCCESS)
		return throw InvalidData("Message recieved is not authentication request.");

	// read password
	bool isPasswordCorrect = (password==nullptr);
	if (message.param1.u>0u) {
		passwordBuffer.resize(message.param1, '\0');
		ATOMIC_BLOCK_BEGIN;
		if (flags(FLAG::CANCEL)) {
			ATOMIC_BLOCK_END;
			throw OperationAborted("Reading password was aborted.");
		}
		async_read(
			clientSocket,
			buffer(passwordBuffer),
			[&](const boost::system::error_code& error, size_t nBytesRead) {
				ec = error;
			}
		);
		ATOMIC_BLOCK_END;
		ioService.reset();
		ioService.run();
		if (ec==boost::asio::error::operation_aborted)
			throw OperationAborted("Reading password was aborted.");
		else if (ec != boost::system::errc::success)
			throw RCServerException(string("Network error, ") + boost::system::system_error(ec).what());

		// validate password
		if (password == nullptr) {
			isPasswordCorrect = true;
		}
		else if (strlen(password)==passwordBuffer.size()) {
			bool match=true;
			for (size_t i=0; i<passwordBuffer.size(); i++) {
				if (password[i]!=passwordBuffer[i]) {
					match=false;
					break;
				}
			}
			isPasswordCorrect=match;
		}
	}

	// good password
	if (isPasswordCorrect) {
		reply2 = Message(RCI_SUCCESS, 0,0);
	}
	// bad password
	else {
		reply2 = Message(RCI_ERROR_PASSWORD, 0,0);
	}

	// send a reply
	ATOMIC_BLOCK_BEGIN;
	if (flags(FLAG::CANCEL)) {
		ATOMIC_BLOCK_END;
		throw OperationAborted("Sending reply #2 was aborted..");
	}
	smessage = reply2;
	async_write(
		clientSocket,
		buffer(smessage.binary, smessage.size()),
		[&](const boost::system::error_code& error, size_t) {
			ec = error;
		}
	);
	ATOMIC_BLOCK_END;
	ioService.reset();
	ioService.run();
	if (ec==boost::asio::error::operation_aborted)
		throw OperationAborted("Sending reply #2 was aborted..");
	else if (ec != boost::system::errc::success)
		throw RCServerException(string("Network error, ") + boost::system::system_error(ec).what());

	// good password
	if (isPasswordCorrect) {
		status = STATUS::AUTHENTICATED;
	}
	// bad password
	else {
		throw InvalidPassword("Password sent by client is invalid.");
	}
}

//	Reads one message from the socket
//		Exceptions:
//			RCServerException thrown on networking failure.
//		Return value: the actual message or RCI_ERROR_TIMEOUT on cancel
Message RCServer::ReadMessage() {
	// used correctly?
	if (status!=STATUS::AUTHENTICATED) {
		const char* extraInfo = nullptr;
		switch (status) {
			case STATUS::STOPPED: 
				extraInfo = "Not connected.";
				break;
			case STATUS::CONNECTED: 
				extraInfo = "Authentication required.";
				break;
			default: 
				extraInfo = nullptr;
		}
		throw InvalidCall(extraInfo);
	}

	// clear cancel flags
	at_scope_exit<void()> atExit([this](){CLEAR_CANCEL;});

	Message message;
	SerializedMessage smessage;
	boost::system::error_code ec;

	// read a message
	ATOMIC_BLOCK_BEGIN;
	if (flags(FLAG::CANCEL)) {
		ATOMIC_BLOCK_END;
		throw OperationAborted("Reading socket was aborted.");
	}
	async_read(
		clientSocket,
		buffer(smessage.binary, smessage.size()),
		[&](const boost::system::error_code& error, size_t) {
			ec = error;
		}
	);
	ATOMIC_BLOCK_END;
	ioService.reset();
	ioService.run();
	message = smessage;
	if (ec==boost::asio::error::operation_aborted)
		throw OperationAborted("Reading from socket was aborted.");
	else if (ec != boost::system::errc::success)
		throw RCServerException(string("Network error, ") + boost::system::system_error(ec).what());
	
	return message;
}


//	Send a Message to the client
//		Exceptions: RCServerException& thrown on networking failure.
void RCServer::Respond(Message message) {
	// used correctly?
	if (status==STATUS::STOPPED) {
		throw InvalidCall("Not connected.");
	}
	boost::system::error_code ec;

	// clear cancel flags
	at_scope_exit<void()> atExit([this](){CLEAR_CANCEL;});

	// write message
	ATOMIC_BLOCK_BEGIN;
	if (flags(FLAG::CANCEL)) {
		ATOMIC_BLOCK_END;
		throw OperationAborted("Writing to socket was aborted.");
	}
	SerializedMessage smessage = message;
	async_write(
		clientSocket,
		buffer(smessage.binary, smessage.size()),
		boost::asio::transfer_all(),
		[&](const boost::system::error_code error, size_t) {
			ec = error;
		}
	);
	ATOMIC_BLOCK_END;
	ioService.reset();
	ioService.run();
	if (ec==boost::asio::error::operation_aborted)
		throw OperationAborted("Writing to socket was aborted.");
	else if (ec != boost::system::errc::success)
		throw RCServerException(string("Network error, ") + boost::system::system_error(ec).what());
}



//	Cancel pending operations, but not closing connection!
//		Notes: 
//			- this IS thread-safe
//			- there might be some delay till the thread reaches a suitable point to exit
//			- the NEXT(!) OR CURRENT operation will be cancelled 
//			  (basically the next operation instantly returns)			  
//		Exceptions: none
//		Return: if you cancelled something, it's cancelled, no need for feedback
void RCServer::Cancel() {
	ATOMIC_BLOCK_BEGIN;
	flags(FLAG::CANCEL, true);
	try {
		clientSocket.cancel();
	}
	catch (...) {
		/*swallow exceptions */
	}
	ATOMIC_BLOCK_END;
}
//	Get the server back to a stable state.
//		Under-the-hood:
//			Cancels already posted cancel signal, thus allows the system to 
//			continue functioning as expected.
//		Usage:
//			- if a cancelled operation does NOT return with 'operation aborted' error,
//			  you should call this
void RCServer::Reset() {
	ATOMIC_BLOCK_BEGIN;
	flags(FLAG::CANCEL, false);
	ATOMIC_BLOCK_END;
}

// Close connection, reset state to accepting
//		Note: this is NOT thread-safe
//		Exceptions: none
void RCServer::Close() {
	try {
		clientSocket.close();
	}
	catch (...) {
		/* swallow exceptions */
	}
	status = STATUS::STOPPED;
}




////////////////////////////////////////////////////////////////////////////////
//	Exceptions
const char timedOutMessage[]			= "Operation timed out.";
const char operationAbortedMessage[]	= "Operation aborted.";
const char invalidDataMessage[]			= "Invalid data.";
const char invalidCallMessage[]			= "Invalid call.";
const char invalidPasswordMessage[]		= "Invalid password.";