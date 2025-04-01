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
    // 创建探测器对象
    probe = new QAudioProbe(this);
    // 创建定时器对象
    timer = new QTimer(this);
    silenceTimer = new QTimer(this);
    thresholdTimer = new QTimer(this);

    // 连接定时器的 timeout 信号到 onTimeout 方法
    connect(timer, &QTimer::timeout, this, &AudioInput::onTimeout);
    // 连接静音检测定时器的 timeout 信号到 stopAudio 方法
    connect(silenceTimer, &QTimer::timeout, this, &AudioInput::stopAudio);
    // 连接阈值检测定时器的 timeout 信号到 thresholdTimeout 方法
    connect(thresholdTimer, &QTimer::timeout, this, &AudioInput::thresholdTimeout);
    thresholdTimer->setSingleShot(true);    // 设置为单次触发
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
    // 设置默认录音设备为第一个
    setAudioInputDevice(getAvailableAudioInputDevices().first());
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
        wavData = generateWavHeader(rawPCMData.size());  // 添加头
        wavData.append(rawPCMData);                             // 添加数据
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
    this->isAutoRecording = false;  // 自动录音结束tag
}

// 阈值检测超时槽函数
void AudioInput::thresholdTimeout()
{
    this->isAutoThreshold = false;  // 自动阈值结束tag

    // 停止录音
    stop();

    // 停止定时器
    if (thresholdTimer->isActive()) {
        thresholdTimer->stop();
    }
    if (!this->rmsValues.empty()){
        // 求和
        double sumT = std::accumulate(rmsValues.begin(), rmsValues.end(), 0.0);
        // 求平均
        double avgT = sumT / rmsValues.size();
        this->silenceThreshold = avgT + 500.0;
        // 清空缓存
        rmsValues.clear();
        // 发射最优静音阈值
        emit thresholdCalculated(this->silenceThreshold);
    }
    else{
        // 否则发射0作为静音阈值
        emit thresholdCalculated(0);
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
    this->isAutoRecording = true;   // 自动录音开始tag

    // 记录当前的静音阈值和静音持续时间
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

// 开始自动阈值检测
void AudioInput::startAutoThresholdClu(int Duration)
{
    this->isAutoThreshold = true;   // 自动阈值开始tag

    // 启动录音
    startAudio();

    // 绑定 QAudioProbe
    if (probe->setSource(this)) {
        qDebug() << "QAudioProbe 绑定成功";
    } else {
        qWarning() << "QAudioProbe 绑定失败";
    }

    // 启动定时器
    QTimer::singleShot(100, this, [this, Duration]() {
        thresholdTimer->start(Duration);
    });

}

/**
 * 处理音频缓冲区
 * 下面的函数同时承载了两个功能
 *      1.静音检测
 *      2.自动阈值检测
 * 为什么要让一个槽函数承载两个功能，这就要说说Qt5音频处理这一块了
 * QAudioProbe是用作音频探测的，将QAudioProbe绑定到QAudioRecorder
 * QAudioProbe会触发probeBufferProbed信号
 * 如果存在多个QAudioProbe绑定到相同QAudioRecorder
 * 那么当QAudioRecorder启动录音时多个QAudioProbe的probeBufferProbed信号都会触发
 * 因此在两个功能同时被使用时，与其使用两个槽函数交替运行浪费堆栈
 * 不如都写在一个里面用bool去区分
 *
 * @param buffer
 */
void AudioInput::processBuffer(const QAudioBuffer& buffer)
{
    // 计算RMS
    this->rmsValue = this->calculateRMS(buffer);

    if(this->isAutoRecording){  // 自动停止录音
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

        qDebug() << "RMS 音量：" << this->rmsValue;

        // 判断是否静音
        if (this->rmsValue < this->silenceThreshold) {
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
    if(this->isAutoThreshold){  // 阈值检测
        // 得到当前实时的RMS值通过信号发射出去，如何处理交给绑定这个信号的槽函数
        rmsValues.push_back(rmsValue);  // 添加到vector当中
        qDebug() << "RMS 音量：" << rmsValue;
        emit rmsRealValue(rmsValue);    // 一般用于实时阈值显示
    }
}

qreal AudioInput::calculateRMS(const QAudioBuffer& buffer)
{
    qreal rmsValueT = 0;

    const qint16* data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    if (sampleCount == 0) return 0;

    // 计算平方和
    for (int i = 0; i < sampleCount; ++i) {
        rmsValueT += static_cast<qreal>(data[i]) * data[i];
    }

    // 计算均方根
    rmsValueT = qSqrt(rmsValueT / sampleCount);

    return rmsValueT;
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

void AudioInput::setSilenceThreshold(qreal silenceThreshold)
{
    this->silenceThreshold = silenceThreshold;
}

qreal AudioInput::getSilenceThreshold() const
{
    return this->silenceThreshold;
}
