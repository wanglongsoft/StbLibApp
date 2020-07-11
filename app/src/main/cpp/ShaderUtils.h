//
// Created by 24909 on 2020/4/1.
//

#ifndef OPENGLSTART_SHADERUTILS_H
#define OPENGLSTART_SHADERUTILS_H

#include "LogUtils.h"
#include <cstring>
#include <string>
#include <android/asset_manager.h>

class ShaderUtils {
public:
    static std::string * openAssetsFile(AAssetManager *mgr, char *file_name);
};


#endif //OPENGLSTART_SHADERUTILS_H
