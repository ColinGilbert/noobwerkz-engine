package net.noobwerkz.sampleapp;//.NoobwerkzEngine;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.widget.Toast;
import android.content.res.AssetManager;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.util.Log;
import java.io.*;
import java.util.Arrays;

public class NoobwerkzEngine extends Activity implements SurfaceHolder.Callback
{

	private static String TAG = "NoobwerkzJava";

	@Override
		public void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);

			Log.i(TAG, "onCreate()");

			setContentView(R.layout.main);
			SurfaceView surfaceView = (SurfaceView)findViewById(R.id.surfaceview);
			surfaceView.getHolder().addCallback(this);

			//surfaceView.setOnClickListener(new OnClickListener() {
			//		public void onClick(View view) {
			//		Toast toast = Toast.makeText(NoobwerkzEngine.this, "Welcome to Noobwerkz Engine, Android edition.", Toast.LENGTH_LONG);
			//		toast.show();
			//		}});

		}

	@Override
		protected void onStart()
		{
			super.onStart();
			Log.i(TAG, "onStart()");
			nativeOnStart();
			copyAssets();			
		}

	@Override
		protected void onResume()
		{
			super.onResume();
			Log.i(TAG, "onResume()");
			nativeOnResume();
		}

	@Override
		protected void onPause()
		{
			super.onPause();
			Log.i(TAG, "onPause()");
			nativeOnPause();
		}

	@Override
		protected void onStop()
		{
			super.onStop();
			Log.i(TAG, "onStop()");
			nativeOnStop();
		}

	private void copyAssets()
	{
		Log.i(TAG, "Copying assets.");
		AssetManager assetManager = getAssets();
		String toPath = getExternalFilesDir(new String()).getPath();
		nativeSetArchiveDir(toPath);
		copyAssetFolder(assetManager, "sounds", toPath + "/sounds");
		copyAssetFolder(assetManager, "font", toPath + "/font");
	}

	private static boolean copyAssetFolder(AssetManager assetManager, String fromAssetPath, String toPath)
	{
		try
		{
			String[] files = assetManager.list(fromAssetPath);
			new File(toPath).mkdirs();
			boolean res = true;
			for (String file : files)
			{
				String ss = new String("(Java) MainActivity - Copying " + file + " to " + toPath + "/" + file);
				Log.i(TAG, ss);
				if (file.contains("."))
					res &= copyAsset(assetManager, fromAssetPath + "/" + file, toPath + "/" + file);
				else 
					res &= copyAssetFolder(assetManager, fromAssetPath + "/" + file, toPath + "/" + file);
			}
			return res;
		}
		catch (Exception e)
		{
			// JNILib.Log("MainActivity - Could not copy: " + e.toString());
			e.printStackTrace();
			return false;
		}
	}

	private static boolean copyAsset(AssetManager assetManager, String fromAssetPath, String toPath)
	{
		InputStream in = null;
		OutputStream out = null;
		try {
			in = assetManager.open(fromAssetPath);
			new File(toPath).createNewFile();
			out = new FileOutputStream(toPath);
			copyFile(in, out);
			in.close();
			in = null;
			out.flush();
			out.close();
			out = null;
			return true;
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return false;
		}
	}

	private static void copyFile(InputStream in, OutputStream out) throws IOException
	{
		byte[] buffer = new byte[1024];
		int read;
		while((read = in.read(buffer)) != -1)
		{
			out.write(buffer, 0, read);
		}
	}



	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		nativeSetSurface(holder.getSurface());
	}

	public void surfaceCreated(SurfaceHolder holder) {
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		nativeSetSurface(null);
	}




	public static native void nativeOnStart();
	public static native void nativeOnResume();
	public static native void nativeOnPause();
	public static native void nativeOnStop();
	public static native void nativeSetSurface(Surface surface);
	public static native void nativeSetArchiveDir(String name);

	static {
		System.loadLibrary("Entry");
	}

}
