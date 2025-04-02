//
// Created by Administrator on 2025/3/4.
//

#include "AudioPage.h"


#include <QHBoxLayout>

#include "ElaComboBox.h"
#include "ElaPlainTextEdit.h"
#include "ElaProgressBar.h"
#include "ElaScrollPageArea.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "ElaMessageBar.h"

#include "AudioInput.h"
#include "AudioOutput.h"
#include "TextRenderer.h"
#include "LAppLive2DManager.hpp"
AudioPage::AudioPage(QWidget* parent)
        : BasePage(parent)
{
    // 预览窗口标题
    setWindowTitle("AudioPage");

    audioInputDeviceComboBox = new ElaComboBox(this);
    audioInputDeviceComboBox->setToolTip("选择可用的录音设备");
    QStringList comboList = AudioInput::getAvailableAudioInputDevices();

    audioInputDeviceComboBox->addItems(comboList);
    ElaScrollPageArea* comboBoxArea = new ElaScrollPageArea(this);
    QHBoxLayout* comboBoxLayout = new QHBoxLayout(comboBoxArea);
    ElaText* comboBoxText = new ElaText("录音设备", this);
    comboBoxText->setTextPixelSize(15);
    comboBoxLayout->addWidget(comboBoxText);
    comboBoxLayout->addWidget(audioInputDeviceComboBox);
    comboBoxLayout->addStretch();
    comboBoxLayout->addSpacing(10);
    connect(audioInputDeviceComboBox, &ElaComboBox::currentTextChanged, [this](const QString& text) {
        AudioInput::getInstance()->setAudioInputDevice(text);
        ElaMessageBar::success(ElaMessageBarType::TopRight, "音频设置", "成功设置 " + text + " 为当前录音设备", 800.0, this);
    });


    audioInputSpinBox = new ElaSpinBox(this);   // SpinBox
    audioInputSpinBox->setRange(0, 10000);
    audioInputProgressBar = new ElaProgressBar(this);
    audioInputProgressBar->setRange(0, 10000);
    // 关闭ProgressBar的百分比显示
    audioInputProgressBar->setTextVisible(false);
    // 将SpinBox和ProgressBar的数值相互绑定
    connect(audioInputSpinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), [this](int value) {
        audioInputProgressBar->setValue(value);
    });
    connect(audioInputProgressBar, QOverload<int>::of(&ElaProgressBar::valueChanged), [this](int value) {
        audioInputSpinBox->setValue(value);
    });
    // 绑定实时录音阈值到ProgressBar，同时归一到0~1000范围内
    connect(AudioInput::getInstance(), &AudioInput::rmsRealValue, [this](qreal value) {
        audioInputProgressBar->setValue(value);
    });
    // 当计算完成最优阈值
    connect(AudioInput::getInstance(), &AudioInput::thresholdCalculated, [this](qreal value) {
        ElaMessageBar::success(ElaMessageBarType::TopRight, "音频设置", "自动计算出的最优阈值为：" + QString::number(value), 1000, this);
        // AudioInput会自动设置计算出的最优阈值
    });
    audioAutoThresholdStartButton = new ElaPushButton("自动最优阈值", this);
    audioAutoThresholdStartButton->setToolTip("点击后保持当前环境音5秒，自动计算出最合适的静音检测阈值");
    connect(audioAutoThresholdStartButton, &ElaPushButton::clicked, [this]() {
        AudioInput::getInstance()->startAutoThresholdClu(5000);
    });
    audioManualThresholdStartButton = new ElaPushButton("手动设置阈值", this);
    audioManualThresholdStartButton->setToolTip("如果你觉得自动计算的不准的话");
    connect(audioManualThresholdStartButton, &ElaPushButton::clicked, [this]() {
        AudioInput::getInstance()->setSilenceThreshold(audioInputSpinBox->value());
        ElaMessageBar::success(ElaMessageBarType::TopRight, "音频设置", "手动设置的阈值为：" + QString::number(audioInputSpinBox->value()), 1000, this);
    });

    ElaScrollPageArea* audioInputProgressBarArea = new ElaScrollPageArea(this);
    QHBoxLayout* audioInputProgressBarLayout = new QHBoxLayout(audioInputProgressBarArea);
    ElaText* audioInputProgressBarText = new ElaText("录音阈值", this);
    audioInputProgressBarText->setTextPixelSize(15);
    audioInputProgressBarLayout->addWidget(audioInputProgressBarText);
    audioInputProgressBarLayout->addWidget(audioInputProgressBar, 1);
    audioInputProgressBarLayout->addWidget(audioInputSpinBox);
    audioInputProgressBarLayout->addStretch();  // 添加弹性空间将后续控件推到右侧
    audioInputProgressBarLayout->addWidget(audioAutoThresholdStartButton);
    audioInputProgressBarLayout->addWidget(audioManualThresholdStartButton);
    audioInputProgressBarLayout->addStretch();
    audioInputProgressBarLayout->addSpacing(10);

    testAudioPlayButton = new ElaPushButton("播放测试", this);
    testAudioPlayButton->setToolTip("播放一段测试音频来检测播放功能是否正常,注意观察模型嘴唇以及文字下落动画");
    ElaScrollPageArea* testAudioArea = new ElaScrollPageArea(this);
    QHBoxLayout* testAudioLayout = new QHBoxLayout(testAudioArea);
    ElaText* testAudioText = new ElaText("测试", this);
    testAudioText->setTextPixelSize(15);
    testAudioLayout->addWidget(testAudioText);
    testAudioLayout->addStretch();  // 添加弹性空间将后续控件推到右侧
    testAudioLayout->addWidget(testAudioPlayButton);
    testAudioLayout->addSpacing(10);
    connect(testAudioPlayButton, &ElaPushButton::clicked, [this]() {
        const QString text = "あれアイリーじゃないよ!急にいなくなるからどこに行ったのかと思えば~";
        const float duration = 6.0f; // 音频时长
        TextRenderer::getInstance()->addText(text, 40.0f, QColor("#FF69B4"), duration);
        LAppLive2DManager::GetInstance()->StartLipSync("test.wav");
        AudioOutput::getInstance()->setAudioPath("test.wav");
        AudioOutput::getInstance()->playAudio();
    });


    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("音频设置");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addWidget(comboBoxArea);
    centerLayout->addWidget(audioInputProgressBarArea);
    centerLayout->addWidget(testAudioArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

}

AudioPage::~AudioPage()
{
}