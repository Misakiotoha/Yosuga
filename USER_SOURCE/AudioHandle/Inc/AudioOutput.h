//
// Created by Administrator on 2025/1/17.
//

#ifndef AIRI_DESKTOPGRIL_AUDIOOUTPUT_H
#define AIRI_DESKTOPGRIL_AUDIOOUTPUT_H

#include <QMediaPlayer>
#include <QUrl>
#include <QThread>
#include <QBuffer>

/**
 * @brief 音频播放模块
 * @author Misaki
 * 本模块基于Qt5当中的Multimedia中的QMediaPlayer模块二次封装了一些用于音频播放的操作
 * 实现的功能
 * 1. 设定传入的音频文件路径
 * 2. 根据音频文件路径播放音频
 * 将上面的两个函数封装成一个槽函数，以及设定一个对应的信号
 *
 *  QMediaPlayer 提供了多个信号，用于通知状态变化：
        stateChanged(QMediaPlayer::State state)：播放状态变化时触发。
        mediaStatusChanged(QMediaPlayer::MediaStatus status)：媒体状态变化时触发。
        positionChanged(qint64 position)：播放位置变化时触发。
        durationChanged(qint64 duration)：媒体总时长变化时触发。
        volumeChanged(int volume)：音量变化时触发。
        errorOccurred(QMediaPlayer::Error error, const QString &errorString)：发生错误时触发。
 *
 */
class AudioOutput : public QMediaPlayer
{
Q_OBJECT

public:
    /**
     * 构造函数
     * @param parent
     */
    AudioOutput(QObject *parent = nullptr);

    /**
     * 析构函数
     */
    ~AudioOutput();

    /**
     * 设置音频文件路径
     * @param path
     */
    void setAudioPath(const QString &path);

    /**
     * 播放音频
     */
    void playAudio();

    /**
     * 播放内存中的WAV字节流数据
     * @param wavData 完整的WAV格式字节流
     */
    void playFromByteArray(const QByteArray &wavData);

    /**
     * 暂停播放音频
     */
    void pauseAudio();

    /**
     * 停止播放音频
     */
    void stopAudio();

    /**
     * 设置播放速度
     * @param double
     */
    void setPlaySpeed(double speed);

    /**
     * 获取播放速度
     * @return double
     */
    double getPlaySpeed();

    /**
     * 设置播放音量
     * @param int
     */
    void setPlayVolume(int volume);

    /**
     * 获取播放音量
     * @return int
     */
    int getPlayVolume();

    /**
     * 获取当前播放位置（毫秒）
     * @return long long
     */
    qint64 getPlayPosition();

    /**
     * 获取媒体总时长（毫秒）
     * @return long long
     */
    qint64 getMediaDuration();

    /**
     * 根据当前播放位置，返回播放进度百分比
     * @return double
     */
    double getPlayProgress();


    /**
     * 获取播放状态
     * QMediaPlayer::StoppedState、QMediaPlayer::PlayingState、QMediaPlayer::PausedState
     * @return QMediaPlayer::State
     */
    QMediaPlayer::State getState();

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
    QMediaPlayer::Error getError();

    /**
     * 获取错误描述
     * @return QString
     */
    QString getErrorString();

    /**
     * 子线程创建时调用二次初始化方法
     */
    void initialize();

signals:
    void playbackFinished(); // 播放完成信号

private:
    QUrl url;                   /// <!音频文件路径
    QThread *thread;            /// <!子线程
    QBuffer *audioBuffer;       /// <!存储内存数据
};

#endif //AIRI_DESKTOPGRIL_AUDIOOUTPUT_H
