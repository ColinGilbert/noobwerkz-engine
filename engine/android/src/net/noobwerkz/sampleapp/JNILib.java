package net.noobwerkz.sampleapp;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;


public class JNILib
{
	public static native void OnInit();

	public static native void OnFrame();
	public static native void OnResize(int iWidth, int iHeight);
	public static native void OnShutdown();
	public static native void OnResume();
	public static native void OnPause();
	public static native void OnTouch(int iPointerID, float fPosX, float fPosY, int iAction);
	public static native void SetupArchiveDir(String dir);
	public static native void Log(String message);
	public static native void NativeSetSurface(Surface surface);
	
	static
	{
		System.loadLibrary("Engine");
	}
}
