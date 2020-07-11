package com.soft.stblibapp;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.soft.common.BaseActivity;
import com.soft.common.FunctionControl;
import com.soft.common.GetPathFromUri;

//滤镜
public class FilterActivity extends BaseActivity {

    public static final String TAG = "FilterActivity";
    private static final int IMAGE_FILE_SELECT_CODE = 200;

    private Button mSelectImage;
    private Button mSrcImage;
    private SurfaceView m_surface_view;
    private RecyclerView recyclerView;
    private ListViewAdapter listViewAdapter;
    private RecyclerView.LayoutManager layoutManager;

    private String mImagePath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_filter);

        initSurfaceView();
        initRecyclerView();
        initCommon();
    }

    private void initRecyclerView() {
        recyclerView = findViewById(R.id.recycler_view);
        layoutManager = new LinearLayoutManager(this);
        listViewAdapter = new ListViewAdapter(new String[] {
                "区域可上下滚动",
                "灰度",
                "旋涡滤镜",
                "马赛克",
                "分屏",
                "纯黑白",
                "负片",
                "怀旧",
                "冷调",
                "图片点阵",
                "灵魂出窍",
                "分色偏移",
        });
        recyclerView.setLayoutManager(layoutManager);
        recyclerView.setAdapter(listViewAdapter);
        recyclerView.addItemDecoration(new DividerItemDecoration(this,DividerItemDecoration.VERTICAL));
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

        mSrcImage = findViewById(R.id.src_image);
        mSrcImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: ");
                if(null != mImagePath && !mImagePath.isEmpty()) {
                    FunctionControl.getInstance().applayFilter(mImagePath, -1);
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

    public class ListViewAdapter extends RecyclerView.Adapter<ListViewAdapter.MyViewHolder> {

        private String[] mDataset;
        public class MyViewHolder extends RecyclerView.ViewHolder {
            public Button button;
            public MyViewHolder(ConstraintLayout root) {
                super(root);
                button = root.findViewById(R.id.view_btn);
            }
        }

        public ListViewAdapter(String[] myDataset) {
            mDataset = myDataset;
        }

        @NonNull
        @Override
        public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            ConstraintLayout root = (ConstraintLayout) LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.recycler_view_layout, parent, false);
            MyViewHolder vh = new MyViewHolder(root);
            return vh;
        }

        @Override
        public void onBindViewHolder(@NonNull ListViewAdapter.MyViewHolder holder, final int position) {
            holder.button.setText(mDataset[position]);
            holder.button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.d(TAG, "onClick position: " + position);
                    if(null != mImagePath && !mImagePath.isEmpty()) {
                        FunctionControl.getInstance().applayFilter(mImagePath, position);
                    }
                }
            });
        }

        @Override
        public int getItemCount() {
            return mDataset.length;
        }
    }
}
