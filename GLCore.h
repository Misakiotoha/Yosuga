#pragma once

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include "menu.h"
#include "AudioOutput.h"
#include "AudioInput.h"
#include "TextRenderer.h"

//

class GLCore : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLCore(int width, int height, QWidget* parent = nullptr);
    // 删除拷贝构造函数
    GLCore(const GLCore&) = delete;
    // 删除拷贝运算符
    GLCore& operator=(const GLCore&) = delete;
    // 删除移动构造函数
    GLCore(GLCore&&) = delete;
    // 删除移动运算符
    GLCore& operator=(GLCore&&) = delete;

    ~GLCore();

    // 帧率控制
    void setFrameRate(double fps);
    double getFrameRate() const;

    void playAudioTest();


    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;


    // 重写函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void closeEvent(QCloseEvent* event) override;

private:
    void closeGL();  // 关闭当前窗口


private:
    /*
     * 在这个地方我遇到一个很抽象的问题，就是私有成员变量顺序的问题
     * 出问题的顺序是:
     *  bool isLeftPressed;             /// 鼠标左键是否按下
        bool isRightPressed;            /// 鼠标右键是否按下
        QPoint currentPos;              /// 当前鼠标位置
        Menu *contextMenu;              /// 使用 Menu 类
        AudioInput *audioInput;         /// 音频录制类
        AudioOutput *audioOutput;       /// 音频播放类
        这样的顺序导致了我的鼠标一放在窗口上，窗口就往右下瞬移
        改成现在下面的顺序就正常了
        我一开始以为是我音频录制类里面多线程导致的
        但想了想我都没new这个对象，哪来的多线程
        后面问了问AI，它的解释是:
            可能与C++中类成员的初始化顺序有关。
            在C++中，类成员变量按照它们在类中声明的顺序进行初始化，
            而不是根据它们在构造函数初始化列表中的顺序。
            如果某些成员变量的初始化依赖于其他成员变量的状态
            ，而它们的实际初始化顺序与预期不符，可能会导致未定义行为或其他意外问题。
        我感觉这不一定是根本原因，谁能告诉我到底发生了啥？？？
     */
    double frameRate = 60.0;
    /// 帧率
    QTimer* frameTimer;             /// 帧控制定时器
    Menu *contextMenu;              /// 使用 Menu 类
    AudioInput *audioInput;         /// 音频录制类
    AudioOutput *audioOutput;       /// 音频播放类
    bool isLeftPressed;             /// 鼠标左键是否按下
    bool isRightPressed;            /// 鼠标右键是否按下
    QPoint currentPos;              /// 当前鼠标位置

};