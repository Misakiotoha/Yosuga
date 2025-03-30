//
// Created by Administrator on 2025/1/17.
//

#ifndef AIRI_DESKTOPGRIL_AUDIOINPUT_H
#define AIRI_DESKTOPGRIL_AUDIOINPUT_H

#include <QAudioRecorder>
#include <QAudioEncoderSettings>
#include <QUrl>
#include <QMediaRecorder>
#include <QTimer>
#include <QAudioProbe>
#include <QAudioBuffer>
#include <QAudioDeviceInfo>
#include <QList>


/**
 * @brief  录音模块
 * @author Misaki
 *
 */
class AudioInput : public QAudioRecorder
{
Q_OBJECT


public:

    /**
     * @brief  构造函数
     * @param parent
     */
    AudioInput(QObject *parent = nullptr);

    /**
     * @brief  析构函数
     */
    ~AudioInput();

    /**
     * @brief  设置录音音频音质 \n
     * 几种可选的编码类型 \n
     * codec: 设置音频编码格式。常见的编码格式包括：\n
     *      audio/pcm：未压缩的音频数据，通常用于 WAV 文件。 \n
            audio/amr：自适应多速率编码，常用于 AMR 文件。 \n
            audio/aac：高级音频编码，常用于 AAC 文件。 \n
            audio/mp3：MP3 编码，常用于 MP3 文件。 \n
       rate: 设置音频的采样率，单位是赫兹（Hz）。常见的采样率包括： \n
            8000 Hz：低质量音频，适合语音通信。 \n
            11025 Hz：较低质量音频。 \n
            22050 Hz：中等质量音频。 \n
            44100 Hz：CD 质量音频。 \n
            48000 Hz：高质量音频，常用于视频和专业音频应用。 \n
       channels: 设置音频的通道数。常见的通道数包括： \n
            1：单声道（Mono） \n
            2：双声道（Stereo） \n
        bitrate: 设置音频的比特率，单位是比特每秒（bps）。常见的比特率包括： \n
            128000 bps：中等质量音频。 \n
            192000 bps：较高质量音频。 \n
            320000 bps：高质量音频。 \n
        quality: 设置音频的质量。常见的质量级别包括： \n
            QMultimedia::VeryLowQuality \n
            QMultimedia::LowQuality \n
            QMultimedia::NormalQuality \n
            QMultimedia::HighQuality \n
            QMultimedia::VeryHighQuality \n
        mode: 设置音频的编码模式。常见的编码模式包括： \n
            QMultimedia::ConstantBitRateEncoding：恒定比特率编码。 \n
            QMultimedia::VariableBitRateEncoding：可变比特率编码。 \n
            QMultimedia::AverageBitRateEncoding：平均比特率编码。 \n
        \n
        默认为: \n
            使用 PCM 编码 \n
            采样率 44.1 kHz \n
            单声道 \n
            比特率 1411200 bps (44100 * 2 * 16) \n
            高质量 \n
            恒定比特率编码 \n
     *
     *
     * @param codec     编码类型
     * @param rate      采样率
     * @param channels  声道
     * @param bitrate   比特率
     * @param quality   质量
     * @param mode      比特率编码模式
     */
    void setAudioSettings(const QString& codec ="audio/pcm", int rate = 44100, int channels = 2, int bitrate = 1411200, QMultimedia::EncodingQuality quality = QMultimedia::HighQuality, QMultimedia::EncodingMode mode = QMultimedia::ConstantBitRateEncoding);

    /**
     * @brief  设置录音文件输出路径与文件名
     * @param path       输出路径
     * @param fileName   文件名
     */
    void setAudioPath(const QString &path, const QString &fileName);

    /**
     * @brief  开始录音
     */
    void startAudio();

    /**
     * @brief  停止录音
     */
    void stopAudio();

    /**
     * @brief 设置录音时间并开始录音
     * @param duration 录音时长，单位为秒
     */
    void startAudioWithDuration(int duration);

    /**
     * @brief 开始自动录音，根据声音判断是否停止
     * @param silenceThreshold 静音阈值，低于该值则认为没有声音
     * @param silenceDuration 静音持续时间，单位为毫秒
     */
    void startAutoStopAudio(qreal silenceThreshold = 1200, int silenceDuration = 1500);

    /**
     * @brief 获取当前系统所有的音频输入设备
     * @return 音频输入设备名称列表
     */
    static QList<QString> getAvailableAudioInputDevices();

    /**
     * @brief 设置当前录音设备
     * @param deviceName 设备名称
     */
    void setAudioInputDevice(const QString &deviceName);

private:
    // WAV头生成工具函数
    QByteArray generateWavHeader(quint32 dataSize) const;

signals:
    // 录音完成信号
    void recordingFinished();
    void recordingFinished_Byte(const QByteArray &wavData);

private slots:
    void onTimeout();  // 定时器超时槽函数
    void processBuffer(const QAudioBuffer& buffer); // 处理缓冲区数据


private:
    QAudioEncoderSettings *audioSettings;   /// 音频设置
    QTimer *timer;                          /// 定时器
    QAudioProbe *probe;                     /// 音频探测
    QTimer *silenceTimer;                   /// 静音检测定时器
    qreal silenceThreshold;                 /// 静音阈值
    int silenceDuration;                    /// 静音持续时间

    QByteArray rawPCMData;                  /// 存储原始PCM数据
    QAudioFormat audioFormat;               /// 保存音频格式信息
};


#endif //AIRI_DESKTOPGRIL_AUDIOINPUT_H
