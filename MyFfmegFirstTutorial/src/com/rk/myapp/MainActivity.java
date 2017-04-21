package com.rk.myapp;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.Bitmap;
import android.util.Log;

public class MainActivity extends Activity
{
    private static final String TAG = "FFMpeg_JNI";
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    }

    public void onStart() {
        super.onStart();
        nativeInit();
        nativeSetUri("/mnt/usb/AD0C-15F0/Test/video/20151010_132114.mp4");
        nativePrepare();
        nativeStart();
        Log.i(TAG, "onstart end");
    }

    public void onResume() {
        super.onResume();
        Bitmap b = nativeGetFrameBitmap();
        Log.i(TAG, "bitmap is " + b);
    }

    public void onStop() {
        super.onStop();
        nativeDeinit();
    }

    static
    {
        System.loadLibrary("FFMpegJni");
    }

    private native void nativeInit();
    private native void nativeSetSurface();
    private native void nativeSetUri(String uri);
    private native Bitmap nativeGetFrameBitmap();
    private native void nativePrepare();
    private native void nativeStart();
    private native void nativePause();
    private native void nativePlay();
    private native void nativeSeek(int position);
    private native void nativeDeinit();
}
