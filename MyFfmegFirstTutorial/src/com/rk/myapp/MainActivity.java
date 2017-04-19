package com.rk.myapp;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    }

    static
    {
        System.loadLibrary("FFMpegJni");
    }

    private native void nativeInit();
    private native void nativeSetSurface();
    private native void nativeSetUri(String uri);
    private native void nativeGetFrameBitmap(int frame);
    private native void nativePrepare();
    private native void nativeStart();
    private native void nativePause();
    private native void nativePlay();
    private native void nativeSeek(int position);
}
