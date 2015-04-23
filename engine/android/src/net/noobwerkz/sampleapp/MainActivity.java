package net.noobwerkz.sampleapp;

import net.noobwerkz.sampleapp.GameView;
import android.view.Surface;
import android.os.Bundle;
import android.app.Activity;
import android.content.res.AssetManager;
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
		mView = new GameView(getApplication());
		setContentView(mView);
		copyAssets();
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
		String[] files = null;
		try
		{
			files = assetManager.list("");
			String allFiles = new String("");
			for (String s: files)
			{
				allFiles += s;
				allFiles += " ";
			}
			JNILib.Log("MainActivity: Files found in APK: " + allFiles);
			String files_path = getExternalFilesDir(new String()).getPath();
			JNILib.SetupArchiveDir(files_path);
		}
		catch (IOException e)
		{
			Log.e("tag", "Failed to get asset file list.", e);
			JNILib.Log("MainActivity: Failed to get asset file list");
		}
		for(String filename : files)
		{
			InputStream in = null;
			OutputStream out = null;
			try
			{
				in = assetManager.open(filename);
				File outFile = new File(getExternalFilesDir(null), filename);
				JNILib.Log("MainActivity: Copying " + outFile.getPath());
				out = new FileOutputStream(outFile);
				copyFile(in, out);
			}
			catch(IOException e)
			{
				Log.e("tag", "Failed to copy asset file: " + filename, e);
				JNILib.Log("MainActivity: Failed to copy asset file " + filename);
			}     
			finally
			{
				if (in != null)
				{
					try
					{
						in.close();
					}
					catch (IOException e)
					{
						// NOOP
					}
				}
				if (out != null)
				{
					try
					{
						out.close();
					}
					catch (IOException e)
					{
						// NOOP
					}
				}
			}  
		}
	}

	private void copyFile(InputStream in, OutputStream out) throws IOException {
		byte[] buffer = new byte[1024];
		int read;
		while((read = in.read(buffer)) != -1){
			out.write(buffer, 0, read);
		}
	}
	

}
