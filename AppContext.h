//
// Created by Administrator on 2025/3/30.
//

/**
 * @brief 中介类,避免直接让GLCore成为单例类
 */

#ifndef YOSUGA_APPCONTEXT_H
#define YOSUGA_APPCONTEXT_H

#include "GLCore.h"

class AppContext {
public:
    static void RegisterGLCore(GLCore* core) { s_glCore = core; }
    static void UnregisterGLCore() { s_glCore = nullptr; }
    static GLCore* GetGLCore() { return s_glCore; }

private:
    static inline GLCore* s_glCore = nullptr; // C++17 内联静态成员
};

#endif //YOSUGA_APPCONTEXT_H
