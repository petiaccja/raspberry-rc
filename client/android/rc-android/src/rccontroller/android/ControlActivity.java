////////////////////////////////////////////////////////////////////////////////
//	File: ControlActivity.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This activity is launched by main when successfully connected to a server.
//
///////////////////////////////////////////////////////////////////////////////


package rccontroller.android;

import android.app.Activity;
import android.os.Bundle;
import android.view.MenuItem;
import android.support.v4.app.NavUtils;
import android.annotation.SuppressLint;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;

import android.view.WindowManager;


public class ControlActivity extends Activity {
	private SensorManager sensorManager;
	private Sensor accelerometer;
	boolean useTouchSteering;
	private ControlPanel panel;
	
	
	@SuppressLint("NewApi")
	@Override
	// on creation
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		// enable accelerometer if possible
		sensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
		accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		useTouchSteering = (accelerometer == null);

		// create the control panel gui handler
		panel = new ControlPanel(this, SharedMessenger.messenger, useTouchSteering);
		setContentView(panel);
	}
	
	// some api stuff I don't even understand -> don't mess w/ it
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case android.R.id.home:
            NavUtils.navigateUpFromSameTask(this);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
	
	
	@Override
	protected void onResume() {
		super.onResume();
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		if (!useTouchSteering)
			sensorManager.registerListener(panel, accelerometer, SensorManager.SENSOR_DELAY_FASTEST);
	}

	@Override
	protected void onPause() {
		getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		if (!useTouchSteering)
			sensorManager.unregisterListener(panel);
		super.onPause();
	}
}