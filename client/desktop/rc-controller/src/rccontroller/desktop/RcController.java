
package rccontroller.desktop;

import java.awt.event.WindowEvent;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import rc.client.*;


/**
 * Main frame.
 * Contains all code for matching GUI with RcClient and the GUI itself.
 */
public class RcController extends javax.swing.JFrame {
	private RcClient client;
	private int pinThrottle, pinSteering;
	private ConnectFrame connectFrame;
	private SettingsFrame settingsFrame;
	private String address;
	private int port;
	
	private float epaThrottleMin;
	private float epaThrottleMax;
	private float epaSteeringMin;
	private float epaSteeringMax;
	
	
	/**
	 * Creates new form RcController.
	 */
	public RcController() {
		client = new RcClient();
		connectFrame = new ConnectFrame(this);
		settingsFrame = new SettingsFrame(this);
		
		// UI CODE HERE
		initComponents();
		
		// try to load last settings
		try {
			FileInputStream file = new FileInputStream("settings.dat");
			ObjectInputStream reader = new ObjectInputStream(file);
			SaveSettings settings = (SaveSettings)reader.readObject();
			
			address = settings.address;
			port = settings.port;
			pinThrottle = settings.pinThrottle;
			pinSteering = settings.pinSteering;
			
			epaSteeringMin = settings.epaSteeringMin;
			epaSteeringMax = settings.epaSteeringMax;
			epaThrottleMin = settings.epaThrottleMin;
			epaThrottleMax = settings.epaThrottleMax;
			
			connectFrame.restoreValues(settings);
			settingsFrame.restoreValues(settings);
		}
		catch (FileNotFoundException e) {
			/* no settings, don't care */
		}
		catch (IOException | ClassNotFoundException e) {
			/* don't care */
		}
	}
	
	/**
	 * Set throttle.
	 */
	void setThrottle(float value) {
		if (!client.isConnected()) {
			return;
		}
		
		value = Math.max(0.0f, Math.min(1.0f, value));
		try {
			client.setSteering(pinThrottle, value);
		}
		catch (Throwable e) {
			/* om-nom-nom swallow */
		}
	}
	
	/**
	 * Set steering.
	 */
	void setSteering(float value) {
		if (!client.isConnected()) {
			return;
		}
		
		value = Math.max(0.0f, Math.min(1.0f, value));
		try {
			client.setSteering(pinSteering, value);
		}
		catch (Throwable e) {
			/* swallow */
		}
	}
	
	/**
	 * Set epa for steering.
	 * @param min In percent.
	 * @param max In percent.
	 */
	void setEpaSteering(float min, float max) {
		min = Math.max(0.0f, Math.min(min, max));
		max = Math.min(Math.max(min, max), 100.0f);
		epaSteeringMin = min;
		epaSteeringMax = max;
	}
	/**
	 * Set epa for throttle.
	 * @param min In percent.
	 * @param max In percent.
	 */
	void setEpaThrottle(float min, float max) {
		min = Math.max(0.0f, Math.min(min, max));
		max = Math.min(Math.max(min, max), 100.0f);
		epaThrottleMin = min;
		epaThrottleMax = max;
	}
	
	/**
	 * Set timing for steering.
	 * @param min In microseconds.
	 * @param max In microseconds.
	 */
	void setTimingSteering(float min, float max) {
		if (!client.isConnected())
			return;
		
		min = Math.min(min, max);
		max = Math.max(min, max);
		try {
			client.setMinWidth(pinSteering, min);
			client.setMaxWidth(pinSteering, max);
		}
		catch (Exception e) {
			/* swallow */
		}
	}
	/**
	 * Set timing for throttle.
	 * @param min In microseconds.
	 * @param max In microseconds.
	 */
	void setTimingThrottle(float min, float max) {
		if (!client.isConnected())
			return;
		
		min = Math.min(min, max);
		max = Math.max(min, max);
		try {
			client.setMinWidth(pinThrottle, min);
			client.setMaxWidth(pinThrottle, max);
		}
		catch (Exception e) {
			/* swallow */
		}
	}
	
