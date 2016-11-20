package net.noobwerkz.sampleapp;

import net.noobwerkz.sampleapp.GameView;
import android.view.WindowManager;
import android.view.Surface;
import android.media.AudioManager;
import android.os.Bundle;
import android.app.Activity;
import android.content.res.AssetManager;
import android.content.Context;
import android.os.Environment;
import android.util.Log;
import java.io.*;
import java.util.Arrays;

public class MainActivity extends Activity
{
	GameView mView;
	@Override protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);

		int sampleRate = 0;
		int bufSize = 0;

		// Retrieve fast audio path sample rate and buf size; if we have it, we pass to native side to create a player with fast audio enabled [ fast audio == low latency audio ];
		// If we do not have a fast audio path, we pass 0 for sampleRate, which will force native side to pick up the 8Khz sample rate.

		AudioManager myAudioMgr = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
		String nativeParam = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
		sampleRate = Integer.parseInt(nativeParam);
		nativeParam = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
		bufSize = Integer.parseInt(nativeParam);

		JNILib.CreateBufferQueueAudioPlayer(sampleRate, bufSize);

		mView = new GameView(getApplication());
		// getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON | WindowManager.LayoutParams.FLAG_FULLSCREEN);
		copyAssets();

		setContentView(mView);
	}

	@Override protected void onPause() 
	{
		super.onPause();
		mView.onPause();
	}

	@Override protected void onResume() 
	{
		super.onResume();
		mView.onResume();
	}

	@Override protected void onDestroy() 
	{
		super.onDestroy();
		mView.onDestroy();
	}

	private void copyAssets()
	{
		AssetManager assetManager = getAssets();
		String toPath = getExternalFilesDir(new String()).getPath();
		JNILib.SetupArchiveDir(toPath);
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
				JNILib.Log(ss);
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
}

