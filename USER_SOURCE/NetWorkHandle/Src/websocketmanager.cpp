//
// Created by Administrator on 2025/2/4.
//


#include "websocketmanager.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <zlib.h>

WebSocketManager::WebSocketManager(QObject *parent)
        : QObject(parent)
{
    // 连接信号槽
    connect(&m_socket, &QWebSocket::connected,
            this, &WebSocketManager::onConnected);
    connect(&m_socket, &QWebSocket::binaryMessageReceived,
            this, &WebSocketManager::onBinaryMessageReceived);
    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &WebSocketManager::onError);

}

WebSocketManager::~WebSocketManager()
{
    m_socket.close();
}

void WebSocketManager::uploadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(tr("无法打开文件: %1").arg(filePath));
        return;
    }

    // 读取并压缩数据
    QByteArray rawData = file.readAll();
    QByteArray compressedData = qCompress(rawData, 9);

    // 重置接收缓存
    m_receivedData.clear();
    m_totalFileSize = compressedData.size();

    // 分块发送（每64KB一个块）
    const int chunkSize = 64 * 1024;
    int totalChunks = (compressedData.size() + chunkSize - 1) / chunkSize;

    for (int i = 0; i < totalChunks; ++i) {
        QByteArray chunk = compressedData.mid(i * chunkSize, chunkSize);
        m_socket.sendBinaryMessage(chunk);

        // 计算上传进度
        int progress = (i + 1) * 100 / totalChunks;
        emit uploadProgressChanged(progress);
    }

    // 发送结束标记
    m_socket.sendBinaryMessage("END");
}

void WebSocketManager::onBinaryMessageReceived(const QByteArray &message)
{
    if (message == "END") {
        // 解压接收数据
        QByteArray decompressedData = qUncompress(m_receivedData);

        // 生成保存路径
        QString savePath = QDir::tempPath() + "/processed_" +
                           QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".wav";

        // 保存文件
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(decompressedData);
            emit fileProcessed(savePath);
        } else {
            emit errorOccurred(tr("无法保存文件: %1").arg(savePath));
        }

        m_receivedData.clear();
    } else {
        m_receivedData.append(message);

        // 计算下载进度
        if (m_totalFileSize > 0) {
            int progress = m_receivedData.size() * 100 / m_totalFileSize;
            emit downloadProgressChanged(progress);
        }
    }
}

void WebSocketManager::connectToServer()
{
    // 检查是否已经连接到服务器
    if (m_socket.state() == QAbstractSocket::ConnectedState) {
        emit errorOccurred(tr("已经连接到服务器"));
        return;
    }
    // 连接服务器
    m_socket.open(QUrl(this->url));
}


void WebSocketManager::setUrl(const QString &url)
{
    this->url = url;
}

QString WebSocketManager::getUrl()
{
    return this->url;
}


void WebSocketManager::onConnected()
{
    qDebug() << "Connected to server";
}

void WebSocketManager::onError(QAbstractSocket::SocketError error)
{
    emit errorOccurred(tr("网络错误: %1").arg(m_socket.errorString()));
}