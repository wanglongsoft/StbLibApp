#include <jni.h>
#include <string>

#include <pthread.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include "GlobalContexts.h"
#include "LogUtils.h"
#include "EGLDisplayYUV.h"
#include "ShaderYUV.h"
#include "TextureDisplay.h"
#include "FilterDisplay.h"
#include "VertexBufferDisplay.h"
#include "VertexArrayDisplay.h"
#include "MapBuffersDisplay.h"
#include "FrameBufferDisplay.h"
#include "WaterMarkDisplay.h"
#include "FrameBufferTexture.h"

#include "LogUtils.h"

//图片处理开源库:stb
//https://github.com/nothings/stb

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

unsigned char* convertJByteaArrayToChars(JNIEnv *env, jbyteArray bytearray);
unsigned char* convertNV21ToYUV420P(unsigned char* array_data, int width, int height);

ANativeWindow * nativeWindow = NULL;
GlobalContexts *global_context = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
EGLDisplayYUV *eglDisplayYuv = NULL;
ShaderYUV * shaderYuv = NULL;
TextureDisplay* textureDisplay = NULL;
FilterDisplay* filterDisplay = NULL;
VertexBufferDisplay* vertexBufferDisplay = NULL;
VertexArrayDisplay* vertexArrayDisplay = NULL;
WaterMarkDisplay* waterMarkDisplay = NULL;

MapBuffersDisplay* mapBuffersDisplay = NULL;
FrameBufferDisplay* frameBufferDisplay = NULL;
FrameBufferTexture* frameBufferTexture = NULL;

unsigned char* array_data = NULL;
unsigned char* yuv_array_data = NULL;
unsigned char* y_array_data = NULL;
unsigned char* u_array_data = NULL;
unsigned char* v_array_data = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    LOGD("setSurface in");
    pthread_mutex_lock(&mutex);
    if (nativeWindow) {
        LOGD("setSurface nativeWindow != NULL");
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }

    // 创建新的窗口用于视频显示
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if(NULL == global_context) {
        LOGD("new GlobalContext");
        global_context = new GlobalContexts();
    }
    global_context->nativeWindow = nativeWindow;

    if(NULL != eglDisplayYuv) {
        LOGD("eglDisplayYuv->eglClose");
        eglDisplayYuv->eglClose();
        delete eglDisplayYuv;
        eglDisplayYuv = NULL;
    }
    if(NULL != shaderYuv) {
        delete shaderYuv;
        shaderYuv = NULL;
    }
    pthread_mutex_unlock(&mutex);
    LOGD("setSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_setSurfaceSize(JNIEnv *env, jobject thiz, jint width,
                                                    jint height) {
    LOGD("setSurfaceSize in");
    pthread_mutex_lock(&mutex);
    if(NULL == global_context) {
        global_context = new GlobalContexts();
    }
    global_context->gl_window_width = width;
    global_context->gl_window_height = height;
    pthread_mutex_unlock(&mutex);
    LOGD("setSurfaceSize out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_rendSurface(JNIEnv *env, jobject thiz, jbyteArray data,
                                                 jint width, jint height, jint video_rotation) {
    LOGD("rendSurface in");
    if(NULL == global_context) {
        global_context = new GlobalContexts();
    }
    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
        global_context->gl_video_width = width;
        global_context->gl_video_height = height;
    }
    if(global_context->gl_video_rotation_angle != video_rotation) {
        global_context->gl_video_rotation_angle = video_rotation;
        LOGD("rotation_angle is change : %d", global_context->gl_video_rotation_angle);
        if(shaderYuv != NULL) {
            shaderYuv->changeVideoRotation();
        }
    }
    if(NULL == shaderYuv) {
        shaderYuv = new ShaderYUV(global_context);
        shaderYuv->CreateProgram();
    }
    if(NULL != array_data) {
        delete array_data;
    }
    if(NULL != yuv_array_data) {
        delete yuv_array_data;
    }
    array_data = convertJByteaArrayToChars(env, data);
    yuv_array_data = convertNV21ToYUV420P(array_data, width, height);
    unsigned char* frame_data[3];
    frame_data[0] = yuv_array_data;
    frame_data[1] = yuv_array_data + width * height;
    frame_data[2] = yuv_array_data + width * height +  width * height / 4;
    shaderYuv->Render(frame_data);
    pthread_mutex_unlock(&mutex);
    LOGD("rendSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_saveAssetManager(JNIEnv *env, jobject thiz, jobject manager) {
    LOGD("saveAssetManager in");
    pthread_mutex_lock(&mutex);
    AAssetManager *mgr = AAssetManager_fromJava(env, manager);
    if(NULL == global_context) {
        global_context = new GlobalContexts();
    }
    global_context->assetManager = mgr;
    pthread_mutex_unlock(&mutex);
    LOGD("saveAssetManager out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_releaseResources(JNIEnv *env, jobject thiz) {
    LOGD("releaseResources in");
    pthread_mutex_lock(&mutex);
    if(NULL != shaderYuv) {
        delete shaderYuv;
        shaderYuv = NULL;
    }
    if(NULL != eglDisplayYuv) {
        eglDisplayYuv->eglClose();
        delete eglDisplayYuv;
        eglDisplayYuv = NULL;
    }
    if(NULL != global_context) {
        delete global_context;
        global_context = NULL;
    }
    if(NULL != array_data) {
        delete array_data;
        array_data = NULL;
    }
    if(NULL != yuv_array_data) {
        delete yuv_array_data;
        yuv_array_data = NULL;
    }
    if(NULL != y_array_data) {
        delete y_array_data;
        y_array_data = NULL;
    }
    if(NULL != u_array_data) {
        delete u_array_data;
        u_array_data = NULL;
    }
    if(NULL != v_array_data) {
        delete v_array_data;
        v_array_data = NULL;
    }
    if (NULL != nativeWindow) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }
    if (NULL != nativeWindow) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }

    if (NULL != textureDisplay) {
        delete textureDisplay;
        textureDisplay = NULL;
    }

    if (NULL != filterDisplay) {
        delete filterDisplay;
        filterDisplay = NULL;
    }

    if (NULL != vertexBufferDisplay) {
        delete vertexBufferDisplay;
        vertexBufferDisplay = NULL;
    }

    if (NULL != vertexArrayDisplay) {
        delete vertexArrayDisplay;
        vertexArrayDisplay = NULL;
    }

    if (NULL != mapBuffersDisplay) {
        delete mapBuffersDisplay;
        mapBuffersDisplay = NULL;
    }

    if (NULL != waterMarkDisplay) {
        delete waterMarkDisplay;
        waterMarkDisplay = NULL;
    }

    if (NULL != frameBufferDisplay) {
        delete frameBufferDisplay;
        frameBufferDisplay = NULL;
    }

    if (NULL != frameBufferTexture) {
        delete frameBufferTexture;
        frameBufferTexture = NULL;
    }

    pthread_mutex_unlock(&mutex);
    LOGD("releaseResources out");
}

