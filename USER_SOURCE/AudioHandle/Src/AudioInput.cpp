//
// Created by Administrator on 2025/1/17.
//

#include <QtMath>
#include "AudioInput.h"

AudioInput *AudioInput::instance = nullptr;
AudioInput *AudioInput::getInstance()
{
    // 懒汉式
    if (instance == nullptr) {
        instance = new AudioInput();
    }
    return instance;
}

AudioInput::AudioInput(QObject *parent) : QAudioRecorder(parent)
{
    // new一些必要的对象
    // 创建音频设置对象
    audioSettings = new QAudioEncoderSettings();
    timer = new QTimer(this);
    probe = new QAudioProbe(this);
    silenceTimer = new QTimer(this);

    // 连接定时器的 timeout 信号到 onTimeout 方法
    connect(timer, &QTimer::timeout, this, &AudioInput::onTimeout);
    // 连接静音检测定时器的 timeout 信号到 stopAudio 方法
    connect(silenceTimer, &QTimer::timeout, this, &AudioInput::stopAudio);
    // 连接探测器到 processBuffer 方法
    connect(probe, &QAudioProbe::audioBufferProbed, this, &AudioInput::processBuffer);

    // 连接状态变化信号
    connect(this, &QMediaRecorder::stateChanged, this, [this](QMediaRecorder::State state) {
        if (state == QMediaRecorder::StoppedState) {
            emit recordingFinished();
        }
    });

    // 进行一些必要的设置 默认
    this->setAudioSettings();
}

AudioInput::~AudioInput()
{
    // 停止录音
    stopAudio();

    // 释放资源
    delete audioSettings;
}


void AudioInput::setAudioSettings(const QString& codec, int rate, int channels, int bitrate, QMultimedia::EncodingQuality quality, QMultimedia::EncodingMode mode)
{
    audioSettings->setCodec(codec);
    audioSettings->setSampleRate(rate);
    audioSettings->setChannelCount(channels);
    audioSettings->setBitRate(bitrate);
    audioSettings->setQuality(quality);
    audioSettings->setEncodingMode(mode);

    // 设置音频设置
    setEncodingSettings(*audioSettings);
}


void AudioInput::setAudioPath(const QString &path, const QString &fileName)
{
    // 设置音频文件路径
    this->setOutputLocation(QUrl::fromLocalFile(path + fileName));

}


void AudioInput::startAudio()
{
    // 开始录音
    record();
}

void AudioInput::stopAudio()
{
    // 停止录音
    stop();

    // 生成WAV头并拼接数据
    QByteArray wavData;
    if (!rawPCMData.isEmpty()) {
        wavData = generateWavHeader(rawPCMData.size());
        wavData.append(rawPCMData);
        rawPCMData.clear(); // 清空缓存
    }
    emit recordingFinished_Byte(wavData); // 发射信号携带完整WAV数据

    // 停止定时器
    if (timer->isActive()) {
        timer->stop();
    }
    if (silenceTimer->isActive()) {
        silenceTimer->stop();
    }
}

// 开始录音并设置定时器
void AudioInput::startAudioWithDuration(int duration)
{
    startAudio();
    timer->start(duration * 1000);  // 将秒转换为毫秒
}

// 定时器超时槽函数
void AudioInput::onTimeout()
{
    stop();
    timer->stop();
    silenceTimer->stop();
    qDebug() << "录音已停止：检测到静音";
}

// 开始自动录音
void AudioInput::startAutoStopAudio(qreal silenceThreshold, int silenceDuration)
{
    this->silenceThreshold = silenceThreshold;
    this->silenceDuration = silenceDuration;

    // 启动录音
    startAudio();

    // 绑定 QAudioProbe
    if (probe->setSource(this)) {
        qDebug() << "QAudioProbe 绑定成功";
    } else {
        qWarning() << "QAudioProbe 绑定失败";
    }

    // 延迟启动定时器，等待第一次音频数据分析
    QTimer::singleShot(100, this, [this]() {
        silenceTimer->start(this->silenceDuration);
    });
}

// 处理音频缓冲区
void AudioInput::processBuffer(const QAudioBuffer& buffer)
{
    // 保存音频格式（首次调用时记录）
    if (audioFormat != buffer.format()) {
        audioFormat = buffer.format();
        qDebug() << "音频格式已捕获:"
                 << "采样率:" << audioFormat.sampleRate()
                 << "声道数:" << audioFormat.channelCount()
                 << "位深:" << audioFormat.sampleSize();
    }

    // 将音频数据追加到缓冲区
    const char *datas = buffer.constData<char>();
    rawPCMData.append(datas, buffer.byteCount());


    qreal rmsValue = 0;
    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    // 计算 RMS 音量
    for (int i = 0; i < sampleCount; ++i) {
        rmsValue += static_cast<qreal>(data[i]) * data[i];
    }
    rmsValue = qSqrt(rmsValue / sampleCount);

    qDebug() << "RMS 音量：" << rmsValue;

    // 判断是否静音
    if (rmsValue < silenceThreshold) {
        // 如果静音，启动或继续定时器
        if (!silenceTimer->isActive()) {
            silenceTimer->start(silenceDuration);
        }
    } else {
        // 如果有声音，重置定时器
        silenceTimer->stop();
        silenceTimer->start(silenceDuration);  // 重置静音计时
    }
}

// 获取所有音频输入设备
QList<QString> AudioInput::getAvailableAudioInputDevices()
{
    QList<QString> devices;
            foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
            devices.append(deviceInfo.deviceName());
        }
    return devices;
}

// 设置当前录音设备
void AudioInput::setAudioInputDevice(const QString &deviceName)
{
            foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
            if (deviceInfo.deviceName() == deviceName) {
                setAudioInput(deviceInfo.deviceName());
                break;
            }
        }
}

QByteArray AudioInput::generateWavHeader(quint32 dataSize) const {
    // WAV头结构定义
    struct WavHeader {
        char     riff[4] = {'R','I','F','F'};
        quint32  chunkSize;
        char     wave[4] = {'W','A','V','E'};
        char     fmt[4] = {'f','m','t',' '};
        quint32  fmtSize = 16;          // PCM格式块大小
        quint16  audioFormat = 1;       // PCM=1
        quint16  numChannels;
        quint32  sampleRate;
        quint32  byteRate;
        quint16  blockAlign;
        quint16  bitsPerSample;
        char     data[4] = {'d','a','t','a'};
        quint32  dataSize;
    } header;

    // 填充头信息
    header.numChannels = audioFormat.channelCount();
    header.sampleRate = audioFormat.sampleRate();
    header.bitsPerSample = audioFormat.sampleSize();
    header.byteRate = audioFormat.sampleRate() * audioFormat.channelCount() * (header.bitsPerSample / 8);
    header.blockAlign = audioFormat.channelCount() * (header.bitsPerSample / 8);
    header.dataSize = dataSize;
    header.chunkSize = 36 + dataSize; // 整个文件大小 - 8字节

    // 将结构体转为字节数组
    QByteArray headerBytes(reinterpret_cast<const char*>(&header), sizeof(WavHeader));
    return headerBytes;
}
