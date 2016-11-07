package net.noobwerkz.sampleapp;

import net.noobwerkz.sampleapp.JNILib;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
// import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class GameView extends GLSurfaceView
{
	private static class Renderer implements GLSurfaceView.Renderer 
	{
		public void onDrawFrame(GL10 gl) 
		{
			// JNILib.Log("(Java) onDrawFrame");
			JNILib.OnFrame();
		}

		public void onSurfaceChanged(GL10 gl, int iWidth, int iHeight) 
		{
			JNILib.Log("(Java) GameView - onSurfaceChanged");
			JNILib.OnResize(iWidth, iHeight);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) 
		{
			JNILib.Log("(Java) GameView - onSurfaceCreated");
		}
	}

	public GameView(Context context) 
	{
		super(context);

		setEGLConfigChooser(8, 8, 8, 8, 16, 0);
		setEGLContextClientVersion(3);
		// setPreserveEGLContextOnPause(true);
		setRenderer(new Renderer());

		setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
		JNILib.OnInit();
	}


	public void onDestroy()
	{	
		// Send the event to the renderer thread
		queueEvent( new Runnable() {
			@Override
			public void run() 
		{
			JNILib.OnShutdown();
		}
		});
	}

	public void onResume()
	{
		super.onResume();

		// Send the event to the renderer thread
		queueEvent( new Runnable() {
			@Override
			public void run() 
		{
			JNILib.OnResume();
		}
		});
	}

	public void onPause()
	{
		// Send the event to the renderer thread
		queueEvent( new Runnable() {
			@Override
			public void run() 
		{
			JNILib.OnPause();
		}
		});

	}

	@Override
	public boolean onTouchEvent(MotionEvent event) 
	{
		// Implement Runnable for MotionEvent parameter
		class MotionEventRunnable implements Runnable
		{
			private MotionEvent mEvent;
			public MotionEventRunnable( MotionEvent event ) { mEvent = event; }

			@Override
			public void run() 
			{
				// Get the number of pointers to iterate
				int iNumPointers = mEvent.getPointerCount();
				for ( int i = 0; i < iNumPointers; ++i )
				{
					try
					{
						// Get the pointer ID and index
						int iPointerID = mEvent.getPointerId( i );
						int iPointerIndex = mEvent.findPointerIndex( iPointerID );

						// Get the xy position and action
						float x = mEvent.getX( iPointerIndex );
						float y = mEvent.getY( iPointerIndex );

						int iAction = mEvent.getActionMasked();

						// Send to C++
						JNILib.OnTouch( iPointerID, x, y, iAction );
					}
					catch(Exception e)
					{
						JNILib.Log("(Java) GameView - Caught nasty expression in method onTouchEvent");
					} // This was necessary, as using too many fingers killed the program.
				}				
			}
		}

		// Send the event to the renderer thread
		queueEvent(new MotionEventRunnable(event));

		return true;
	}
}