	/**
	 * Returns the underlying client.
	 */
	public RcClient getClient() {
		return client;
	}
	
	/**
	 * Connect to a server.
	 * @param address Address of server.
	 * @param port Remote port on the server.
	 * @param password Authentication password.
	 * @param pinThrottle Which physical GPIO pin of the Raspy to drive for throttle.
	 * @param pinSteering Which physical GPIO pin of the Raspy to drive for steering.
	 * @throws InvalidPasswordException Password is invalid.
	 * @throws HostRejectedException Server refused to connect for some reason.
	 * @throws IllegalStateException You can't call that since you are already connceted.
	 * @throws IOException Network error occured.
	 * @throws Exception Could not add servos. Check message.
	 */
	public void connect(String address, int port, byte[] password, int pinThrottle, int pinSteering) 
			throws InvalidPasswordException, HostRejectedException, IllegalStateException, IOException, Exception
	{
		// check pins
		// i'm gonna do it sometime
		
		// save stuff
		this.address = address;
		this.port = port;
		
		// connect & add servos
		client.connect(address, port, password);
		
		try {
			client.addServo(pinThrottle);
			this.pinThrottle = pinThrottle;
			client.addServo(pinSteering);
			this.pinSteering = pinSteering;	
		}
		catch (IOException e) {
			client.close();
			throw new Exception("failed to add servos");
		}
	}
	
	/**
	 * Disconnect form server.
	 * Closes network connection, server is reseted automatically.
	 */
	public void disconnect() {
		if (client.isConnected()) {
			client.close();
		}
	}

	////////////////////////////////////
	// UI CODE HERE
	
