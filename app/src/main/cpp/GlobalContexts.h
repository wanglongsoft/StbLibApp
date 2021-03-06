//
// Created by 24909 on 2020/3/23.
//

#ifndef FFMPEGPLAYER_GLOBALCONTEXTS_H
#define FFMPEGPLAYER_GLOBALCONTEXTS_H

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>

class GlobalContexts {
public:
    GlobalContexts();
    ~GlobalContexts();

    EGLDisplay eglDisplay;
    EGLSurface eglSurface;
    EGLContext eglContext;
    EGLint eglFormat;

    GLuint mTextureID;
    GLuint glProgram;
    GLint positionLoc;
    GLuint mProgram;
    GLint gl_position;
    GLint gl_color;
    GLint gl_coordinate;
    GLint gl_uTexture;
    GLint gl_uMatrix;
    GLint gl_textCoord;
    GLuint gl_texture_id[3];
    GLint gl_video_width;
    GLint gl_video_height;
    GLint gl_video_rotation_angle;
    GLint gl_window_width;
    GLint gl_window_height;
    GLint gl_image_width;
    GLint gl_image_height;
    GLint gl_image_channels;
    GLint gl_water_image_width;
    GLint gl_water_image_height;
    GLint gl_water_image_channels;
    GLint gl_filter_type;
    ANativeWindow * nativeWindow;
    AAssetManager * assetManager;
};


#endif //FFMPEGPLAYER_GLOBALCONTEXTS_H
