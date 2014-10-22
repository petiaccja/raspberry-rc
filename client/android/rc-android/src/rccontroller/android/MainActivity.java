////////////////////////////////////////////////////////////////////////////////
//	File: MainActivity.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This contains the main logic for the RC Controller application.
//	For god's sake, rename this file and class...
////////////////////////////////////////////////////////////////////////////////



package rccontroller.android;

import android.app.Activity;
import android.os.Bundle;
import android.widget.*;
import android.view.View;
import android.content.Intent;

import java.net.UnknownHostException;
import java.io.IOException;
import rc.client.*;

////////////////////////////////////////////////////////////////////////////////
//	Android main
public class MainActivity extends Activity {
	final int THROTTLE_SERVO = 11;
	final int STEERING_SERVO = 17;
	// network client
	private RCClient client;
	//private Messenger messenger;
	boolean isRunning;
	int count;
	boolean isConnected;
    // widgets
	private TextView connectMessage;
	private ProgressBar connectProgress;
	private EditText addressEdit;
	private EditText passwordEdit;
	private Button connectButton;
	
    // Called when the activity is first created.
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
		client = new RCClient();	// communication with server
		try {
			setContentView(R.layout.main);
		}
		catch (Exception e) {
			System.out.println(e.getMessage());
		}
        isConnected = false;
		SharedMessenger.messenger = new Messenger(client, STEERING_SERVO, THROTTLE_SERVO);	// GPIO 11 and 17 HARDCODED!
		
        // init widgets
		connectMessage	= (TextView)this.findViewById(R.id.connectMessage);
		connectProgress	= (ProgressBar)this.findViewById(R.id.connectProgress);
		addressEdit		= (EditText)this.findViewById(R.id.addressEdit);
		passwordEdit	= (EditText)this.findViewById(R.id.passwordEdit);
		connectButton	= (Button)this.findViewById(R.id.connectButton);
		
		connectProgress.setVisibility(View.INVISIBLE);
		connectMessage.setVisibility(View.INVISIBLE);
	}
	// called when Connect button is clicked
	public void onConnect(View v) {
		// connect to a server
		if (!isConnected) {
			// gui feedback that connectiing is in progress
			connectMessage.setText("Connecting...");
			connectMessage.setVisibility(View.VISIBLE);
			connectButton.setClickable(false);

			// extract address
			String strAddr = addressEdit.getText().toString();
			int idxSeparator = strAddr.indexOf(':');
			if (idxSeparator==-1) {
				connectMessage.setText("Please specify a port.");
				connectButton.setClickable(true);
				return;
			}
			String address = strAddr.substring(0, idxSeparator);
			String strPort = strAddr.substring(idxSeparator+1, strAddr.length());
			int port;
			try {
				port = Integer.parseInt(strPort);
			}
			catch (NumberFormatException e) {
				port = -1;
			}
			if (port<1024 || port>65535) {
				connectMessage.setText("Invalid port.\nMust be in range 1024-65535.");
				connectButton.setClickable(true);
				return;
			}

			connectProgress.setVisibility(View.VISIBLE); // visual feedback
			
			// extract password
			String strPassword = passwordEdit.getText().toString();

			// try to connect
			new Thread(new AsyncConnect(address, port, strPassword.getBytes())).start();
		}
		// disconnect
		else {
			client.close();
			connectButton.setText("Connect");
			connectMessage.setVisibility(View.INVISIBLE);
			isConnected = false;
		}
	}
	
	// connects to the server, run asynchronously, not to block UI
	private class AsyncConnect implements Runnable {
		private String address;
		private int port;
		private byte[] password;
		private String message;
		public AsyncConnect(String address, int port, byte[] password) {
			this.address = address;
			this.port = port;
			this.password = password;
		}
		public void run() {
			message = "Success!";
			try {
				client.connect(address, port, password, 5000);
			}
			catch (UnknownHostException e) {
				message = "Host unreachable.";
			}
			catch (IOException e) {
				message = "Failed to connect.\n"/*+e.getMessage()*/;
			}
			catch (InvalidPasswordException e) {
				message = "Invalid password.";
			}
			catch (HostRejectedException e) {
				message = "Host rejected the connection.";
			}
			catch (IllegalStateException e) {
				message = "Already connected.";
			}
			runOnUiThread(new Runnable() {
				public void run() {
					// set widgets
					connectButton.setClickable(true);
					connectMessage.setText(message);
					connectProgress.setVisibility(View.INVISIBLE);					
					if (message.equals("Success!")) {
						// connected to server
						// add the servos
						try {
							client.send(new Message(RCInstruction.ADD_SERVO, STEERING_SERVO));
							client.send(new Message(RCInstruction.ADD_SERVO, THROTTLE_SERVO));
						}
						catch (IOException e) {
							connectMessage.setText("Failed to communicate with server.");
							client.close();
							return;
						}
						// set post-connection stuff
						isConnected = true;
						connectButton.setText("Disconnect");
						// start new intent for controlling :)
						Intent intent = new Intent(MainActivity.this, ControlActivity.class);
						MainActivity.this.startActivity(intent);
					}
				}
			});
		}
	}
}
