//
// Created by Administrator on 2025/3/30.
//

/**
 * @brief 中介类,避免直接让GLCore成为单例类
 * 虽然变得方便了，但也带来了危险，如果你肆意通过中介指针去调用GLCore的成员函数
 * 可能会导致渲染问题等
 */

#ifndef YOSUGA_APPCONTEXT_H
#define YOSUGA_APPCONTEXT_H

#include "GLCore.h"

class AppContext {
public:
    // 注册GLCore
    static void RegisterGLCore(GLCore* core) { s_glCore = core; }
    // 注销GLCore
    static void UnregisterGLCore() { s_glCore = nullptr; }
    static GLCore* GetGLCore() { return s_glCore; }

private:
    static inline GLCore* s_glCore = nullptr; // C++17 内联静态成员
};

#endif //YOSUGA_APPCONTEXT_H
