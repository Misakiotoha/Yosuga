#include "LAppDelegate.hpp" // 必须要放在第一个，不然包老实的
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "GLCore.h"
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QDir>

#include <QFont>
#include <QApplication>
#include <QFontDatabase>

GLCore::GLCore(int w, int h, QWidget *parent)
    : QOpenGLWidget(parent),
      isLeftPressed(false),    // 显式初始化
      isRightPressed(false)    // 显式初始化
{
    // 启用高分辨率位图（High DPI Pixmaps）支持
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
    //根据实际屏幕缩放比例更改
    qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif

    // 不为窗口创建额外的兄弟窗口，从而简化窗口管理并可能提高性能
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    // 设置字体
    QFontDatabase::addApplicationFont("Font/ElaAwesome.ttf");
    QApplication::setFont(QFont("Microsoft YaHei", 13));


    // new一些必要的对象
    contextMenu = new Menu();
    audioOutput = new AudioOutput();
    audioInput = new AudioInput();

    // 设置窗口大小
    setFixedSize(w, h);
    // 设置文本渲染器窗口大小
    TextRenderer::getInstance()->setWindowSize(w, h);
    TextRenderer::getInstance()->setGlobalFont(QFont("Microsoft YaHei", 14, QFont::Bold));
    TextRenderer::getInstance()->setHoldDuration(1.0f);  // 停留1.2秒
    TextRenderer::getInstance()->setGravity(600.0f);     // 更快的下坠速度
    TextRenderer::getInstance()->setDampFactor(0.85f);   // 更强的弹性效果


    //this->setAttribute(Qt::WA_DeleteOnClose);       // 窗口关闭时自动释放内存
    this->setWindowFlag(Qt::FramelessWindowHint); // 设置无边框窗口
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // 设置窗口始终在顶部
    this->setWindowFlag(Qt::Tool); // 隐藏应用程序图标
    this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口背景透明


    // 帧率控制初始化
    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, [=]() {
        update();
    });
    frameTimer->start(static_cast<int>((1.0 / frameRate) * 1000)); // 使用成员变量计算间隔


    // 启用鼠标跟踪，不启用鼠标按下才会回调mouseMoveEvent函数
    this->setMouseTracking(true);

    // 连接一些必要的信号与槽
    connect(contextMenu, &Menu::closeMainWindow, this, &GLCore::closeGL);
    connect(contextMenu, &Menu::startPlay, this, &GLCore::playAudioTest);


}


GLCore::~GLCore()
{

}

void GLCore::playAudioTest()
{
    // 示例：播放6秒音频，显示文字
    const QString text = "はい这是一个流式显示的测试文本，我需要它尽可能的长一些，来测试各种各样的Bug、はい～";
    const QVector2D position(40, 40);
    const QColor color(Qt::cyan);
    const float duration = 6.0f; // 音频时长

    TextRenderer::getInstance()->addText(text, 40.0f, QColor("#FF69B4"), duration);

    LAppLive2DManager::GetInstance()->StartLipSync("test.wav");
    audioOutput->setAudioPath("test.wav");
    audioOutput->playAudio();
}

// 帧率设置
void GLCore::setFrameRate(double fps)
{
    if (qFuzzyCompare(fps, frameRate))  // 避免无意义更新
        return;

    if (fps <= 0.0) {
        qWarning() << "Invalid frame rate:" << fps << "using default 60.0";
        fps = 60.0;
    }

    frameRate = fps;
    frameTimer->setInterval(static_cast<int>((1.0 / frameRate) * 1000));
}

// 获取当前帧率
double GLCore::getFrameRate() const
{
    return frameRate;
}


/**
 * 关闭窗口
 */
void GLCore::closeGL()
{
    this->close();
}

/**
 * 主要是为setWindowFlag(Qt::Tool)这段代码擦屁股。
 * 在 Qt 中，程序的退出通常依赖于主事件循环（QApplication的事件循环）的退出。当主窗口关闭时，通常会触发QApplication的lastWindowClosed信号，从而退出事件循环，导致程序退出。
    然而，当你将窗口设置为工具窗口（Qt::Tool）时，这个窗口可能不会被视为应用程序的“主窗口”，因此关闭它可能不会触发lastWindowClosed信号，导致程序不会正常退出。
 * @param event
 */
void GLCore::closeEvent(QCloseEvent* event)
{
    QApplication::quit();  // 显式退出事件循环
    event->accept();  // 确保关闭事件被接受
}

void GLCore::mouseMoveEvent(QMouseEvent* event)
{
//    qDebug() << "GLCore::mouseMoveEvent" << event->x() << event->y();
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(event->x(), event->y());

    if (isLeftPressed) {
        QPoint newPos = event->globalPos() - currentPos;
        this->move(newPos);
    }

}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(event->x(), event->y());

    if (event->button() == Qt::LeftButton) {
        this->isLeftPressed = true;
        this->currentPos = event->globalPos() - this->frameGeometry().topLeft();
    }
    // TODO: 右键菜单等
    if (event->button() == Qt::RightButton) {

        // 在鼠标右键点击的位置创建菜单，显示自定义右键菜单
        contextMenu->showMenu(event->globalPos());
        this->isRightPressed = true;
    }


}

void GLCore::mouseReleaseEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesEnded(event->x(), event->y());

    if (event->button() == Qt::LeftButton) {
        isLeftPressed = false;
    }
    if (event->button() == Qt::RightButton) {
        isRightPressed = false;
    }

}

void GLCore::initializeGL()
{
    LAppDelegate::GetInstance()->Initialize(this);
    
    // 选择模型
}

void GLCore::paintGL()
{
    LAppDelegate::GetInstance()->update();
    // 渲染文本
    TextRenderer::getInstance()->update();
    TextRenderer::getInstance()->render();
}

void GLCore::resizeGL(int w, int h)
{
    // 设置文本渲染器窗口大小
    TextRenderer::getInstance()->setWindowSize(w, h);

    LAppDelegate::GetInstance()->resize(w, h);
}