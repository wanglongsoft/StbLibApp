//
// Created by 24909 on 2020/7/2.
//

#ifndef STBLIBAPP_FRAMEBUFFERDISPLAY_H
#define STBLIBAPP_FRAMEBUFFERDISPLAY_H

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <math.h>
#include "GlobalContexts.h"
#include "LogUtils.h"
#include "ShaderUtils.h"

#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION

//FrameBuffer 颜色附着和深度附着

class FrameBufferDisplay {
public:
    FrameBufferDisplay(GlobalContexts *global_context);
    ~FrameBufferDisplay();
    GLuint LoadShader(GLenum type, const char *shaderSrc);
    GLuint LoadProgram(const char *vShaderStr, const char *fShaderStr);
    int createProgram();
    void initShaderAttributes(GLint program, bool is_frame_program);
    void initCommonAttributes();

    void render(unsigned char *data);
    void setImageSize(int width, int height);
    void setWindowSize(int width, int height);
    void initDefMatrix();
    void readPixelData();
    void savePixelData();
    void orthoM(float m[], int mOffset,
                float left, float right, float bottom, float top,
                float near, float far);
    float adjustRatio(float ratio, float worldRatio);

    float vertex_coords[12] = {//世界坐标
            -1, -1, 0, // left bottom
            1, -1, 0, // right bottom
            -1, 1, 0,  // left top
            1, 1, 0,   // right top
    };

    float fragment_coords[8] = {//纹理坐标
            0, 1,//left bottom
            1, 1,//right bottom
            0, 0,//left top
            1, 0,//right top
    };

    //纹理坐标, glReadPixels是从图片从左下到右上顺序读取像素，
    //因此需要对图片纹理坐标左变换
    float fragment_coords_frame[8] = {
            0, 0,//left bottom
            1, 0,//right bottom
            0, 1,//left top
            1, 1,//right top
    };

    float matrix_scale[16];

    GLint gl_program;
    GLint gl_program_frame;

    GLint gl_position;
    GLint gl_textCoord;
    GLint gl_uMatrix;

    GLint gl_image_width;
    GLint gl_image_height;
    GLint gl_window_width;
    GLint gl_window_height;
    GLuint gl_image_channels;

    GLuint gl_texture_id;
    std::string *vertex_shader_graphical = NULL;
    std::string *fragment_shader_graphical = NULL;

    uint8_t *rgba_data = NULL;
    bool isNeedRotation;
    GlobalContexts *context = NULL;

    GLuint frameBuffer;
    GLuint colorRenderBuffer;
    GLuint depthRenderBuffer;
    GLubyte *pixel_data = NULL;;

    GLint maxRendBufferSize;
    GLint read_image_width;
    GLint read_image_height;
};


#endif //STBLIBAPP_FRAMEBUFFERDISPLAY_H
