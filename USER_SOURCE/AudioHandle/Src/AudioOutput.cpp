//
// Created by Administrator on 2025/1/17.
//

#include "AudioOutput.h"

AudioOutput::AudioOutput(QObject *parent) : QMediaPlayer(parent)
{
//    thread = new QThread(this);
    audioBuffer = new QBuffer(this); // 初始化缓冲区
//    // 将对象移动到子线程中
//    this->moveToThread(thread);

    // 监听媒体状态变化
    connect(this, &QMediaPlayer::mediaStatusChanged, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            emit playbackFinished(); // 触发播放完成信号
        }
    });
    // 连接线程的 started 信号到 initialize 方法
//    connect(thread, &QThread::started, this, &AudioOutput::initialize);

    // 启动子线程
//    thread->start();
}


AudioOutput::~AudioOutput()
{
    // 停止播放
    this->stopAudio();
    delete audioBuffer; // 释放缓冲区

//    // 退出子线程
//    thread->quit();
//    thread->wait();

    // 释放资源
//    delete thread;
}

/**
 * 设置音频文件路径
 * @param path
 */
void AudioOutput::setAudioPath(const QString &path)
{
    this->url = QUrl::fromLocalFile(path);
}

/**
 * 播放音频
 */
void AudioOutput::playAudio()
{
    if(this->url.isEmpty()){
        return;
    }
    this->setMedia(this->url);
    this->play();
}

/**
 * 从字节数组中播放音频
 * @param wavData
 */
void AudioOutput::playFromByteArray(const QByteArray &wavData) {
    // 确保数据有效
    if (wavData.isEmpty()) {
        qWarning() << "尝试播放空音频数据";
        return;
    }

    // 重置缓冲区
    audioBuffer->close();
    audioBuffer->setData(wavData);
    audioBuffer->open(QIODevice::ReadOnly);

    // 设置媒体源并播放
    this->setMedia(QMediaContent(), audioBuffer);
    this->play();
}

/**
 * 暂停播放
 */
void AudioOutput::pauseAudio()
{
    this->pause();
}

/**
 * 停止播放
 */
void AudioOutput::stopAudio()
{
    this->stop();
}

/**
 * 设置播放速度
 * @param double
 */
void AudioOutput::setPlaySpeed(double speed)
{
    this->setPlaybackRate(speed);
}


/**
 * 获取播放速度
 * @return double
 */
double AudioOutput::getPlaySpeed()
{
    return this->playbackRate();
}


/**
 * 设置播放音量
 * @param int
 */
void AudioOutput::setPlayVolume(int volume)
{
    this->setVolume(volume);
}

/**
 * 获取播放音量
 * @return int
 */
int AudioOutput::getPlayVolume()
{
    return this->volume();
}

/**
 * 获取当前播放位置（毫秒）
 * @return long long
 */
qint64 AudioOutput::getPlayPosition()
{
    // 首先要确定当前音频是否是正在播放
    if(this->state() != QMediaPlayer::PlayingState){
        return 0;
    }
    return this->position();
}

/**
 * 获取音频时长（毫秒）
 * @return long long
 */
qint64 AudioOutput::getMediaDuration()
{
    if(this->state() != QMediaPlayer::PlayingState){
        return 0;
    }
    return this->duration();
}

/**
 * 根据当前播放位置，返回播放进度百分比
 * @return double
 */
double AudioOutput::getPlayProgress()
{
    if(this->state() != QMediaPlayer::PlayingState){
        return 0;
    }
    return static_cast<double>(this->getPlayPosition()) / static_cast<double>(this->getMediaDuration());
}


/**
 * 获取播放状态
 * QMediaPlayer::StoppedState、QMediaPlayer::PlayingState、QMediaPlayer::PausedState
 * @return QMediaPlayer::State
 */
QMediaPlayer::State AudioOutput::getState()
{
    return this->state();
}

/**
 * 获取错误类型
 *  NoError,
    ResourceError,
    FormatError,
    NetworkError,
    AccessDeniedError,
    ServiceMissingError,
    MediaIsPlaylist
 * @return QMediaPlayer::Error
 */
QMediaPlayer::Error AudioOutput::getError()
{
    return this->error();
}

/**
 * 获取错误描述
 * @return QString
 */
QString AudioOutput::getErrorString()
{
    return this->errorString();
}


/**
 * 子线程创建时调用二次初始化方法
 */
void AudioOutput::initialize()
{
    // 进行一些设置之类的
}