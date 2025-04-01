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
AudioPage::AudioPage(QWidget* parent)
        : BasePage(parent)
{
    // 预览窗口标题
    setWindowTitle("AudioPage");

    audioInputDeviceComboBox = new ElaComboBox(this);
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
    audioInputSpinBox->setRange(0, 1000);
    audioInputProgressBar = new ElaProgressBar(this);
    audioInputProgressBar->setRange(0, 1000);
    // 关闭ProgressBar的百分比显示
    audioInputProgressBar->setTextVisible(false);
    // 将SpinBox和ProgressBar的数值相互绑定
    connect(audioInputSpinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), [this](int value) {
        audioInputProgressBar->setValue(value);
    });
    connect(audioInputProgressBar, QOverload<int>::of(&ElaProgressBar::valueChanged), [this](int value) {
        audioInputSpinBox->setValue(value);
    });

    ElaScrollPageArea* audioInputProgressBarArea = new ElaScrollPageArea(this);
    QHBoxLayout* audioInputProgressBarLayout = new QHBoxLayout(audioInputProgressBarArea);
    ElaText* audioInputProgressBarText = new ElaText("录音阈值", this);
    audioInputProgressBarText->setTextPixelSize(15);
    audioInputProgressBarLayout->addWidget(audioInputProgressBarText);
    audioInputProgressBarLayout->addWidget(audioInputProgressBar);
    audioInputProgressBarLayout->addWidget(audioInputSpinBox);
    audioInputProgressBarLayout->addStretch();


    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("音频设置");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addWidget(comboBoxArea);
    centerLayout->addWidget(audioInputProgressBarArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

}

AudioPage::~AudioPage()
{
}

void AudioPage::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button())
    {
        case Qt::LeftButton:
        {
            //ElaMessageBar::success(ElaMessageBarType::TopRight, "Success", "Never Close Your Eyes", 2500);
            //ElaMessageBar::success(ElaMessageBarType::TopRight, "Success", "Never Close Your Eyes", 1500);
            break;
        }
        case Qt::BackButton:
        {
            this->navigation(0);
            break;
        }
        case Qt::ForwardButton:
        {
            this->navigation(1);
            break;
        }
        case Qt::MiddleButton:
        {
            this->navigation(2);
            break;
        }
        default:
        {
            break;
        }
    }
    ElaScrollPage::mouseReleaseEvent(event);
}