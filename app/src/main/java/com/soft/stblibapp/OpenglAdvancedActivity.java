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

//OpenGL VBO(顶点缓冲区对象)
//VAO(顶点数组对象)
//MBO(映射缓冲区对象)

//VBO(顶点缓冲区)和 EBO(图元缓冲区，GL_ELEMENT_ARRAY_BUFFER) 的作用是在显存中提前开辟好一块内存，
//用于缓存顶点数据或者图元索引数据，从而避免每次绘制时的
//CPU 与 GPU 之间的内存拷贝，可以改进渲染性能，
//降低内存带宽和功耗。
//VBO（Vertex Buffer Object）是指顶点缓冲区对象，而 EBO（Element Buffer Object）是指图元索引缓冲区对象，
// VBO 和 EBO 实际上是对同一类 Buffer 按照用途的不同称呼

public class OpenglAdvancedActivity extends BaseActivity {

    public static final String TAG = "TextureDisplayActivity";
    private static final int IMAGE_FILE_SELECT_CODE = 100;

    private SurfaceView m_surface_view;
    private Button mSelectImage;
    private Button mVBOImage;
    private Button mVAOImage;
    private Button mMBOImage;
    private String mImagePath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_opengl_advanced);
        requestRunTimePermission(new String[]{
                        Manifest.permission.CAMERA,
                        Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE},
                null);
        initSurfaceView();
        initCommon();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
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

        mVBOImage = findViewById(R.id.vbo_image);
        mVBOImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: ");
                if(null != mImagePath && !mImagePath.isEmpty()) {
                    FunctionControl.getInstance().openglVertexBufferDisplay(mImagePath);  // VBO
                }
            }
        });

        mVAOImage = findViewById(R.id.vao_image);
        mVAOImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FunctionControl.getInstance().openglVertexArrayDisplay(mImagePath);  // VAO
            }
        });

        mMBOImage = findViewById(R.id.mbo_image);
        mMBOImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FunctionControl.getInstance().openglMapBufferDisplay(mImagePath);  // MBO
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