	/**
	 * This method is called from within the constructor to initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is always
	 * regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        jMenuItem4 = new javax.swing.JMenuItem();
        mainPanel = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        sliderThrottle = new javax.swing.JSlider();
        tfThrottle = new javax.swing.JTextField();
        sliderSteering = new javax.swing.JSlider();
        tfSteering = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        menuBar = new javax.swing.JMenuBar();
        mFile = new javax.swing.JMenu();
        miExit = new javax.swing.JMenuItem();
        mConnect = new javax.swing.JMenu();
        miConnect = new javax.swing.JMenuItem();
        miDisconnect = new javax.swing.JMenuItem();
        mTools = new javax.swing.JMenu();
        miSettings = new javax.swing.JMenuItem();

        jMenuItem4.setText("jMenuItem4");

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("RCController");
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                formWindowClosing(evt);
            }
        });
        getContentPane().setLayout(new javax.swing.BoxLayout(getContentPane(), javax.swing.BoxLayout.Y_AXIS));

        mainPanel.setMaximumSize(new java.awt.Dimension(32767, 100));
        mainPanel.setLayout(new java.awt.GridBagLayout());

        jLabel2.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel2.setText("Servos:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        mainPanel.add(jLabel2, gridBagConstraints);

        sliderThrottle.setMajorTickSpacing(10);
        sliderThrottle.setMinorTickSpacing(2);
        sliderThrottle.setPaintTicks(true);
        sliderThrottle.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                sliderThrottleStateChanged(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.weightx = 1.0;
        mainPanel.add(sliderThrottle, gridBagConstraints);

        tfThrottle.setText("50%");
        tfThrottle.setMaximumSize(new java.awt.Dimension(80, 25));
        tfThrottle.setMinimumSize(new java.awt.Dimension(4, 25));
        tfThrottle.setPreferredSize(new java.awt.Dimension(80, 25));
        tfThrottle.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                tfThrottleActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 1;
        mainPanel.add(tfThrottle, gridBagConstraints);

        sliderSteering.setMajorTickSpacing(10);
        sliderSteering.setMinorTickSpacing(2);
        sliderSteering.setPaintTicks(true);
        sliderSteering.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                sliderSteeringStateChanged(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.weightx = 1.0;
        mainPanel.add(sliderSteering, gridBagConstraints);

        tfSteering.setText("50%");
        tfSteering.setMaximumSize(new java.awt.Dimension(80, 25));
        tfSteering.setMinimumSize(new java.awt.Dimension(4, 25));
        tfSteering.setPreferredSize(new java.awt.Dimension(80, 25));
        tfSteering.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                tfSteeringActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 2;
        mainPanel.add(tfSteering, gridBagConstraints);

        jLabel1.setText("Throttle:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        mainPanel.add(jLabel1, gridBagConstraints);

        jLabel3.setText("Steering:");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        mainPanel.add(jLabel3, gridBagConstraints);

        getContentPane().add(mainPanel);

        mFile.setText("File");

        miExit.setText("Exit");
        miExit.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miExitActionPerformed(evt);
            }
        });
        mFile.add(miExit);

        menuBar.add(mFile);

        mConnect.setText("Connect");

        miConnect.setText("Connect");
        miConnect.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miConnectActionPerformed(evt);
            }
        });
        mConnect.add(miConnect);

        miDisconnect.setText("Disconnect");
        miDisconnect.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miDisconnectActionPerformed(evt);
            }
        });
        mConnect.add(miDisconnect);

        menuBar.add(mConnect);

        mTools.setText("Tools");

        miSettings.setText("Settings");
        miSettings.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miSettingsActionPerformed(evt);
            }
        });
        mTools.add(miSettings);

        menuBar.add(mTools);

        setJMenuBar(menuBar);

        pack();
    }// </editor-fold>//GEN-END:initComponents

	/**
	 * Handler when clicked menu>file>exit.
	 * Closes the form.
	 * @param evt unused
	 */
    private void miExitActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miExitActionPerformed
		dispatchEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
    }//GEN-LAST:event_miExitActionPerformed

	/**
	 * Handler when clicked menu>connection>connect.
	 * Brings up connection menu.
	 * @param evt unused
	 */
    private void miConnectActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miConnectActionPerformed
        // bring up connection menu
		connectFrame.setVisible(true);
    }//GEN-LAST:event_miConnectActionPerformed

	/**
	 * Handler when clicked menu>connection>disconnect.
	 * Disconnects from server.
	 * @param evt unused
	 */
    private void miDisconnectActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miDisconnectActionPerformed
        // close connection
		disconnect();
    }//GEN-LAST:event_miDisconnectActionPerformed

	/**
	 * Called when frame is closing.
	 * Disconnects, frees resources, saves settings.
	 */
    private void formWindowClosing(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosing
		// kill connection when windows is closed
		disconnect();
		
		// save last settings
		SaveSettings settings = settingsFrame.getValues();
		settings.address = address;
		settings.port = port;
		settings.pinSteering = pinSteering;
		settings.pinThrottle = pinThrottle;
				
		try {
			FileOutputStream file = new FileOutputStream("settings.dat");
			ObjectOutputStream writer = new ObjectOutputStream(file);
			writer.writeObject(settings);
		}
		catch (Exception e) {
			/* don't care */
		}
    }//GEN-LAST:event_formWindowClosing

	/**
	 * Reads steering state from the text field.
	 * @param evt 
	 */
    private void tfSteeringActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_tfSteeringActionPerformed
        // parse steering from text edit and set values
		String text = tfSteering.getText();
		int idx = text.indexOf('%');
		float steering;
		if (idx > 0) {
			text = text.substring(0, idx);
		}
		try {
			steering = Float.parseFloat(text);
			steering = Math.max(0.0f, Math.min(100.0f, steering));
			tfSteering.setText(Float.toString(steering) + "%");
			sliderSteering.setValue((int)(steering));
		}
		catch (NumberFormatException e ) {
			// TODO: recover somehow...
		}
    }//GEN-LAST:event_tfSteeringActionPerformed

	/**
	 * Reads throttle state from the text field.
	 * @param evt 
	 */
    private void tfThrottleActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_tfThrottleActionPerformed
		// parse throttle from text edit and set values
		String text = tfThrottle.getText();
		int idx = text.indexOf('%');
		float steering;
		if (idx > 0) {
			text = text.substring(0, idx);
		}
		try {
			steering = Float.parseFloat(text);
			steering = Math.max(0.0f, Math.min(100.0f, steering));
			tfThrottle.setText(Float.toString(steering) + "%");
			sliderThrottle.setValue((int)(steering));
		}
		catch (NumberFormatException e ) {
			// TODO: recover somehow...
		}
    }//GEN-LAST:event_tfThrottleActionPerformed

	/**
	 * Reads throttle state from the slider.
	 * @param evt 
	 */
    private void sliderThrottleStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_sliderThrottleStateChanged
		int value = sliderThrottle.getValue();
		
		// apply epa
		if (value < epaThrottleMin) {
			value = (int)(Math.ceil(epaThrottleMin) + 0.1);
			sliderThrottle.setValue(value);
		}
		else if (value > epaThrottleMax) {
			value = (int)epaThrottleMax;
			sliderThrottle.setValue(value);
		}
		
		// set throttle on text field
		tfThrottle.setText(value + "%");
		
		// set physical throttle
		setThrottle(value / 100.0f);
    }//GEN-LAST:event_sliderThrottleStateChanged

	/**
	 * Reads steering state from the slider.
	 * @param evt 
	 */
    private void sliderSteeringStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_sliderSteeringStateChanged
		int value = sliderSteering.getValue();
		
		// apply epa
		if (value < epaSteeringMin) {
			value = (int)(Math.ceil(epaSteeringMin) + 0.1);
			sliderSteering.setValue(value);
		}
		else if (value > epaSteeringMax) {
			value = (int)epaSteeringMax;
			sliderSteering.setValue(value);
		}
		
		// set steering on text field
		tfSteering.setText(value + "%");
		
		// set physical steering
		setSteering(value / 100.0f);
    }//GEN-LAST:event_sliderSteeringStateChanged

	/**
	 * Called when clicking menu>tools>settings.
	 * Brings up settings menu.
	 * @param evt 
	 */
    private void miSettingsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miSettingsActionPerformed
        // show settings dialogue
		settingsFrame.setVisible(true);
    }//GEN-LAST:event_miSettingsActionPerformed

	/**
	 * @param args the command line arguments
	 */
	public static void main(String args[]) {
		/* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
		 * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
		 */
		try {
			for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
				if ("Nimbus".equals(info.getName())) {
					javax.swing.UIManager.setLookAndFeel(info.getClassName());
					break;
				}
			}
		} catch (ClassNotFoundException ex) {
			java.util.logging.Logger.getLogger(RcController.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
		} catch (InstantiationException ex) {
			java.util.logging.Logger.getLogger(RcController.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
		} catch (IllegalAccessException ex) {
			java.util.logging.Logger.getLogger(RcController.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
		} catch (javax.swing.UnsupportedLookAndFeelException ex) {
			java.util.logging.Logger.getLogger(RcController.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
		}
        //</editor-fold>

		/* Create and display the form */
		java.awt.EventQueue.invokeLater(new Runnable() {
			public void run() {
				new RcController().setVisible(true);
			}
		});
	}

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JMenuItem jMenuItem4;
    private javax.swing.JMenu mConnect;
    private javax.swing.JMenu mFile;
    private javax.swing.JMenu mTools;
    private javax.swing.JPanel mainPanel;
    private javax.swing.JMenuBar menuBar;
    private javax.swing.JMenuItem miConnect;
    private javax.swing.JMenuItem miDisconnect;
    private javax.swing.JMenuItem miExit;
    private javax.swing.JMenuItem miSettings;
    private javax.swing.JSlider sliderSteering;
    private javax.swing.JSlider sliderThrottle;
    private javax.swing.JTextField tfSteering;
    private javax.swing.JTextField tfThrottle;
    // End of variables declaration//GEN-END:variables
}



/**
 * A helper class for saving settings.
 */
class SaveSettings implements Serializable {
	public String address;
	public int port;
	public int pinThrottle;
	public int pinSteering;
	
	public float epaThrottleMin;
	public float epaThrottleMax;
	public float epaSteeringMin;
	public float epaSteeringMax;
	
	public float timingThrottleMin;
	public float timingThrottleMax;
	public float timingSteeringMin;
	public float timingSteeringMax;
}
