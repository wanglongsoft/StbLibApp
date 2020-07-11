//
// Created by 24909 on 2020/7/2.
//

#include "FrameBufferDisplay.h"

FrameBufferDisplay::FrameBufferDisplay(GlobalContexts *global_context) {
    gl_program = -1;
    gl_uMatrix = -1;
    gl_position = -1;
    gl_textCoord = -1;
    isNeedRotation = false;
    this->context = global_context;
    this->gl_image_channels = this->context->gl_image_channels;
    setImageSize(this->context->gl_image_width, this->context->gl_image_height);
    setWindowSize(this->context->gl_window_width, this->context->gl_window_height);
    initDefMatrix();
}

FrameBufferDisplay::~FrameBufferDisplay() {
    if(NULL != vertex_shader_graphical) {
        delete vertex_shader_graphical;
        vertex_shader_graphical = NULL;
    }
    if(NULL != fragment_shader_graphical) {
        delete fragment_shader_graphical;
        fragment_shader_graphical = NULL;
    }

    if(pixel_data != NULL) {
        free(pixel_data);
        pixel_data = NULL;
    }

    glDeleteTextures(1, &gl_texture_id);
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteRenderbuffers(1, &colorRenderBuffer);
    glDeleteRenderbuffers(1, &depthRenderBuffer);
    glDeleteProgram(gl_program);
    glDeleteProgram(gl_program_frame);
}

GLuint FrameBufferDisplay::LoadShader(GLenum type, const char *shaderSrc) {
    LOGD("LoadShader type: %d", type);
    GLuint shader;
    shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length + 1];
        glGetShaderInfoLog(shader, length, &length, log);
        LOGD("glCompileShader fail: %s", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint FrameBufferDisplay::LoadProgram(const char *vShaderStr, const char *fShaderStr) {
    LOGD("LoadProgram in");
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint mProgram;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    if(0 == vertexShader) {
        LOGD("LoadShader vertexShader failed");
        return -1;
    }

    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    if(0 == fragmentShader) {
        LOGD("LoadShader vertexShader failed");
        return -1;
    }

    // Create the program object
    mProgram = glCreateProgram();

    // Attaches a shader object to a program object
    glAttachShader(mProgram, vertexShader);
    glAttachShader(mProgram, fragmentShader);

    // Link the program object
    glLinkProgram(mProgram);
    GLint status = 0;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &status);
    if (status == EGL_FALSE) {
        GLint length = 0;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length + 1];
        glGetProgramInfoLog(mProgram, length, &length, log);
        LOGD("glLinkProgram failed : %s", log);
        glDeleteProgram (mProgram);
        return -1;
    }
    LOGD("glLinkProgram success");

    glDeleteShader (vertexShader);
    glDeleteShader (fragmentShader);

    return mProgram;
}

int FrameBufferDisplay::createProgram() {
    LOGD("CreateProgram in : ");

    //eglMakeCurrent 函数来将当前的上下文切换，这样opengl的函数才能启动作用
    if(EGL_TRUE != eglMakeCurrent(context->eglDisplay,
                                  context->eglSurface, context->eglSurface,
                                  context->eglContext)) {
        LOGD("eglMakeCurrent failed");
        return -1;
    }

    //加载系统窗口使用shader
    vertex_shader_graphical = ShaderUtils::openAssetsFile(this->context->assetManager, "vertex_texture_display_frame.glsl");
    fragment_shader_graphical = ShaderUtils::openAssetsFile(this->context->assetManager, "fragment_texture_display_frame.glsl");

    gl_program = LoadProgram(vertex_shader_graphical->c_str(), fragment_shader_graphical->c_str());

    if(gl_program == -1) {
        LOGD("LoadProgram gl_program failed");
        return -1;
    }

    delete vertex_shader_graphical;
    vertex_shader_graphical = NULL;
    delete fragment_shader_graphical;
    fragment_shader_graphical = NULL;

    //加载FrameBuffer使用shader
    vertex_shader_graphical = ShaderUtils::openAssetsFile(this->context->assetManager, "vertex_texture_display_frame.glsl");
    fragment_shader_graphical = ShaderUtils::openAssetsFile(this->context->assetManager, "fragment_texture_display_frame.glsl");

    gl_program_frame = LoadProgram(vertex_shader_graphical->c_str(), fragment_shader_graphical->c_str());

    LOGD("createProgram : gl_program: %d  gl_program_frame: %d", gl_program, gl_program_frame);

    if(gl_program_frame == -1) {
        LOGD("LoadProgram gl_program_frame failed");
        return -1;
    }

    initCommonAttributes();

    return gl_program;
}

