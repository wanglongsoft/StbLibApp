package com.soft.common;

import android.content.res.AssetManager;
import android.view.Surface;

public class FunctionControl {
    static {
        System.loadLibrary("native-lib");
    }

    private static volatile FunctionControl instance;

    private FunctionControl() {
    }

    public static FunctionControl getInstance() {
        if(null == instance) {
            synchronized (FunctionControl.class) {
                if(null == instance) {
                    instance = new FunctionControl();
                }
            }
        }
        return instance;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void setSurface(Surface surface);
    public native void setSurfaceSize(int width, int height);
    public native void rendSurface(byte data[], int width, int height, int video_rotation);
    public native void saveAssetManager(AssetManager manager);
    public native void displayImage(String path);
    public native void applayFilter(String path, int type);
    public native void openglVertexBufferDisplay(String path);
    public native void openglVertexArrayDisplay(String path);
    public native void openglMapBufferDisplay(String path);
    public native void openglFrameBufferDisplay(String path);
    public native void openglWaterMarkDisplay(String image_path, String water_image_path);
    public native void savePixelData();
    public native void releaseResources();
}
