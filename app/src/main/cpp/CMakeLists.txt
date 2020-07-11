
cmake_minimum_required(VERSION 3.4.1)

file(GLOB my_source_path ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.h FrameBufferTexture.h FrameBufferTexture.cpp)

add_library(
        native-lib
        SHARED
        ${my_source_path})

find_library(
        log-lib
        log)

target_link_libraries(
        native-lib
        EGL # EGL设置Surface
        GLESv3 #OpenGL渲染屏幕
        android #渲染屏幕
        ${log-lib})