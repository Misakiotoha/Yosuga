//
// Created by Administrator on 2025/2/5.
//

#ifndef AIRI_DESKTOPGRIL_SOCKETMANAGER_H
#define AIRI_DESKTOPGRIL_SOCKETMANAGER_H

#include <QTcpSocket>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QDataStream>
#include <QHostAddress>
#include <QDateTime>
#include <QMutex>
/**
 * @author Misaki
 * @brief SocketManager类
 * 本模块基于QTcpSocket进一步拓展，主要针对音频文件的上传与下载
 * 基于CS架构与服务端进行通信
 * Socket提供长连接支持，异步通信
 */

class SocketManager : public QTcpSocket {
    Q_OBJECT
private:
    // 构造函数私有化
    explicit SocketManager(QObject *parent = nullptr);
public:
    // 删除拷贝构造函数和赋值运算符，禁止复制
    SocketManager(const SocketManager&) = delete;
    SocketManager& operator=(const SocketManager&) = delete;
    ~SocketManager();

    // 获取单例的静态方法
    static SocketManager* getInstance();

    void connectToServer();
    void disconnectFromServer();

    /**
     * 发送文件(wav) \n
     * 使用前需要先确保连接到服务端，不要调用完connectToServer后就直接调用这个函数 \n
     * TCP握手是需要一个很短的时间的，等连接稳定了多次调用这个函数就没有问题了 \n
     * 注意：调用这个函数请显示传入QString类型，否则编译器会不知道使用哪一个重载 \n
     * 原因如下： \n
     *  当调用 sendWavFile 时，如果传递的参数类型可能会被这两种函数参数类型接受或隐式转换，编译器就会报 “ambigous” 错误。例如： \n
        如果调用 sendWavFile("audio.wav")，因为 "audio.wav" 是一个 C 风格的字符串（const char*）， \n
        而 QString 和 QByteArray 都可以接受 const char* 的隐式转换： \n
        QString 的构造函数可以接受一个 const char*。 \n
        QByteArray 的构造函数也可以接受一个 const char*。 \n
        因此，编译器无法确定是要调用 sendWavFile(const QString &filePath) 还是 sendWavFile(const QByteArray &wavData)，从而导致歧义。\n
     * @author Misaki
     * @param filePath
     */
    void sendWavFile(const QString &filePath);

    /**
     * 直接发送二进制数据(wav)
     * 使用前需要先确保连接到服务端，不要调用完connectToServer后就直接调用这个函数
     * TCP握手是需要一个很短的时间的，等连接稳定了多次调用这个函数就没有问题了
     * @author Misaki
     * @param wavData
     */
    void sendWavFile(const QByteArray &wavData);

    // 设置目标服务端ip和端口的get&set方法
    void setIp(const QString &ip);
    QString getIp();

    void setPort(qint16 port);
    qint16 getPort();



signals:
    void revWavFileFinish(const QString &filePath, const QString &response, const float duration);
    void revWavDataFinish(const QByteArray &wavData);    // 字节流信号

private slots:
    /**
     * 处理接收到的数据
     * @author Misaki
     */
    void handleReadyRead();

private:
    // 单例实例指针
    static SocketManager* m_instance;
    static QMutex m_mutex; // 互斥锁确保线程安全

    QString ip = "127.0.0.1";                 /// 目标ip
    qint16 port = 12345;                      /// 目标端口

    QString filePath = "WavFiles\\";          /// 文件路径
    QString receiveBuffer;                 /// 接收缓冲区
    const char *endMarker = "<Eden*>";        /// 结束标记
};




#endif //AIRI_DESKTOPGRIL_SOCKETMANAGER_H