void FrameBufferDisplay::render(unsigned char *data) {
    this->rgba_data = data;

    GLenum status_frame = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(GL_FRAMEBUFFER_COMPLETE != status_frame) {
        LOGD("glCheckFramebufferStatus fail, return");
        return ;
    }

    glViewport(0, 0, context->gl_window_width, context->gl_window_height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    initShaderAttributes(gl_program_frame, true);
    glUseProgram(gl_program_frame);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_texture_id);

    //替换纹理，比重新使用glTexImage2D性能高多
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,//细节基本 默认0
                    0, //相对原来的纹理的offset
                    0,//相对原来的纹理的offset
                    context->gl_image_width,
                    context->gl_image_height,//加载的纹理宽度、高度。最好为2的次幂
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    this->rgba_data);

    glEnableVertexAttribArray(gl_uMatrix);
    glUniformMatrix4fv(gl_uMatrix, 1, false, matrix_scale);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    readPixelData();

    glDisable(GL_DEPTH_TEST);
    glBindRenderbuffer(GL_RENDERBUFFER, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);  //切换为原生窗口,方便显示

    glViewport(0, 0, context->gl_window_width, context->gl_window_height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    initShaderAttributes(gl_program, false);
    glUseProgram(gl_program);

    glActiveTexture(GL_TEXTURE0);
    //绑定对应的纹理
    glBindTexture(GL_TEXTURE_2D, gl_texture_id);

    //替换纹理，比重新使用glTexImage2D性能高多
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,//细节基本 默认0
                    0, //相对原来的纹理的offset
                    0,//相对原来的纹理的offset
                    context->gl_image_width,
                    context->gl_image_height,//加载的纹理宽度、高度。最好为2的次幂
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    this->rgba_data);

    glEnableVertexAttribArray(gl_uMatrix);
    glUniformMatrix4fv(gl_uMatrix, 1, false, matrix_scale);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    eglSwapBuffers(context->eglDisplay, context->eglSurface);
}

void FrameBufferDisplay::setImageSize(int width, int height) {
    this->gl_image_width = width;
    this->gl_image_height = height;
}

void FrameBufferDisplay::setWindowSize(int width, int height) {
    this->gl_window_width = width;
    this->gl_window_height = height;
}

void FrameBufferDisplay::initDefMatrix() {
    LOGD("gl_image_width: %d, gl_image_height: %d, gl_window_width: %d, gl_window_height: %d",
         gl_image_width, gl_image_height, gl_window_width, gl_window_height);
    float originRatio = (float) gl_image_width / gl_image_height;
    float worldRatio = (float) gl_window_width / gl_window_height;
//    originRatio = adjustRatio(originRatio, worldRatio);  //宽高比与屏幕不同时，转图片
    if (worldRatio > 1) {
        if (originRatio > worldRatio) {
            float actualRatio = originRatio / worldRatio;
            orthoM(
                    matrix_scale, 0,
                    -1, 1,
                    -actualRatio, actualRatio,
                    -1, 3
            );
        } else {// 原始比例小于窗口比例，缩放高度度会导致高度超出，因此，高度以窗口为准，缩放宽度
            float actualRatio = worldRatio / originRatio;
            orthoM(
                    matrix_scale, 0,
                    -actualRatio, actualRatio,
                    -1, 1,
                    -1, 3
            );
        }
    } else {
        if (originRatio > worldRatio) {
            float actualRatio = originRatio / worldRatio;
            orthoM(
                    matrix_scale, 0,
                    -1, 1,
                    -actualRatio, actualRatio,
                    -1, 3
            );
        } else {// 原始比例小于窗口比例，缩放高度会导致高度超出，因此，高度以窗口为准，缩放宽度
            float actualRatio = worldRatio / originRatio;
            orthoM(
                    matrix_scale, 0,
                    -actualRatio, actualRatio,
                    -1, 1,
                    -1, 3
            );
        }
    }
}

void FrameBufferDisplay::orthoM(float *m, int mOffset, float left, float right, float bottom, float top,
                            float near, float far) {
    float r_width  = 1 / (right - left);
    float r_height = 1 / (top - bottom);
    float r_depth  = 1 / (far - near);
    float x =  2 * (r_width);
    float y =  2 * (r_height);
    float z = -2 * (r_depth);
    float tx = -(right + left) * r_width;
    float ty = -(top + bottom) * r_height;
    float tz = -(far + near) * r_depth;
    m[mOffset + 0] = x;
    m[mOffset + 5] = y;
    m[mOffset + 10] = z;
    m[mOffset + 12] = tx;
    m[mOffset + 13] = ty;
    m[mOffset + 14] = tz;
    m[mOffset + 15] = 1;
    m[mOffset + 1] = 0;
    m[mOffset + 2] = 0;
    m[mOffset + 3] = 0;
    m[mOffset + 4] = 0;
    m[mOffset + 6] = 0;
    m[mOffset + 7] = 0;
    m[mOffset + 8] = 0;
    m[mOffset + 9] = 0;
    m[mOffset + 11] = 0;
}

