package com.soft.stblibapp;

import android.Manifest;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import com.soft.common.BaseActivity;
import com.soft.common.CameraHelper;
import com.soft.common.FunctionControl;

public class CameraPreviewActivity extends BaseActivity {

    public static final String TAG = "CameraPreviewActivity";
    private SurfaceView m_surface_view;
    private CameraHelper myCamera;
    private Button mStartPreview;
    private Button mStopPreview;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_preview);
        requestRunTimePermission(new String[]{
                        Manifest.permission.CAMERA,
                        Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE},
                null);
        cameraPreview();
    }

    private void cameraPreview() {
        myCamera = new CameraHelper();
        myCamera.setPictureRawCaptureListener(new CameraHelper.OnPictureRawCapture() {
            @Override
            public void onCapture(byte[] data, int width, int height, int video_rotation) {
                Log.d(TAG, "onCapture length: " + data.length);
                FunctionControl.getInstance().rendSurface(data, width, height, video_rotation);
            }
        });
        initSurfaceView();
        initCommon();
    }

    private void initCommon() {
        mStartPreview = findViewById(R.id.start_preview);
        mStartPreview.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: ");
                myCamera.startPreview();
            }
        });

        mStopPreview = findViewById(R.id.stop_preview);
        mStopPreview.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: ");
                myCamera.stopPreview();
            }
        });
    }

    private void initSurfaceView() {
        m_surface_view = findViewById(R.id.surface_view);
        m_surface_view.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Log.d(TAG, "surfaceCreated: ");
                FunctionControl.getInstance().saveAssetManager(getAssets());
                FunctionControl.getInstance().setSurface(holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.d(TAG, "surfaceChanged: ");
                FunctionControl.getInstance().setSurfaceSize(width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.d(TAG, "surfaceDestroyed: ");
                myCamera.stopPreview();
                FunctionControl.getInstance().releaseResources();
            }
        });
    }
}
