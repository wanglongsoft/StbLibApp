//
// Created by 24909 on 2020/7/7.
//

#ifndef STBLIBAPP_WATERMARKDISPLAY_H
#define STBLIBAPP_WATERMARKDISPLAY_H

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <math.h>
#include "GlobalContexts.h"
#include "LogUtils.h"
#include "ShaderUtils.h"
#include <string>

class WaterMarkDisplay {
public:
    WaterMarkDisplay(GlobalContexts *global_context);
    ~WaterMarkDisplay();
    GLuint LoadShader(GLenum type, const char *shaderSrc);
    GLuint LoadProgram(const char *vShaderStr, const char *fShaderStr);
    int createProgram();
    void render(unsigned char *src_data, unsigned char *water_data);
    void setImageSize(int width, int height);
    void setWindowSize(int width, int height);
    void initDefMatrix();
    void orthoM(float m[], int mOffset,
                float left, float right, float bottom, float top,
                float near, float far);
    float adjustRatio(float ratio, float worldRatio);

    float texture_coords[20] = {//世界坐标
            -1, -1, 0, 0, 1,// x, y, z, x, y
            1, -1, 0, 1, 1,// right bottom
            -1, 1, 0, 0, 0,// left top
            1, 1, 0,  1, 0,// right top
    };

    float texture_coords_90[20] = {//世界坐标
            -1, -1, 0, 1, 1,// x, y, z, x, y
            1, -1, 0, 1, 0,// right bottom
            -1, 1, 0, 0, 1,// left top
            1, 1, 0, 0, 0,// right top
    };

    float matrix_scale[16];

    GLint gl_program;
    GLint gl_position;
    GLint gl_textCoord;
    GLint gl_uMatrix;

    GLint gl_image_width;
    GLint gl_image_height;
    GLint gl_window_width;
    GLint gl_window_height;
    GLuint gl_image_channels;

    GLuint gl_texture_id;
    GLuint gl_water_id;
    GLuint gl_coord_id;

    GLint u_texture;
    GLint water_mark;

    std::string *vertex_shader_graphical = NULL;
    std::string *fragment_shader_graphical = NULL;

    //       N=#comp     components
    //       1           grey
    //       2           grey, alpha
    //       3           red, green, blue
    //       4           red, green, blue, alpha
    GLenum gl_data_format[5] = {
            GL_LUMINANCE,
            GL_LUMINANCE,
            GL_LUMINANCE_ALPHA,
            GL_RGB,
            GL_RGBA
    };//gl_image_channels

    uint8_t *rgba_data = NULL;
    uint8_t *water_data = NULL;
    bool isNeedRotation;
    GlobalContexts *context;
};


#endif //STBLIBAPP_WATERMARKDISPLAY_H
