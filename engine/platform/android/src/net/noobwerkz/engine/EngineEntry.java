//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

package net.noobwerkz.engine;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.util.Log;
import android.content.res.AssetManager; 

public class EngineEntry extends Activity implements SurfaceHolder.Callback
{
    private static String TAG = "Engine";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG, "onCreate()");
        
        setContentView(R.layout.main);
        SurfaceView surfaceView = (SurfaceView)findViewById(R.id.surfaceview);
        surfaceView.getHolder().addCallback(this);

        surfaceView.setOnClickListener(new OnClickListener() {
                public void onClick(View view) {
                    Toast toast = Toast.makeText(EngineEntry.this, "This demo combines Java UI and native EGL + OpenGL renderer", Toast.LENGTH_LONG);
                    toast.show();
                }});
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.i(TAG, "onStart()");
	nativeSetAssets(getAssets());
	
        nativeOnStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i(TAG, "onResume()");
        nativeOnResume();
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        Log.i(TAG, "onPause()");
        nativeOnPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.i(TAG, "onStop()");
        nativeOnStop();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h)
    {
        nativeSetSurface(holder.getSurface());
    }

    public void surfaceCreated(SurfaceHolder holder)
    {
    }

    public void surfaceDestroyed(SurfaceHolder holder)
    {
        nativeSetSurface(null);
    }


    public static native void nativeOnStart();
    public static native void nativeOnResume();
    public static native void nativeOnPause();
    public static native void nativeOnStop();
    public static native void nativeSetSurface(Surface surface);
    public static native void nativeSetAssets(AssetManager assets);

    static
    {
        System.loadLibrary("Entry");
    }

}
