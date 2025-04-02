#include "Menu.h"
#include "ElaTheme.h"
#include "LAppLive2DManager.hpp"
#include <QDebug>
#include <QTimer>

#include "TextRenderer.h"
#include "AudioInput.h"
#include "AudioOutput.h"

Menu::Menu(QWidget *parent)
        : ElaMenu(parent)
{
    // 设置默认主题
    eTheme->setThemeMode(ElaThemeType::Dark);

    createMenu();
}

Menu::~Menu()
{

}

void Menu::createMenu()
{
    toggleThe = addAction("切换主题");

    // 添加播放音频按钮
    startPlayAction = addAction("播放音频(测试)");


    // 添加WebSocket功能模块按钮
    socketAction = addAction("Socket(开启)");
    socketCloseAction = addAction("Socket(关闭)");
    sendFileAction = addAction("发送文件");

    // 连续对话功能按钮
    startExchangeAction = addAction("连续对话(测试)");

    // 添加设置按钮
    settingsAction = addAction("设置");

    // 添加关闭按钮
    closeAction = addAction("关闭");

    // 连接信号与槽

    // 切换主题按钮
    connect(toggleThe, &QAction::triggered, this, [this]() {
        toggleTheme();
    });


    // 播放音频按钮
    connect(startPlayAction, &QAction::triggered, this, [this]() {
        qDebug() << "Play Audio triggered";

        emit startPlay();   // 发射播放音频信号
    });




    // Socket功能模块按钮
    connect(socketAction, &QAction::triggered, this, [this]() {
        SocketManager::getInstance()->setIp("127.0.0.1");
        SocketManager::getInstance()->setPort(12345);
        SocketManager::getInstance()->connectToServer();

//        // 5秒后关闭socket连接
//        QTimer::singleShot(5000, [this]() {
//            socketManager.disconnectFromServer();
//        });
    });

    connect(socketCloseAction, &QAction::triggered, this, [this]() {
        SocketManager::getInstance()->disconnectFromServer();
    });

    connect(sendFileAction, &QAction::triggered, this, [this]() {
        SocketManager::getInstance()->sendWavFile(QString("test.wav"));
    });

    // TODO 连续对话功能
    connect(startExchangeAction, &QAction::triggered, this, [this]() {
        // 创建startExchange函数子线程执行
        startExchange();
    });


    // 设置按钮
    connect(settingsAction, &QAction::triggered, this, [this]() {
        qDebug() << "Settings triggered";
        // 打开设置窗口

        // 如果 Setting 窗口已经存在，则不再创建
        if (settingWindow) {
            settingWindow->show();
            return;
        }

        // 动态创建 Setting 窗口
        settingWindow.reset(new Setting()); // 使用智能指针管理

        // 显示 Setting 窗口
        settingWindow->show();
    });

    // 关闭
    connect(closeAction, &QAction::triggered, this, [this]() {
        emit closeMainWindow();  // 发射关闭信号
    });
}

void Menu::showMenu(const QPoint &pos)
{
    // 在指定位置显示菜单
    exec(pos);
}

void Menu::toggleTheme()
{
    if (eTheme->getThemeMode() == ElaThemeType::Light) {
        eTheme->setThemeMode(ElaThemeType::Dark);
    } else {
        eTheme->setThemeMode(ElaThemeType::Light);
    }
}

void Menu::startExchange()
{
    // 列出所有音频输入设备
    QList<QString> devices = AudioInput::getAvailableAudioInputDevices();
    qDebug() << "可用录音设备:";
    for (const QString &device : devices) {
        qDebug() << device;
    }

    // 设置当前录音设备（假设选择第一个设备）
    if (!devices.isEmpty()) {
        qDebug() << "选择的录音设备是: " << devices.first();
        AudioInput::getInstance()->setAudioInputDevice(devices.first());
        qDebug() << "当前录音设备: " << AudioInput::getInstance()->audioInput();  // 检查当前录音设备
    }

    // 第一次需要主动录音来启动 信号与槽状态机(FSM)
    qDebug() << "开始录音";
    AudioInput::getInstance()->startAutoStopAudio();

    // 检查录音状态
    if (AudioInput::getInstance()->state() == QMediaRecorder::RecordingState) {
        qDebug() << "录音已启动";
    } else {
        qDebug() << "录音启动失败";
    }


    // 连接信号和槽
    // 播放回答
    // 当完整接受wav文件后播放相关的wav文件
    connect(SocketManager::getInstance(), &SocketManager::revWavFileFinish, [this](const QString &filePath, const QString &response, const float duration) {
        LAppLive2DManager::GetInstance()->StartLipSync(filePath.toUtf8().constData());
        AudioOutput::getInstance()->setAudioPath(filePath);
        AudioOutput::getInstance()->playAudio();
        TextRenderer::getInstance()->addText(response, 40.0f, QColor("#FF69B4"), duration);
    });



    // 开始录音
    // 当播放完成后继续开始录音
    connect(AudioOutput::getInstance(), &AudioOutput::playbackFinished, [this]() {
        qDebug() << "开始录音";
        AudioInput::getInstance()->startAutoStopAudio();

        // 检查录音状态
        if (AudioInput::getInstance()->state() == QMediaRecorder::RecordingState) {
            qDebug() << "录音已启动";
        } else {
            qDebug() << "录音启动失败";
        }
    });

    // 上传录音
    // 当录音完成时，发送wav文件
    connect(AudioInput::getInstance(), &AudioInput::recordingFinished_Byte, [this](const QByteArray &wavData) {
        qDebug() << "录音完成，开始上传录音文件...";
        if (wavData.isEmpty()) {
            qWarning() << "录音数据为空！";
            return;
        }

        qDebug() << "准备发送WAV数据，大小:" << wavData.size() << "字节";

        SocketManager::getInstance()->sendWavFile(wavData);
    });

    /**
     * 见连接的信号与槽函数
     */

}