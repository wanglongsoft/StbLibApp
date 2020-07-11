package com.soft.stblibapp;

import android.Manifest;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import androidx.annotation.Nullable;

import com.soft.common.BaseActivity;
import com.soft.common.FunctionControl;
import com.soft.common.GetPathFromUri;

public class WaterMarkActivity extends BaseActivity {

    public static final String TAG = "WaterMarkActivity";
    private static final int IMAGE_FILE_SELECT_CODE = 108;
    private SurfaceView m_surface_view;
    private Button mSelectImage;
    private Button mDisplayImage;
    private String mImagePath;
    private String mWaterImagePath = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_water_mark);

        requestRunTimePermission(new String[]{
                        Manifest.permission.CAMERA,
                        Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE},
                null);
        initSurfaceView();
        initCommon();
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
                FunctionControl.getInstance().releaseResources();
            }
        });
    }

    private void initCommon() {
        mSelectImage = findViewById(R.id.select_image);
        mSelectImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: ");
                showFileChooser(IMAGE_FILE_SELECT_CODE);
            }
        });

        mDisplayImage = findViewById(R.id.display_image);
        mDisplayImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: ");
                if(null != mImagePath && !mImagePath.isEmpty()) {
                    FunctionControl.getInstance().openglWaterMarkDisplay(mImagePath, mWaterImagePath);  // VBO
                }
            }
        });
    }

    private void showFileChooser(int selectcode) {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        startActivityForResult(Intent.createChooser(intent, "Select a File to Open"), selectcode);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case IMAGE_FILE_SELECT_CODE:
                if (resultCode == RESULT_OK) {
                    // Get the Uri of the selected file
                    Uri uri = data.getData();
                    mImagePath = GetPathFromUri.getPath(this, uri);
                    Log.d(TAG, "onActivityResult mImagePath: " + mImagePath);
                }
                break;
            default:
                break;
        }
    }

}