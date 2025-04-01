//
// Created by Administrator on 2025/1/19.
//

/**
 * 已废弃
 */

#ifndef AIRI_DESKTOPGRIL_NETWORKMANAGER_H
#define AIRI_DESKTOPGRIL_NETWORKMANAGER_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QWaitCondition>

class NetWorkManager : public QObject
{
Q_OBJECT

public:
    explicit NetWorkManager(QObject *parent = nullptr);
    ~NetWorkManager();

    // GET 请求
    void get(const QString &url);

    // POST 请求（JSON 数据）
    void post(const QString &url, const QJsonObject &json);

    // 文件下载
    void downloadFile(const QString &url, const QString &savePath);

    // 文件上传
    void uploadFile(const QString &url, const QString &filePath);

    // 设置超时时间（毫秒）
    void setTimeout(int timeout);

    // 设置请求头
    void setHeader(const QString &key, const QString &value);

    // 清除请求头
    void clearHeaders();

signals:
    // 请求完成信号，返回响应数据
    void requestFinished(const QByteArray &response);

    // 下载进度信号
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    // 上传进度信号
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

    // 错误信号
    void errorOccurred(const QString &errorString);

    // 超时信号
    void timeoutOccurred();

private slots:
    // 请求完成槽函数
    void onReplyFinished(QNetworkReply *reply);

    // 下载进度槽函数
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    // 上传进度槽函数
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

    // 超时槽函数
    void onTimeout();

private:
    QNetworkAccessManager *manager; /// 网络管理对象
    QFile *file;                    /// 文件对象（用于下载/上传）
    QNetworkReply *reply;           /// 网络响应对象
    QTimer *timer;                  /// 超时计时器
    QMutex mutex;                   /// 互斥锁
    QWaitCondition condition;       /// 条件变量
    QMap<QString, QString> headers; /// 请求头
    int timeout;                    /// 超时时间
};



#endif //AIRI_DESKTOPGRIL_NETWORKMANAGER_H
