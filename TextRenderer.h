//
// Created by Administrator on 2025/2/16.
//

#ifndef AIRI_DESKTOPGRIL_TEXTRENDERER_H
#define AIRI_DESKTOPGRIL_TEXTRENDERER_H

#include <QOpenGLWidget>
#include <QVector>
#include <QString>
#include <QColor>
#include <QElapsedTimer>
#include <QVector2D>
#include <QFontMetrics>
#include <QLinearGradient>

class TextRenderer {
public:
    // 删除拷贝构造函数和赋值运算符
    TextRenderer(const TextRenderer&) = delete;
    void operator=(const TextRenderer&) = delete;
    // 获取单例实例
    static TextRenderer* getInstance()
    {
        if(instance == nullptr){
            instance = new TextRenderer();
        }
        return instance;
    }

    struct TextInstance {
        QString text;                // 文本内容
        QVector2D basePosition;      // 基础位置（Y轴）
        QColor primaryColor;         // 主要文字颜色
        QColor outlineColor;         // 轮廓颜色
        float duration;              // 显示总时长（秒）
        qint64 startTime;            // 开始显示时间（毫秒）
        bool isDropping;             // 是否正在下坠
        qint64 dropStartTime;        // 下坠开始时间
        float dropYVelocity;         // Y轴下落速度
        float alpha;                 // 透明度
        QList<QPoint> charPositions; // 字符位置
        QList<int> charWidths;       // 每个字符宽度
        int visibleChars;            // 可见字符数
        bool flowCompleted;          // 流式显示是否完成
        float holdDuration;          // 实际使用的停留时间
        qint64 flowEndTime;          // 流式完成时间戳

        TextInstance() : isDropping(false), flowCompleted(false),
                         dropYVelocity(0.0f), alpha(1.0f),
                         visibleChars(0), holdDuration(0.5f),
                         flowEndTime(0) {}
    };


    void setWindowSize(int w, int h);
    void addText(const QString &text, float yPos,
                 const QColor &color, float duration);
    void update();
    void render();
    void setGlobalFont(const QFont &newFont);

    /**
     * 参数建议值：
        效果类型	    gravity	    dampFactor	holdDuration
        柔和下落	    600.0f	    0.85f	    1.0f
        快速坠落	    1200.0f	    0.6f	    0.3f
        弹性效果	    900.0f	    0.75f	    0.8f
        真实物理模拟	980.0f	    0.82f	    0.5f
     */
    void setHoldDuration(float seconds) { defaultHoldDuration = seconds; }
    void setGravity(float g) { gravity = g; }
    void setDampFactor(float damp) { dampFactor = damp; }

private:
    explicit TextRenderer();  // 构造函数私有化
    void updateFlowPositions(TextInstance &instance);
    void updateDropPositions(TextInstance &instance, float deltaTime);

private:
    static TextRenderer *instance;

    QList<TextInstance> activeTexts;    /// 当前显示的文字
    QElapsedTimer globalTimer;          /// 全局计时器
    QFont font;                         /// 全局字体
    int windowWidth;                    /// 窗口宽度
    int windowHeight;                   /// 窗口高度
    qint64 lastFrameTime;               /// 上一帧的时间

    // 一些自定义参数
    float defaultHoldDuration;          /// 默认停留时间（秒）
    float gravity;                      /// 重力加速度（像素/秒²）
    float dampFactor;                   /// 碰撞阻尼系数
};

#endif //AIRI_DESKTOPGRIL_TEXTRENDERER_H
