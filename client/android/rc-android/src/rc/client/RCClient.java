////////////////////////////////////////////////////////////////////////////////
//	File: RCClient.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Provides a class that deals with the network communication with the RCServer.
//
////////////////////////////////////////////////////////////////////////////////

package rc.client;

import java.net.*;
import java.io.*;


////////////////////////////////////////////////////////////////////////////////
//	Connects to a server, facilitates communication.
public class RCClient {
	//////////////////////////////////////////////
	// constructor	
	public RCClient() {	
	}	
	public RCClient(String address, int port)
			throws UnknownHostException, IOException, InvalidPasswordException, HostRejectedException, IllegalStateException
	{
		this(address, port, null);
	}	
	public RCClient(String address, int port, byte[] password)
			throws UnknownHostException, IOException, InvalidPasswordException, HostRejectedException, IllegalStateException
	{
		connect(address, port, password);
	}
	
	
	//////////////////////////////////////////////
	// communication
		
	// connect to the remote rc server
	public final void connect(String address, int port, byte[] password)
			throws UnknownHostException, IOException, InvalidPasswordException, HostRejectedException, IllegalStateException
	{
		connect(address, port, password, 0);
	}
	public final void connect(String address, int port, byte[] password, int timeoutMillis)
			throws UnknownHostException, IOException, InvalidPasswordException, HostRejectedException, IllegalStateException
	{
		if (isConnected())
			throw new IllegalStateException("Already connected");
		// establish connection with the server
		boolean error = true;
		try {
			socket = new Socket();
			socket.connect(new InetSocketAddress(address, port), timeoutMillis);
			out = socket.getOutputStream();
			in = socket.getInputStream();
			dis = new DataInputStream(in);
			error = false;
		}
		catch (UnknownHostException e) {
			throw e;
		}
		catch (IOException e) {
			throw e;
		}
		finally {
			if (error) {
				if (dis!=null)		{try {dis.close();}		catch (Exception e){}}
				if (in!=null)		{try {in.close();}		catch (Exception e){}}
				if (out!=null)		{try {out.close();}		catch (Exception e){}}
				if (socket!=null)	{try {socket.close();}	catch (Exception e){}}
				socket=null; in=null; out=null; dis=null;
			}
		}
		
		// perform authentication
		try {
			Message reply;
			// send auth request
			send(new Message(RCInstruction.AUTHENTICATE, password==null?0:password.length));
			// read response
			reply = read();
				// check response
			if (reply.instruction!=RCInstruction.SUCCESS) {
				throw new HostRejectedException("Server decilined authentication request.");
			}
			// send password
			if (password!=null)
				send(password);
			// read response
			reply = read();
			if (reply.instruction!=RCInstruction.SUCCESS) {
				throw new InvalidPasswordException("Incorrect password.");
			}
		}
		catch (IOException e) {
			close();
			throw e;
		}
		catch (InvalidPasswordException e) {
			close();
			throw e;
		}
		catch (HostRejectedException e) {
			close();
			throw e;
		}
	}
	
	// send one message
	public void send(Message message) throws IllegalStateException, IOException {
		if (socket==null)
			throw new IllegalStateException("Not connected.");
		SerializedMessage smsg = new SerializedMessage(message);
		out.write(smsg.binary);
	}
	private void send(byte[] bytes) throws IllegalStateException, IOException {
		if (socket==null)
			throw new IllegalStateException("Not connected.");
		out.write(bytes);
	}	
	
	// read one message
	public Message read() throws IllegalStateException, IOException {
		if (socket==null)
			throw new IllegalStateException("Not connected.");
		SerializedMessage smsg = new SerializedMessage();
		dis.readFully(smsg.binary);
		return smsg.toMessage();
	}
		
	// close connection, free resources
	public void close() {
		if (socket!=null && socket.isConnected()==true) {
			// if socket if connected, none of these if null
			try {dis.close();}		catch (Exception e){}
			try {in.close();}		catch (Exception e){}
			try {out.close();}		catch (Exception e){}
			try {socket.close();}	catch (Exception e){}
			socket=null; in=null; out=null; dis=null;
		}
	}
	
	
	//////////////////////////////////////////////
	// query/set
	public boolean isConnected() {
		return socket!=null;
	}
	
	
	
	//////////////////////////////////////////////
	// internal
	private Socket socket;
	private OutputStream out;
	private InputStream in;
	private DataInputStream dis;
}