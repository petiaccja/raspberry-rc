////////////////////////////////////////////////////////////////////////////////
//	File: Main.cpp
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Global application logic
////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define _WIN32_WINNT 0x0600
#endif

#include "servo_state.h"
#include "message.h"
#include "net_server.h"

#include <iostream>
#include <exception>

#include <queue>

#include <thread>
#include <mutex>
#include <future>
#include <atomic>


#ifdef _WIN32
#include <conio.h>
#else
inline char _getch() {
	std::cin.get();
}
#endif

using namespace std;


////////////////////////////////////////////////////////////////////////////////
//	Prototypes


////////////////////////////////////////////////////////////////////////////////
//	Globals

////////////////////////////////////////////////////////////////////////////////
//	Functions
void ShowHelp() {
	cout <<
		"Usage:\n"
		"    rcserver --port <port> --password <password> [--ipv6]\n"
		"\n"
		"    Options:\n"
		"        --port, -p <integer>: The port on which to listen to incoming connections."
		"            Accepted range is [1024-65535] inclusive.\n"
		"        --password, -pw <string>: Password required by clients to join.\n"
		"        --ipv6, -6 <none>: Optional. Establishes server on the IPv6 network.\n"
		"        --help, -h: Display this message\n";
}



////////////////////////////////////////////////////////////////////////////////
//	Application entry point.
int main(int argc, char* argv[]) {
	// parse arguments
	// planned options:
	//	-p --port <port number>
	//	-pw --password <password>
	//	[-ipv6]
	//	-h --help
	unsigned short port = 0;
	char* password = nullptr;
	bool usev6 = false;

	for (int i=1; i<argc; i++) {
		string s = argv[i];
		if (s=="-p" || s=="--port") {
			i++;
			if (i>=argc)
				break;
			port = (unsigned short)atoi(argv[i]);
		}
		else if (s=="-pw" || s=="--password") {
			i++;
			if (i>=argc)
				break;
			password = argv[i];
		}
		else if (s=="-6" || s=="--ipv6") {
			usev6 = true;
		}
		else if (s=="-h" || s=="--help") {
			ShowHelp();
			return 0;
		}
		else {
			cout << "Invalid argument(s). Try --help for syntax.\n";
			return 1;
		}
	}

	if (port<1024) {
		cout << "Ports between 1024 and 65535 may only be used.\n";
		return 2;
	}
	if (password==nullptr) {
		cout << "You did not specify a password.\n";
		return 2;
	}

	// get the server and the UI up
	try {
		// create a server instance
		RCServer server(port, usev6 ? RCServer::IPv6 : RCServer::IPv4);
		ServoState controller;

		cout << "Now listening to all incoming connections.\n"
				"Press CTRL+C to stop and exit.\n\n";

		// accept clients in a loop -> press CTRL-C to exit
		while (true) {
			server.Close();
			// start listening and authenticate
			cout << "________________________________________\n";
			cout << "Waiting for incoming connection...\n";

			// try to establish secure connection with client
			try {
				server.Accept();
				cout << "Client accepted!\n";
				cout << "Authenticating...\n";
				server.Authenticate(password);
				cout << "Success!\n";
			}
			catch (OperationAborted&) {
				cout << "Aborted.\n";
				continue;
			}
			catch (InvalidPassword&) {
				cout << "Invalid password.\n";
				continue;
			}
			catch (exception& e) {
				cout << "Failed, " << e.what() << endl;
				continue;
			}

			// measure duration of connection
			auto startTime = chrono::steady_clock::now();

			// process incoming messages
			cout << "Processing messages...\n\n";
			queue<Message> messageFifo;
			promise<bool> isThereMessage;
			future<bool> isThereMessageFuture = isThereMessage.get_future();
			atomic<bool> recieveMessage(true);
			atomic<bool> confirmRead(false);
			mutex lk;
			unsigned timeout = 500;
			// message reader thread
			thread msgThread(
				[&]() {
					while (recieveMessage) {
						// read from socket
						Message msg(RCI_SUCCESS);
						try {
							msg = server.ReadMessage();
						}
						catch (exception&) {
							//MessageBoxA(nullptr, e.what(), "ReadMessage Error", MB_OK);
							msg = Message(RCI_ERROR_UNKNOWN);
						}
						// push to fifo
						unique_lock<mutex> g(lk);
						messageFifo.push(msg);
						if (confirmRead.load()==false) {
							isThereMessage.set_value(true);
							confirmRead = true;
						}
						if (msg.instruction==RCI_ERROR_UNKNOWN)
							break;
					}
				}
			);
			// processing loop
			/* DEBUG */
			auto timeStart = chrono::steady_clock::now();
			auto timePrev = timeStart;
			/* DEBUG */
			Message msg(RCI_SUCCESS);
			do {				
				// wait for a message
				auto result = isThereMessageFuture.wait_for(chrono::milliseconds(timeout));
				if (result!=future_status::ready) {
					controller.Suspend();
					continue;
				}
				
				isThereMessageFuture.get();

				// process messages on list
				unique_lock<mutex> g(lk);
				confirmRead = false;
				isThereMessage = promise<bool>();
				isThereMessageFuture = isThereMessage.get_future();

				size_t nMessages = messageFifo.size();

				// process messages
				for (size_t i=0; i<nMessages; i++) {
					msg = messageFifo.front();
					messageFifo.pop();
					/* DEBUG */
					auto timeNow = chrono::steady_clock::now();
					auto elapsed = chrono::duration_cast<chrono::milliseconds>(timeNow - timeStart);
					// print timestamp
					printf("[%7.3f] ", elapsed.count()/1000.f);
					printf("[%4d] ", chrono::duration_cast<chrono::milliseconds>(timeNow - timePrev));
					timePrev = timeNow;
					switch (msg.instruction) {
						case RCI_ADD_SERVO:
							cout << "Add servo:\t" << msg.param1.u << " pin" << endl; break;
						case RCI_SET_STEERING:
							cout << "Set steering:\t" << msg.param1.u << " pin\t" << msg.param2.fp << endl; break;
						case RCI_KEEP_ALIVE:
							cout << "Keep alive" << endl; break;
						default:
							cout << "Other message:\t" << msg.instruction << endl; break;
					}
					/* DEBUG */
					switch (msg.instruction) {
						case RCI_SET_TIMEOUT:
							timeout = msg.param1;
							break;
						case RCI_KEEP_ALIVE:
							break;
						default:
							controller.MessageProc(msg.instruction, msg.param1, msg.param2);
					}
				}
			}
			while (msg.instruction != RCI_QUIT && msg.instruction != RCI_ERROR_UNKNOWN);

			
			// connection ended
			cout << "Session ended\n   duration: "
				<< chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now()-startTime).count()
				<< " seconds\n\n";

			// cleanup
			recieveMessage = false;
			server.Cancel();
			msgThread.join();
			server.Reset();
			server.Close();
		}
	}
	catch (RCServerException& e) {
		cout << e.what() << endl;
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	// exit procedure
	cout << "Press enter to exit...";
	_getch();
	return 0;
};









