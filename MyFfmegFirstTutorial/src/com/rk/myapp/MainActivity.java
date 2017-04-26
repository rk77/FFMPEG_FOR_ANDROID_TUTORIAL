package com.rk.myapp;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.Bitmap;
import android.util.Log;
import android.widget.ImageView;

import com.rk.myapp.R;

public class MainActivity extends Activity
{
    private static final String TAG = "FFMpeg_JNI";

    private ImageView mImageView;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mImageView = (ImageView) findViewById(R.id.image_frame);
    }

    public void onStart() {
        super.onStart();
        nativeInit();
        /*
         20151010_132114.mp4
         2a8s.mkv
         db.ts
         shiziwang.rmvb
         */
        nativeSetUri("/mnt/usb/AD0C-15F0/Test/video/db.ts");
        nativePrepare();
        nativeStart();
        Log.i(TAG, "onstart end");
    }

    public void onResume() {
        super.onResume();
        Bitmap b = nativeGetFrameBitmap();
        mImageView.setImageBitmap(b);
        Log.i(TAG, "onResume(), bitmap is " + b);
        Log.i(TAG, "onResume(), w = " + b.getWidth() + ", h = " + b.getHeight());
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
