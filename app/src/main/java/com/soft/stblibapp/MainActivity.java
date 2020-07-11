package com.soft.stblibapp;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.soft.common.BaseActivity;

public class MainActivity extends BaseActivity implements View.OnClickListener {

    public static final String TAG = "MainActivity";

    private Button mCameraPreview;
    private Button mTextureDisplay;
    private Button mFilterDisplay;
    private Button mOpenglAdvanced;
    private Button mFrameBuffer;
    private Button mWaterMark;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mCameraPreview = findViewById(R.id.camera_preview);
        mTextureDisplay = findViewById(R.id.texture_display);
        mFilterDisplay = findViewById(R.id.filter_display);
        mOpenglAdvanced = findViewById(R.id.opengl_advanced);
        mFrameBuffer = findViewById(R.id.frame_buffer);
        mWaterMark = findViewById(R.id.water_mark);

        mCameraPreview.setOnClickListener(this);
        mTextureDisplay.setOnClickListener(this);
        mFilterDisplay.setOnClickListener(this);
        mOpenglAdvanced.setOnClickListener(this);
        mFrameBuffer.setOnClickListener(this);
        mWaterMark.setOnClickListener(this);
    }

    @Override
    protected void onRestart() {
        super.onRestart();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop: ");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy: ");
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.camera_preview:
                transToCameraPreview();
                break;
            case R.id.texture_display:
                transToTextureDisplay();
                break;
            case R.id.filter_display:
                transToFilterDisplay();
                break;
            case R.id.opengl_advanced:
                transToOpenglAdvancedActivity();
                break;
            case R.id.frame_buffer:
                transToFrameBufferDisplay();
                break;
            case R.id.water_mark:
                transToWaterMarkDisplay();
                break;
            default:
                break;
        }
    }

    private void transToOpenglAdvancedActivity() {
        Log.d(TAG, "transToOpenglAdvancedActivity: ");
        Intent intent = new Intent();
        intent.setClass(MainActivity.this, OpenglAdvancedActivity.class);
        startActivity(intent);
    }

    private void transToCameraPreview() {
        Log.d(TAG, "transToCameraPreview: ");
        Intent intent = new Intent();
        intent.setClass(MainActivity.this, CameraPreviewActivity.class);
        startActivity(intent);
    }

    private void transToTextureDisplay() {
        Log.d(TAG, "transToTextureDisplay: ");
        Intent intent = new Intent();
        intent.setClass(MainActivity.this, TextureDisplayActivity.class);
        startActivity(intent);
    }

    private void transToFilterDisplay() {
        Log.d(TAG, "transToFilterDisplay: ");
        Intent intent = new Intent();
        intent.setClass(MainActivity.this, FilterActivity.class);
        startActivity(intent);
    }

    private void transToFrameBufferDisplay() {
        Log.d(TAG, "transToFrameBufferDisplay: ");
        Intent intent = new Intent();
        intent.setClass(MainActivity.this, FrameBufferActivity.class);
        startActivity(intent);
    }

    private void transToWaterMarkDisplay() {
        Log.d(TAG, "transToWaterMarkDisplay: ");
        Intent intent = new Intent();
        intent.setClass(MainActivity.this, WaterMarkActivity.class);
        startActivity(intent);
    }
}
