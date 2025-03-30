//
// Created by Administrator on 2025/2/5.
//

#include "socketmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>


// 初始化静态成员变量
SocketManager* SocketManager::m_instance = nullptr;
QMutex SocketManager::m_mutex;

SocketManager* SocketManager::getInstance()
{
    QMutexLocker locker(&m_mutex); // 自动加锁，确保线程安全
    if (!m_instance) {
        m_instance = new SocketManager(); // 延迟初始化，首次调用时创建实例
    }
    return m_instance;
}

SocketManager::SocketManager(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &QTcpSocket::connected, [=]() {
        qDebug() << "SocketManager::connected !";
    });

    connect(this, &QTcpSocket::disconnected, [=]() {
        qDebug() << "SocketManager::disconnected !";
    });
    // 当有数据到达就调用handleReadyRead进行处理数据
    connect(this, &QTcpSocket::readyRead, this, &SocketManager::handleReadyRead);

}

SocketManager::~SocketManager()
{

}

void SocketManager::connectToServer()
{
    // 如果未连接，就连接
    if(this->state() == QAbstractSocket::UnconnectedState){
        this->connectToHost(this->ip, this->port);
        return;
    }
    // 已连接则返回
    if(this->state() ==  QAbstractSocket::ConnectedState){
        return;
    }
}

void SocketManager::disconnectFromServer()
{
    this->disconnectFromHost();
}

/**
 * 报文形式:先发送数据长度，再发送数据本身
 * 数据长度占4字节，大端
 * 数据本身压缩发送
 * @param filePath
 */
void SocketManager::sendWavFile(const QString &filePath)
{
    if(this->state() == QAbstractSocket::ConnectedState){
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)){
            // 压缩数据
            QByteArray compressedData = qCompress(file.readAll(), 9);
            // 发送数据长度（4字节，大端）
            quint32 totalSize = compressedData.size();
            QByteArray sizeData;
            QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
            sizeStream.setByteOrder(QDataStream::BigEndian);
            sizeStream << totalSize;
            this->write(sizeData);
            this->flush();

            // 发送压缩数据
            this->write(compressedData);
            this->flush();
            file.close();
        }
    } else{
        qDebug() << "SocketManager::发送失败! 请检查是否连接到服务端!";
    }
}

/**
 * 发送二进制WAV数据
 * 数据长度占4字节，大端
 * 数据本身压缩发送
 * @param wavData
 */
void SocketManager::sendWavFile(const QByteArray &wavData)
{
    if (this->state() == QAbstractSocket::ConnectedState) {
        if (!wavData.isEmpty()) {
            // 压缩数据（保持与文件发送相同的压缩方式）
            QByteArray compressedData = qCompress(wavData, 9);

            // 发送数据长度（4字节大端）
            quint32 totalSize = compressedData.size();
            QByteArray sizeData;
            QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
            sizeStream.setByteOrder(QDataStream::BigEndian);
            sizeStream << totalSize;

            // 分步发送确保可靠性
            this->write(sizeData);
            this->flush();


            this->write(compressedData);
            this->flush();
            qDebug() << "二进制WAV数据已发送，原始大小:" << wavData.size()
                     << "压缩后大小:" << compressedData.size();

        } else {
            qWarning() << "尝试发送空的WAV数据";
        }
    } else {
        qDebug() << "SocketManager::发送失败! 未连接到服务端!";
    }
}

void SocketManager::handleReadyRead()
{
    receiveBuffer.append(this->readAll()); // 累积数据到缓冲区
    // 检查是否包含结束标记
    int endIndex;
    while ((endIndex = receiveBuffer.indexOf(endMarker)) != -1) {
        // 提取结束标记前的数据
        QString data = receiveBuffer.left(endIndex);
        receiveBuffer = receiveBuffer.mid(endIndex + strlen(endMarker)); // 移除已处理数据

        // 处理数据
        // 使用动态文件名
        QFile rev(filePath + "revTest_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") +".wav");
        if (rev.open(QIODevice::WriteOnly)) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data.toUtf8());
            // 提取JSON对象
            QJsonObject jsonObj = jsonDoc.object();
            QString response;
            // 解析response字段
            if (jsonObj.contains("response") && jsonObj["response"].isString()) {
                response = jsonObj["response"].toString();
                qDebug() << "解析到response:" << response;
            } else {
                qDebug() << "response字段缺失或类型错误";
            }
            float duration;
            if (jsonObj.contains("wav_duration") && jsonObj["wav_duration"].isDouble()) {
                duration = jsonObj["wav_duration"].toDouble();
                qDebug() << "解析到duration:" << duration;
            } else {
                qDebug() << "duration字段缺失或类型错误";
            }
            QByteArray wavData;
            // 解析wav_data_base64字段
            if (jsonObj.contains("wav_data_base64") && jsonObj["wav_data_base64"].isString()) {
                QString base64Data = jsonObj["wav_data_base64"].toString();
                wavData = QByteArray::fromBase64(base64Data.toUtf8());
                // 获取当前音频时长
                qDebug() << "音频数据大小：" << wavData.size() << "字节";
            } else {
                qDebug() << "wav_data_base64字段缺失或类型错误";
            }

            rev.write(wavData);
            rev.close();
            // 清空缓冲区，这一步很重要，不然会导致下次接收数据时，会保存着上次接收的数据
            receiveBuffer.clear();
            // 发送信号
            emit revWavFileFinish(rev.fileName(), response, duration);
            emit revWavDataFinish(wavData);
            qDebug() << "文件接收完成并保存: " + rev.fileName();
        } else {
            qDebug() << "无法保存文件";
        }
    }
}



void SocketManager::setIp(const QString &ip)
{
    this->ip = ip;
}

QString SocketManager::getIp()
{
    return this->ip;
}

void SocketManager::setPort(qint16 port)
{
    this->port = port;
}

qint16 SocketManager::getPort()
{
    return this->port;
}
