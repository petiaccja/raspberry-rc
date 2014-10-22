////////////////////////////////////////////////////////////////////////////////
//	File: ControlPanel.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Processes user input, draws the interface with GuiCore and sends messages
//	on network.
////////////////////////////////////////////////////////////////////////////////

package rccontroller.android;

import android.content.Context;
import android.view.MotionEvent;
import android.app.Activity;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;



public class ControlPanel extends GuiCore implements SensorEventListener {
	private Activity activity;
	private Messenger messenger;
	private boolean useTouchSteering;
	private float throttle;
	private float steering;

	public final float maxAngle = 60.0f;
	public final float zeroAngle = 5.0f;
	
	// constructor
	public ControlPanel(Context context, Messenger messenger, boolean useTouchSteering) {
		super(context);
		activity = (Activity)context;
		
		// start network
		this.messenger = messenger;
		try {
			messenger.start();
		}
		catch (IllegalStateException e) {
			System.out.println(e.getMessage());
		}
	}
	
	// touch event handler
	@Override
	public boolean onTouchEvent(MotionEvent e) {
		Point2D touchPos = new Point2D();
		touchPos.x = 2.f*((float)e.getX() / (float)getWidth()) - 1.f;
		touchPos.y = 1.f - ((float)e.getY() / (float)getHeight())*2.f;
		switch (e.getAction()) {
			case MotionEvent.ACTION_DOWN: {
				// handle exit button
				if (touchPos.x<-0.8 && touchPos.y>0.8) {
					messenger.close();
					activity.finish();
					break;
				}
			}
			case MotionEvent.ACTION_MOVE: {
				// calculate if throttle or steering bar is touched
				float border = (renderer.getThrottlePos().x-renderer.getThrottleScale().x+
							   renderer.getSteererPos().x+renderer.getSteererScale().x) * 0.5f;
				// throttle
				if (touchPos.x>border) {
					// drawing
					float min = renderer.getThrottlePos().y - renderer.getThrottleScale().y;
					float max = renderer.getThrottlePos().y + renderer.getThrottleScale().y;
					throttle = (touchPos.y-min)/(max-min);
					renderer.setThrottleClamp(throttle);
					// network					
					messenger.setThrottle(Math.max(0.5f, Math.min(throttle*0.5f+0.5f, 1.f))); // remap to [0,1] interval
				}
				// steering
				else if (touchPos.x<=border && useTouchSteering) {
					// drawing
					float min = renderer.getSteererPos().x - renderer.getSteererScale().x;
					float max = renderer.getSteererPos().x + renderer.getSteererScale().x;
					steering = 2.f*(touchPos.x-min)/(max-min)-1.f;
					renderer.setSteererClamp(-steering, steering);
					// network
					messenger.setSteering(Math.max(0.0f, Math.min((steering+1.0f)*0.5f, 1.f))); // remap to [0,1] interval
				}
				requestRender();
				break;
			}
			case MotionEvent.ACTION_UP: {
				// calculate if throttle or steering bar is touched
				float border = (renderer.getThrottlePos().x-renderer.getThrottleScale().x+
							   renderer.getSteererPos().x+renderer.getSteererScale().x) * 0.5f;
				// throttle
				if (touchPos.x>border) {
					renderer.setThrottleClamp(0.0f);
					messenger.setThrottle(0.5f);
				}
					
				requestRender();
				break;
			}
		}
		return true;
	}
		
	// sensor event handler
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}
	
	@Override
	public void onSensorChanged(SensorEvent event) {
		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
			// extract & validate input
			float x = event.values[0],
					y = event.values[1],
					z = event.values[2];
					
			// calculate angle
			float len = (float)Math.sqrt(x*x + y*y);
			float xn = x/len,
					yn = y/len,
					zn = z/len;
			float angle = (float)(180.0f/Math.PI*Math.asin(yn));
			if (Math.sqrt(x*x + y*y) < z*0.5) {
				angle = 0.0f;
			}
			
			// set steering
			// -5 to 5 deg goes to 0.0 steering, so it's easier to go straight
			if (angle<0.0f) {
				angle += zeroAngle;
				angle = Math.min(angle, 0.0f);
			}
			else {
				angle -= zeroAngle;
				angle = Math.max(angle, 0.0f);				
			}
			angle *= maxAngle / (maxAngle-zeroAngle);
			// convert angle to steering
			steering = angle/maxAngle;
			renderer.setSteererClamp(-steering, steering);
			messenger.setSteering(Math.max(0.0f, Math.min((steering+1.0f)*0.5f, 1.f)));
			
			requestRender();
		}
	}	
}