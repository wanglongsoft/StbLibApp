//
// Created by 24909 on 2020/7/7.
//

#include "VertexArrayDisplay.h"

VertexArrayDisplay::VertexArrayDisplay(GlobalContexts *global_context) {
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

VertexArrayDisplay::~VertexArrayDisplay() {
    if(NULL != vertex_shader_graphical) {
        delete vertex_shader_graphical;
        vertex_shader_graphical = NULL;
    }
    if(NULL != fragment_shader_graphical) {
        delete fragment_shader_graphical;
        fragment_shader_graphical = NULL;
    }

    glDeleteTextures(1, &gl_texture_id);
    glDeleteProgram(gl_program);
    glDeleteBuffers ( 1, &gl_coord_id);
    glDeleteVertexArrays(1, &gl_texture_id_vao);
}

GLuint VertexArrayDisplay::LoadShader(GLenum type, const char *shaderSrc) {
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

GLuint VertexArrayDisplay::LoadProgram(const char *vShaderStr, const char *fShaderStr) {
    LOGD("LoadProgram in");
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint mProgram;

    //eglMakeCurrent()函数来将当前的上下文切换，这样opengl的函数才能启动作用
    if(EGL_TRUE != eglMakeCurrent(context->eglDisplay,
                                  context->eglSurface, context->eglSurface,
                                  context->eglContext)) {
        LOGD("eglMakeCurrent failed");
        return -1;
    }

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

    context->mProgram = mProgram;
    gl_program = mProgram;

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

    //VAO start
    glGenVertexArrays(1, &gl_texture_id_vao);
    glBindVertexArray(gl_texture_id_vao);
    glBindBuffer(GL_ARRAY_BUFFER,  gl_texture_id_vao);
    //VAO end   在绑定 VAO 之后，操作 VBO ，当前 VAO 会记录 VBO 的操作

    //VBO start
    glGenBuffers(1, &gl_coord_id);
    glBindBuffer(GL_ARRAY_BUFFER, gl_coord_id);
    if(!isNeedRotation) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (3 + 2) * 4, texture_coords, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (3 + 2) * 4, texture_coords_90, GL_STATIC_DRAW);
    }

    //获取顶点着色器和片段着色器句柄句柄
    gl_position = glGetAttribLocation(mProgram, "a_position");
    context->gl_position = gl_position;
    glEnableVertexAttribArray(gl_position);
    LOGD("LoadProgram gl_position : %d", gl_position);

    gl_textCoord = glGetAttribLocation(mProgram, "a_texCoord");
    context->gl_textCoord = gl_textCoord;
    glEnableVertexAttribArray(gl_textCoord);
    LOGD("LoadProgram gl_textCoord : %d", gl_textCoord);

    glVertexAttribPointer(gl_position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (3 + 2), ( const void * ) 0);
    glVertexAttribPointer(gl_textCoord, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * (3 + 2), ( const void * ) (3 * sizeof(GLfloat)));

    //VBO end

    //Reset to the default VAO
    glBindVertexArray(GL_NONE);

    glUniform1i(glGetUniformLocation(gl_program, "uTexture"), 0);

    gl_uMatrix = glGetUniformLocation(mProgram, "uMatrix");
    LOGD("LoadProgram gl_uMatrix : %d", gl_uMatrix);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, context->gl_window_width, context->gl_window_height);

    glUseProgram(gl_program);
    LOGD("LoadProgram out");
    return gl_program;
}

int VertexArrayDisplay::createProgram() {
    LOGD("CreateProgram in : ");
    //可以读Assets文件
    vertex_shader_graphical = ShaderUtils::openAssetsFile(this->context->assetManager, "vertex_texture_display.glsl");
    fragment_shader_graphical = ShaderUtils::openAssetsFile(this->context->assetManager, "fragment_texture_display.glsl");
    gl_program = LoadProgram(vertex_shader_graphical->c_str(), fragment_shader_graphical->c_str());
    LOGD("CreateProgram : gl_program: %d  image_width: %d, image_height： %d, window_width：%d, window_height：%d  gl_image_channels: %d",
         gl_program, gl_image_width, gl_image_height, gl_window_width, gl_window_height, gl_image_channels);
    return gl_program;
}

void VertexArrayDisplay::render(unsigned char *data) {
    this->rgba_data = data;

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

    glBindVertexArray(gl_texture_id_vao);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //窗口显示，交换双缓冲区
    eglSwapBuffers(context->eglDisplay, context->eglSurface);

    glDisableVertexAttribArray (gl_position);
    glDisableVertexAttribArray (gl_textCoord);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VertexArrayDisplay::setImageSize(int width, int height) {
    this->gl_image_width = width;
    this->gl_image_height = height;
}

void VertexArrayDisplay::setWindowSize(int width, int height) {
    this->gl_window_width = width;
    this->gl_window_height = height;
}

void VertexArrayDisplay::initDefMatrix() {
    LOGD("gl_image_width: %d, gl_image_height: %d, gl_window_width: %d, gl_window_height: %d",
         gl_image_width, gl_image_height, gl_window_width, gl_window_height);
    float originRatio = (float) gl_image_width / gl_image_height;
    float worldRatio = (float) gl_window_width / gl_window_height;
    originRatio = adjustRatio(originRatio, worldRatio);
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

void VertexArrayDisplay::orthoM(float *m, int mOffset, float left, float right, float bottom, float top,
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

float VertexArrayDisplay::adjustRatio(float ratio, float worldRatio) {
    if((ratio > 1 && worldRatio < 1) ||
       (ratio < 1 && worldRatio > 1)) {
        isNeedRotation = true;
        return 1 / ratio;
    }
    isNeedRotation = false;
    return ratio;
}