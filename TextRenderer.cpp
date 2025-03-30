//
// Created by Administrator on 2025/2/16.
//
/**
 * 用于渲染文本显示
 */

#include "TextRenderer.h"
#include <QPainter>
#include <QOpenGLPaintDevice>
#include <cmath>
#include <QRandomGenerator>
TextRenderer *TextRenderer::instance = nullptr;
TextRenderer::TextRenderer() :  windowWidth(800),           // 默认窗口宽
                                windowHeight(600),          // 默认窗口高
                                lastFrameTime(0),
                                defaultHoldDuration(0.5f),  // 默认停留0.5秒
                                gravity(980.0f),           // 默认重力
                                dampFactor(0.82f)          // 默认阻尼
{
    globalTimer.start();
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(28);
    font.setWeight(QFont::Bold);
}

void TextRenderer::setGlobalFont(const QFont &newFont)
{
    font = newFont;
    font.setWeight(QFont::Bold);
}

void TextRenderer::setWindowSize(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
}

void TextRenderer::addText(const QString &text, float yPos,
                           const QColor &color, float duration)
{
    TextInstance instance;
    instance.text = text;
    instance.basePosition = QVector2D(0, yPos);
    instance.primaryColor = color;
    instance.outlineColor = QColor(0, 0, 0, 180);
    instance.duration = duration;
    instance.startTime = globalTimer.elapsed();

    instance.holdDuration = defaultHoldDuration;  // 应用当前全局设置

    QFontMetrics metrics(font);
    instance.charWidths.clear();
    for (const QChar &ch : text) {
        instance.charWidths.append(metrics.horizontalAdvance(ch));
    }

    // 初始位置计算
    updateFlowPositions(instance);
    activeTexts.append(instance);
}

void TextRenderer::updateFlowPositions(TextInstance &instance)
{
    QFontMetrics metrics(font);
    const int rightMargin = 20; // 右侧留白

    // 计算可见部分总宽度
    int visibleWidth = 0;
    for (int i = 0; i < instance.visibleChars; ++i) {
        visibleWidth += instance.charWidths[i];
    }

    // 动态计算起始位置
    int startX = qMin(
            windowWidth - visibleWidth - rightMargin, // 优先保证右侧空间
            (windowWidth - visibleWidth) / 2          // 次选居中显示
    );

    // 边界保护：至少保留20px左侧边距
    startX = qMax(20, startX);

    // 更新字符位置
    int currentX = startX;
    instance.charPositions.clear();

    for (int i = 0; i < instance.text.size(); ++i) {
        if (i < instance.visibleChars) {
            instance.charPositions.append(QPoint(currentX, instance.basePosition.y()));
            currentX += instance.charWidths[i];
        } else {
            instance.charPositions.append(QPoint(-10000, -10000));
        }
    }

    // 自动滚动调整：当文字溢出时整体左移
    if (currentX > windowWidth - rightMargin) {
        int overflow = currentX - (windowWidth - rightMargin);
        for (QPoint &pos : instance.charPositions) {
            if (pos.x() != -10000) {
                pos.rx() -= overflow;
            }
        }
    }
}

void TextRenderer::updateDropPositions(TextInstance &instance, float deltaTime)
{
    const float floorY = windowHeight - 30;

    instance.dropYVelocity += gravity * deltaTime;  // 使用全局重力值
    float deltaY = instance.dropYVelocity * deltaTime;

    bool hasCollision = false;

    for (QPoint &pos : instance.charPositions) {
        float newY = pos.y() + deltaY;

        if (newY >= floorY) {
            newY = floorY;
            instance.dropYVelocity = -qAbs(instance.dropYVelocity) * dampFactor;  // 使用全局阻尼系数
            hasCollision = true;
            pos.rx() += QRandomGenerator::global()->bounded(-3, 4);
        }
        pos.setY(newY);
    }

    // 碰撞后处理
    if (hasCollision) {
        // 速度衰减到临界值时开始加速透明
        if (qAbs(instance.dropYVelocity) < 100.0f) {
            instance.alpha *= 0.92f;  // 加快透明度衰减速度
        }

        // 完全静止后强制移除
        if (qAbs(instance.dropYVelocity) < 5.0f) {
            instance.alpha = 0.0f;
        }
    }

    // 常规透明度衰减
    instance.alpha = qMax(0.0f, instance.alpha * 0.98f);

    // 添加随机水平扰动（只在有速度时）
    if (qAbs(instance.dropYVelocity) > 10.0f) {
        for (QPoint &pos : instance.charPositions) {
            pos.rx() += QRandomGenerator::global()->bounded(-1, 2);
        }
    }
}

void TextRenderer::update()
{
    qint64 currentTime = globalTimer.elapsed();
    float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentTime;

    auto it = activeTexts.begin();

    while (it != activeTexts.end()) {
        TextInstance &instance = *it;

        if (instance.isDropping) {
            updateDropPositions(instance, deltaTime); // 使用统一的deltaTime

            // 强化消失条件：Y轴速度接近零 或 透明度低于阈值
            if ((qAbs(instance.dropYVelocity) < 5.0f && instance.alpha < 0.3f)
                || instance.alpha < 0.01f) {
                it = activeTexts.erase(it);
                continue;
            }
        } else {
            // 流式显示更新
            float progress = (currentTime - instance.startTime) / 1000.0f / instance.duration;
            progress = qMin(progress, 1.0f);

            if (!instance.flowCompleted) {
                int newVisible = qMin(instance.text.size(),
                                      static_cast<int>(progress * instance.text.size()));

                if (newVisible != instance.visibleChars) {
                    instance.visibleChars = newVisible;
                    updateFlowPositions(instance);
                }

                // 流式显示完成检测
                if (progress >= 1.0f) {
                    instance.flowCompleted = true;
                    instance.flowEndTime = currentTime;  // 记录完成时间
                }
            }
                // 已流式完成但未开始下坠
            else if (!instance.isDropping) {
                // 检查停留时间是否结束
                if (currentTime - instance.flowEndTime >= instance.holdDuration * 1000) {
                    instance.isDropping = true;
                    instance.dropStartTime = currentTime;

                    // 最终居中定位
                    int totalWidth = 0;
                    for (int w : instance.charWidths) totalWidth += w;
                    int startX = (windowWidth - totalWidth) / 2;
                    int currentX = startX;

                    instance.charPositions.clear();
                    for (int i = 0; i < instance.text.size(); ++i) {
                        instance.charPositions.append(QPoint(currentX, instance.basePosition.y()));
                        currentX += instance.charWidths[i];
                    }
                }
            }
        }
        ++it;
    }
}

void TextRenderer::render()
{
    QOpenGLPaintDevice device(windowWidth, windowHeight);
    QPainter painter(&device);
    painter.setFont(font);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    for (const auto &instance : activeTexts) {
        QColor mainColor = instance.primaryColor;
        mainColor.setAlphaF(instance.alpha);
        QColor outlineColor = instance.outlineColor;
        outlineColor.setAlphaF(instance.alpha * 0.7f);

        for (int i = 0; i < instance.charPositions.size(); ++i) {
            const QPoint &pos = instance.charPositions[i];
            if (pos.x() < -9999) continue; // 跳过隐藏字符

            // 绘制轮廓
            painter.setPen(outlineColor);
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;
                    painter.drawText(pos + QPoint(dx, dy), QString(instance.text[i]));
                }
            }

            // 绘制主体
            painter.setPen(mainColor);
            painter.drawText(pos, QString(instance.text[i]));
        }
    }
}
