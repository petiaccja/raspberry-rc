////////////////////////////////////////////////////////////////////////////////
//	File: Display.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This public class is the core of the GUI used for steering and gas in 2D
//	vehicles.
////////////////////////////////////////////////////////////////////////////////

package rccontroller.android;

import android.opengl.GLES20;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLSurfaceView;
import android.content.Context;
import android.graphics.Matrix;

import java.nio.FloatBuffer;
import java.nio.ByteBuffer; 
import java.nio.ByteOrder;



////////////////////////////////////////////////////////////////////////////////
//	The surface class manages the renderer's state.
//	Extend it to provide additional functionality such as touch events.
public class GuiCore extends GLSurfaceView {
	public GuiCore(Context context){
        super(context);

		setEGLContextClientVersion(2);
		super.setEGLConfigChooser(8,8,8,8,16,0);
		//setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);		
		//setPreserveEGLContextOnPause(true);

		renderer = new RCRenderer();
        setRenderer(renderer);
	}
	// keep a quick reference to the renderer
	protected RCRenderer renderer;
}

////////////////////////////////////////////////////////////////////////////////
//	The renderer class does all the job of drawing the GUI.
class RCRenderer implements GLSurfaceView.Renderer {
	////////////////////////////////////
	// Redraw frame
	private Matrix matRotation,
				   matScaling,
				   matTranslation,
				   matViewProj,
				   matTransform;
	
	RCRenderer() {
		// allocate matrices
		matRotation = new Matrix();
		matScaling = new Matrix();
		matTranslation = new Matrix();
		matViewProj = new Matrix();
		matTransform = new Matrix();
		
		steererPos = new Point2D(-0.5f, 0.f);
		steererScale = new Point2D(0.4f, 0.2f);
		throttlePos = new Point2D(0.5f, 0.f);
		throttleScale = new Point2D(0.2f, 0.8f);
		throttleClamp = 0.5f;
		steererClamp = new Point2D(0.5f, 0.5f);	
	}	
	
