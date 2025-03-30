//
// Created by Administrator on 2025/1/19.
//

#include "networkmanager.h"
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo>
#include <QThread>

NetWorkManager::NetWorkManager(QObject *parent) : QObject(parent)
{
    this->manager = new QNetworkAccessManager(this);
    this->file = nullptr;
    this->reply = nullptr;
    this->timer = new QTimer(this);
    this->timeout = 30000; // 默认超时时间为 30 秒

    // 连接请求完成信号
    connect(this->manager, &QNetworkAccessManager::finished, this, &NetWorkManager::onReplyFinished);

    // 连接超时信号
    connect(this->timer, &QTimer::timeout, this, &NetWorkManager::onTimeout);
}

NetWorkManager::~NetWorkManager()
{
    if (this->reply) {
        this->reply->abort();
        this->reply->deleteLater();
    }
    if (this->file) {
        this->file->close();
        delete this->file;
    }
}

// GET 请求
void NetWorkManager::get(const QString &url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // 设置请求头
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    this->reply = manager->get(request);

    // 启动超时计时器
    this->timer->start(timeout);

    // 连接下载进度信号
    connect(this->reply, &QNetworkReply::downloadProgress, this, &NetWorkManager::onDownloadProgress);
}

// POST 请求（JSON 数据）
void NetWorkManager::post(const QString &url, const QJsonObject &json)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 设置请求头
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    QByteArray data = QJsonDocument(json).toJson();
    this->reply = manager->post(request, data);

    // 启动超时计时器
    this->timer->start(timeout);

    // 连接上传进度信号
    connect(this->reply, &QNetworkReply::uploadProgress, this, &NetWorkManager::onUploadProgress);
}

// 文件下载
void NetWorkManager::downloadFile(const QString &url, const QString &savePath)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // 设置请求头
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    this->reply = manager->get(request);

    // 启动超时计时器
    this->timer->start(timeout);

    // 打开文件
    this->file = new QFile(savePath);
    if (!this->file->open(QIODevice::WriteOnly)) {
        emit errorOccurred("Failed to open file for writing");
        delete this->file;
        this->file = nullptr;
        return;
    }

    // 连接下载进度信号
    connect(this->reply, &QNetworkReply::downloadProgress, this, &NetWorkManager::onDownloadProgress);

    // 读取数据并写入文件
    connect(reply, &QNetworkReply::readyRead, this, [this]() {
        if (this->file) {
            this->file->write(reply->readAll());
        }
    });
}

// 文件上传
void NetWorkManager::uploadFile(const QString &url, const QString &filePath)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // 创建文件部分
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(filePath).fileName() + "\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));

    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit errorOccurred("Failed to open file for reading");
        delete file;
        return;
    }

    filePart.setBodyDevice(file);
    file->setParent(multiPart); // 将文件对象绑定到 multiPart，由 multiPart 负责释放

    multiPart->append(filePart);

    // 发送请求
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // 设置请求头
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    this->reply = manager->post(request, multiPart);
    multiPart->setParent(reply); // 将 multiPart 绑定到 reply，由 reply 负责释放

    // 启动超时计时器
    this->timer->start(timeout);

    // 连接上传进度信号
    connect(reply, &QNetworkReply::uploadProgress, this, &NetWorkManager::onUploadProgress);
}

// 设置超时时间
void NetWorkManager::setTimeout(int timeout)
{
    this->timeout = timeout;
}

// 设置请求头
void NetWorkManager::setHeader(const QString &key, const QString &value)
{
    this->headers[key] = value;
}

// 清除请求头
void NetWorkManager::clearHeaders()
{
    this->headers.clear();
}

// 请求完成槽函数
void NetWorkManager::onReplyFinished(QNetworkReply *reply)
{
    this->timer->stop(); // 停止超时计时器

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        emit requestFinished(response);
    } else {
        emit errorOccurred(reply->errorString());
    }

    // 关闭并释放文件对象
    if (file) {
        file->close();
        delete file;
        file = nullptr;
    }

    reply->deleteLater();
}

// 下载进度槽函数
void NetWorkManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

// 上传进度槽函数
void NetWorkManager::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}

// 超时槽函数
void NetWorkManager::onTimeout()
{
    if (reply) {
        this->reply->abort();
        emit timeoutOccurred();
    }
}