unsigned char* convertJByteaArrayToChars(JNIEnv *env, jbyteArray bytearray)
{
    unsigned char *chars = NULL;
    jbyte *bytes;
    bytes = env->GetByteArrayElements(bytearray, 0);
    int chars_len = env->GetArrayLength(bytearray);
    chars = new unsigned char[chars_len + 1];//使用结束后, delete 该数组
    memset(chars,0,chars_len + 1);
    memcpy(chars, bytes, chars_len);
    chars[chars_len] = 0;
    env->ReleaseByteArrayElements(bytearray, bytes, 0);
    return chars;
}

unsigned char* convertNV21ToYUV420P(unsigned char* array_data, int width, int height) {
    unsigned char *src_chars = array_data;
    unsigned char *dst_chars = NULL;
    dst_chars = new unsigned char[width * height / 2 * 3 + 1];
    int uv_data_size = width * height / 4;
    int y_data_size = width * height;
    for (int i = 0; i < y_data_size; ++i) {
        dst_chars[i] = src_chars[i];
    }
    for (int i = 0; i < uv_data_size; ++i) {
        dst_chars[i + y_data_size] = src_chars[2 * i + 1 + y_data_size];//  U数据存储
        dst_chars[i + y_data_size + uv_data_size] = src_chars[2 * i + y_data_size];//  V数据存储
    }
    return dst_chars;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_displayImage(JNIEnv *env, jobject thiz, jstring image_path) {
    LOGD("displayImage in");
    const char * src_image_path = env->GetStringUTFChars(image_path, NULL);
    int image_width;
    int image_height;
    int image_channels;
    unsigned char *idata = stbi_load(src_image_path, &image_width, &image_height, &image_channels, 4);

    if(NULL == idata) {
        LOGD("stbi_load parse fail, return");
        return;
    }
    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    if(NULL != textureDisplay) {
        delete textureDisplay;
        textureDisplay = NULL;
    }

    global_context->gl_image_width = image_width;
    global_context->gl_image_height = image_height;
    global_context->gl_image_channels = image_channels;

    textureDisplay = new TextureDisplay(global_context);

    if(textureDisplay->createProgram() != -1) {
        LOGD("createProgram success");
        textureDisplay->render(idata);
    } else {
        LOGD("createProgram fail");
    }
    stbi_image_free(idata);
    env->ReleaseStringUTFChars(image_path, src_image_path);
    LOGD("displayImage out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_applayFilter(JNIEnv *env, jobject thiz, jstring path,
                                                  jint type) {
    LOGD("applayFilter in");
    const char * src_image_path = env->GetStringUTFChars(path, NULL);
    int image_width;
    int image_height;
    int image_channels;
    unsigned char *idata = stbi_load(src_image_path, &image_width, &image_height, &image_channels, 4);

    if(NULL == idata) {
        LOGD("stbi_load parse fail, return");
        return;
    }
    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    if(NULL != filterDisplay) {
        delete filterDisplay;
        filterDisplay = NULL;
    }

    global_context->gl_image_width = image_width;
    global_context->gl_image_height = image_height;
    global_context->gl_image_channels = image_channels;
    global_context->gl_filter_type = type;
    filterDisplay = new FilterDisplay(global_context);

    if(filterDisplay->createProgram() != -1) {
        LOGD("createProgram success");
        filterDisplay->render(idata);
    } else {
        LOGD("createProgram fail");
    }
    stbi_image_free(idata);
    env->ReleaseStringUTFChars(path, src_image_path);
    LOGD("applayFilter out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_openglVertexBufferDisplay(JNIEnv *env, jobject thiz, jstring path) {
    LOGD("openglAdvanceDisplay in");
    const char * src_image_path = env->GetStringUTFChars(path, NULL);
    int image_width;
    int image_height;
    int image_channels;
    unsigned char *idata = stbi_load(src_image_path, &image_width, &image_height, &image_channels, 4);

    if(NULL == idata) {
        LOGD("stbi_load parse fail, return");
        return;
    }
    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    if(NULL != vertexBufferDisplay) {
        delete vertexBufferDisplay;
        vertexBufferDisplay = NULL;
    }

    global_context->gl_image_width = image_width;
    global_context->gl_image_height = image_height;
    global_context->gl_image_channels = image_channels;

    vertexBufferDisplay = new VertexBufferDisplay(global_context);

    if(vertexBufferDisplay->createProgram() != -1) {
        LOGD("createProgram success");
        vertexBufferDisplay->render(idata);
    } else {
        LOGD("createProgram fail");
    }
    stbi_image_free(idata);
    env->ReleaseStringUTFChars(path, src_image_path);
    LOGD("openglAdvanceDisplay out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_openglMapBufferDisplay(JNIEnv *env, jobject thiz,
                                                            jstring path) {
    LOGD("openglMapBufferDisplay in");
    const char * src_image_path = env->GetStringUTFChars(path, NULL);
    int image_width;
    int image_height;
    int image_channels;
    unsigned char *idata = stbi_load(src_image_path, &image_width, &image_height, &image_channels, 4);

    if(NULL == idata) {
        LOGD("stbi_load parse fail, return");
        return;
    }
    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    if(NULL != mapBuffersDisplay) {
        delete mapBuffersDisplay;
        mapBuffersDisplay = NULL;
    }

    global_context->gl_image_width = image_width;
    global_context->gl_image_height = image_height;
    global_context->gl_image_channels = image_channels;

    mapBuffersDisplay = new MapBuffersDisplay(global_context);

    if(mapBuffersDisplay->createProgram() != -1) {
        LOGD("createProgram success");
        mapBuffersDisplay->render(idata);
    } else {
        LOGD("createProgram fail");
    }
    stbi_image_free(idata);
    env->ReleaseStringUTFChars(path, src_image_path);
    LOGD("openglMapBufferDisplay out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_openglFrameBufferDisplay(JNIEnv *env, jobject thiz,
                                                              jstring path) {
    LOGD("openglFrameBufferDisplay in");
    const char * src_image_path = env->GetStringUTFChars(path, NULL);
    int image_width;
    int image_height;
    int image_channels;
    unsigned char *idata = stbi_load(src_image_path, &image_width, &image_height, &image_channels, 4);

    if(NULL == idata) {
        LOGD("stbi_load parse fail, return");
        return;
    }
    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    global_context->gl_image_width = image_width;
    global_context->gl_image_height = image_height;
    global_context->gl_image_channels = image_channels;

//  测试FrameBuffer 纹理附着和深度附着     Case 1  开始
//    if(NULL != frameBufferTexture) {
//        delete frameBufferTexture;
//        frameBufferTexture = NULL;
//    }

//    frameBufferTexture = new FrameBufferTexture(global_context);
//
//    if(frameBufferTexture->createProgram() != -1) {
//        LOGD("createProgram success");
//        frameBufferTexture->render(idata);
//    } else {
//        LOGD("createProgram fail");
//    }
//  测试FrameBuffer 纹理附着和深度附着     Case 1  结束

//  测试时，Case 2 与   Case 1 注释一个
//  测试FrameBuffer 颜色附着和深度附着     Case 2  开始

    if(NULL != frameBufferDisplay) {
        delete frameBufferDisplay;
        frameBufferDisplay = NULL;
    }

    frameBufferDisplay = new FrameBufferDisplay(global_context);

    if(frameBufferDisplay->createProgram() != -1) {
        LOGD("createProgram success");
        frameBufferDisplay->render(idata);
    } else {
        LOGD("createProgram fail");
    }

//  测试FrameBuffer 颜色附着和深度附着     Case 2  结束

    stbi_image_free(idata);
    env->ReleaseStringUTFChars(path, src_image_path);
    LOGD("openglFrameBufferDisplay out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_savePixelData(JNIEnv *env, jobject thiz) {
    LOGD("openglFrameBufferData in");
    //  测试FrameBuffer 纹理附着和深度附着
    if(NULL != frameBufferTexture) {
        frameBufferTexture->savePixelData();
    }
    //  测试FrameBuffer 颜色附着和深度附着
    if(NULL != frameBufferDisplay) {
        frameBufferDisplay->savePixelData();
    }
    LOGD("openglFrameBufferData out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_openglVertexArrayDisplay(JNIEnv *env, jobject thiz,
                                                              jstring path) {
    LOGD("openglVertexArrayDisplay in");
    const char * src_image_path = env->GetStringUTFChars(path, NULL);
    int image_width;
    int image_height;
    int image_channels;
    unsigned char *idata = stbi_load(src_image_path, &image_width, &image_height, &image_channels, 4);

    if(NULL == idata) {
        LOGD("stbi_load parse fail, return");
        return;
    }
    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    if(NULL != vertexArrayDisplay) {
        delete vertexArrayDisplay;
        vertexArrayDisplay = NULL;
    }

    global_context->gl_image_width = image_width;
    global_context->gl_image_height = image_height;
    global_context->gl_image_channels = image_channels;

    vertexArrayDisplay = new VertexArrayDisplay(global_context);

    if(vertexArrayDisplay->createProgram() != -1) {
        LOGD("createProgram success");
        vertexArrayDisplay->render(idata);
    } else {
        LOGD("createProgram fail");
    }
    stbi_image_free(idata);
    env->ReleaseStringUTFChars(path, src_image_path);
    LOGD("openglVertexArrayDisplay out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_soft_common_FunctionControl_openglWaterMarkDisplay(JNIEnv *env, jobject thiz,
                                                           jstring image_path, jstring water_image_path) {
    LOGD("openglWaterMarkDisplay in");
    const char * src_image_path = env->GetStringUTFChars(image_path, NULL);
    const char * water_path = "/storage/emulated/0/image_test/water_mark.png";
    int image_width;
    int image_height;
    int image_channels;
    unsigned char *image_data = stbi_load(src_image_path, &image_width, &image_height,
            &image_channels, 4);

    int water_image_width;
    int water_image_height;
    int water_image_channels;
    unsigned char *water_image_data = stbi_load(water_path, &water_image_width, &water_image_height,
            &water_image_channels, 4);

    if(NULL == water_image_data || NULL == image_data) {
        LOGD("water_image_data or image_data == null, return");
        return;
    }

    if(NULL == eglDisplayYuv) {
        eglDisplayYuv = new EGLDisplayYUV(global_context->nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    if(NULL != waterMarkDisplay) {
        delete waterMarkDisplay;
        waterMarkDisplay = NULL;
    }

    global_context->gl_image_width = image_width;
    global_context->gl_image_height = image_height;
    global_context->gl_image_channels = image_channels;

    global_context->gl_water_image_width = water_image_width;
    global_context->gl_water_image_height = water_image_height;
    global_context->gl_water_image_channels = water_image_channels;

    LOGD("gl_water_image_width : %d, gl_water_image_height: %d, gl_water_image_channels: %d",
            water_image_width, water_image_height, water_image_channels);

    waterMarkDisplay = new WaterMarkDisplay(global_context);

    if(waterMarkDisplay->createProgram() != -1) {
        LOGD("createProgram success");
        waterMarkDisplay->render(image_data, water_image_data);
    } else {
        LOGD("createProgram fail");
    }

    stbi_image_free(image_data);
    stbi_image_free(water_image_data);
    env->ReleaseStringUTFChars(image_path, src_image_path);
    LOGD("openglWaterMarkDisplay out");
}