	private void setMatrices(Point2D position, float rotation, Point2D scale, boolean enableViewMatrix) {
		matTranslation.setTranslate(position.x, position.y);
		matRotation.setRotate(rotation);
		matScaling.setScale(scale.x, scale.y);
				
		matTransform.reset();
		matTransform.postConcat(matScaling);
		matTransform.postConcat(matRotation);
		matTransform.postConcat(matTranslation);
		
		if (enableViewMatrix) {
			matViewProj.setScale(1.f, (float)screenWidth/(float)screenHeight);
			matTransform.postConcat(matViewProj);
		}
	}
	public void update() {
		for (int i=0; i<3; i++) {
			// set actual matrices
			float[] clamping = {-1.f, 1.f, -1.f, 1.f};
			switch (i) {
				case 0: // background
					setMatrices(new Point2D(0.f, 0.f), 0.f, new Point2D(1.f, 1.f), false);
					break;
				case 1: // steering
					setMatrices(steererPos, 0.f, steererScale, false);
					clamping[0] = -steererClamp.x;
					clamping[1] = steererClamp.y;
					break;
				case 2: // throttle
					setMatrices(throttlePos, 0.f, throttleScale, false);
					clamping[2] = -1.f;
					clamping[3] = throttleClamp*2.f-1.f;
					break;
			}
			
			// set the transformation matrix in the shader
			int matTransformHandle = GLES20.glGetUniformLocation(shaderProgram, "matTransform");
			float[] matElements = new float[9];
			matTransform.getValues(matElements);			
			GLES20.glUniformMatrix3fv(matTransformHandle, 1, false, matElements, 0);
			
			// set clamping in shader
			int clampingHandle = GLES20.glGetUniformLocation(shaderProgram, "clamping");
			GLES20.glUniform4f(clampingHandle, clamping[0], clamping[1], clamping[2], clamping[3]);			

			// set vertex buffer
			int vPosHandle = GLES20.glGetAttribLocation(shaderProgram, "vPosition");
			GLES20.glEnableVertexAttribArray(vPosHandle);
			GLES20.glVertexAttribPointer(vPosHandle, 2, GLES20.GL_FLOAT, false, 0, vbQuad);
			// draw stuff
			GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 6);
			// disable vertex buffer
			GLES20.glDisableVertexAttribArray(vPosHandle);
		}
	}	
	
	
	
	////////////////////////////////////
	// Shaders
	private final String vertexShaderSrc = 
		"attribute vec2 vPosition;" +
		"uniform mat3 matTransform;" +
		"varying vec2 pos2D;" +
		"void main() {" +
		"	gl_Position = vec4(vec3(vPosition,1.)*matTransform, 1.);" +
		"	gl_Position.z = 0.5;" +
		"	pos2D = vPosition.xy;" +
		"}";
	private final String pixelShaderSrc = 
		"precision highp float;" +
		"varying vec2 pos2D;" +
		"uniform vec4 clamping;" + // left-right, bottom-top
		"void main() {" +
		"	if (pos2D.x<clamping.x || pos2D.x>clamping.y || pos2D.y<clamping.z || pos2D.y>clamping.w)" + 
		"		gl_FragColor = clamping;" +
		"	else" +
		"		gl_FragColor = vec4(0.5*(pos2D.x+1.), 0.5*(pos2D.y+1.), 0.25*(-pos2D.x-pos2D.y+2.), 1.);" +
		"}";
	private int vertexShader;
	private int pixelShader;
	private int shaderProgram;
	
	////////////////////////////////////
	// Scene state
	private int screenWidth, screenHeight;
	private Point2D steererPos, steererScale;
	private Point2D throttlePos, throttleScale;
	private float throttleClamp;	// [0.f, 1.f]
	private Point2D steererClamp;	// x-left [0.f; 1.f], y-right [0.f; 1.f]
	public void setSteererPos(Point2D pos) {
		steererPos = pos;
	}
	public void setThrottlePos(Point2D pos) {
		throttlePos = pos;
	}
	public void setSteererScale(Point2D scale) {
		steererScale = scale;
	}
	public void setThrottleScale(Point2D scale) {
		throttleScale = scale;
	}
	public void setSteererClamp(float left, float right) {
		steererClamp.x = Math.max(0.f, Math.min(1.f, left));
		steererClamp.y = Math.max(0.f, Math.min(1.f, right));
	}
	public void setThrottleClamp(float max) {
		throttleClamp = Math.max(0.f, Math.min(1.f, max));
	}
	public Point2D getThrottlePos() {return new Point2D(throttlePos);}
	public Point2D getSteererPos() {return new Point2D(steererPos);}
	public Point2D getThrottleScale() {return new Point2D(throttleScale);}
	public Point2D getSteererScale() {return new Point2D(steererScale);}
	public float getThrottleClamp() {return throttleClamp;}
	public Point2D getSteererClamp() {return new Point2D(steererClamp);}
	
	////////////////////////////////////
	// Vertiex buffers	
	// full screen quad
	private static final float[] quadVertices = {
		// lower tri
		-1.f, 1.f,
		-1.f, -1.f,
		1.f, -1.f,
		// upper tri
		-1.f, 1.f,
		1.f, -1.f,
		1.f, 1.f,
	};
	private FloatBuffer vbQuad;
	


	//////////////////////////////////
	// Called on creation (like ctor)
	@Override
	public void onSurfaceCreated(GL10 unused, EGLConfig config) {
		GLES20.glClearColor(.15f, .15f, .15f, 1.f);

		// create and compile vertex shader
		vertexShader = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
		GLES20.glShaderSource(vertexShader, vertexShaderSrc);
		GLES20.glCompileShader(vertexShader);
		
		// create and compile fragment shader
		pixelShader = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
		GLES20.glShaderSource(pixelShader, pixelShaderSrc);
		GLES20.glCompileShader(pixelShader);
		
		// create shader program
		shaderProgram = GLES20.glCreateProgram();
		GLES20.glAttachShader(shaderProgram, vertexShader);
		GLES20.glAttachShader(shaderProgram, pixelShader);
		GLES20.glLinkProgram(shaderProgram);
		
		GLES20.glUseProgram(shaderProgram);
		
		// create vertex buffer
		ByteBuffer bb = ByteBuffer.allocateDirect(quadVertices.length*4);
		bb.order(ByteOrder.nativeOrder());
		vbQuad = bb.asFloatBuffer();
		vbQuad.put(quadVertices);
		vbQuad.position(0);
		
		int[] p = new int[3];
		GLES20.glGetShaderiv(vertexShader, GLES20.GL_COMPILE_STATUS, p, 0);
		GLES20.glGetShaderiv(pixelShader, GLES20.GL_COMPILE_STATUS, p, 1);
		
		String infoVS = GLES20.glGetShaderInfoLog(vertexShader);
		String infoPS = GLES20.glGetShaderInfoLog(pixelShader);
				
		GLES20.glCullFace(GLES20.GL_CCW);
	}
	////////////////////////////////////
	// Called on frame redraw
	@Override
	public void onDrawFrame(GL10 unused) {
		// clear surface
		GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
		GLES20.glClearColor(0.2f, 0.2f, 0.7f, 1.0f);
		GLES20.glClearDepthf(1.0f);
		// draw
		update();
	}
	////////////////////////////////////
	// Called when the surface layout is changed
	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height) {
		GLES20.glViewport(0, 0, width, height);
		
		this.screenWidth = width;
		this.screenHeight = height;
	}	
}