float FrameBufferDisplay::adjustRatio(float ratio, float worldRatio) {
    if((ratio > 1 && worldRatio < 1) ||
       (ratio < 1 && worldRatio > 1)) {
        isNeedRotation = true;
        return 1 / ratio;
    }
    isNeedRotation = false;
    return ratio;
}

void FrameBufferDisplay::readPixelData() {
    LOGD("readPixelData in");

    read_image_width = gl_window_width;
    read_image_height = gl_window_height;

    if(pixel_data != NULL) {
        free(pixel_data);
        pixel_data = NULL;
    }

    pixel_data = (GLubyte *) malloc(sizeof(GLubyte) * read_image_width * read_image_height * 4);
    if(NULL == pixel_data) {
        LOGD("readPixelData malloc fail, return");
        return;
    }
    memset(pixel_data, 0, sizeof(GLubyte) * read_image_width * read_image_height * 4);
    glReadPixels(0, 0, read_image_width, read_image_height, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);
    LOGD("readPixelData out");
}

void FrameBufferDisplay::savePixelData() {
    LOGD("savePixelData in");
    if(NULL == pixel_data) {
        LOGD("pixel_data == null, return");
        return;
    }
    int ret = 0;
    ret = stbi_write_png("/storage/emulated/0/image_test/image_windows.png", read_image_width, read_image_height, 4, pixel_data, 0);
    LOGD("stbi_write_png ret = %d", ret);
    free(pixel_data);
    pixel_data = NULL;
    LOGD("savePixelData out");
}

void FrameBufferDisplay::initShaderAttributes(GLint program, bool is_frame_program) {
    //获取顶点着色器和片段着色器句柄句柄

    LOGD("initShaderAttributes isNeedRotation : %d  is_frame_program: %d", isNeedRotation, is_frame_program);

    gl_position = glGetAttribLocation(program, "a_position");

    glEnableVertexAttribArray(gl_position);
    LOGD("initShaderAttributes gl_position : %d", gl_position);
    glVertexAttribPointer(gl_position, 3, GL_FLOAT, GL_FALSE, 0, vertex_coords);

    gl_textCoord = glGetAttribLocation(program, "a_texCoord");
    glEnableVertexAttribArray(gl_textCoord);
    LOGD("initShaderAttributes gl_textCoord : %d", gl_textCoord);

    if(!is_frame_program) {
        glVertexAttribPointer(gl_textCoord, 2, GL_FLOAT, GL_FALSE, 0, fragment_coords);
    } else {
        glVertexAttribPointer(gl_textCoord, 2, GL_FLOAT, GL_FALSE, 0, fragment_coords_frame);
    }

    glUniform1i(glGetUniformLocation(program, "uTexture"), 0);

    gl_uMatrix = glGetUniformLocation(program, "uMatrix");
    LOGD("initShaderAttributes gl_uMatrix : %d", gl_uMatrix);
}

void FrameBufferDisplay::initCommonAttributes() {
    //创建若干个纹理对象，并且得到纹理ID
    glGenTextures(1, &gl_texture_id);

    //将纹理目标和纹理绑定后，对纹理目标所进行的操作都反映到对纹理上
    glBindTexture(GL_TEXTURE_2D, gl_texture_id);

    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_RGBA,
                 context->gl_image_width,//加载的纹理宽度
                 context->gl_image_height,//加载的纹理高度
                 0,//纹理边框
                 GL_RGBA,//数据的像素格式, RGB数据格式
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );

    //过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //设置帧缓冲区
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRendBufferSize);
    LOGD("maxRendBufferSize : %d", maxRendBufferSize);
    glGenFramebuffers(1, &frameBuffer);
    glGenRenderbuffers(1, &depthRenderBuffer);
    glGenRenderbuffers(1, &colorRenderBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, context->gl_window_width, context->gl_window_height);

    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, context->gl_window_width, context->gl_window_height);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // 不能直接使用纹理附着，否则采样器颜色混乱
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_texture_id, 0);
//  使用渲染缓冲区作为颜色附着，不能直接使用纹理附着
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    LOGD("initTextureAttributes out");
}
