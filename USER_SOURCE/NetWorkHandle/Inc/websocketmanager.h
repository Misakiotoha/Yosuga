//
// Created by Administrator on 2025/2/4.
//

#ifndef AIRI_DESKTOPGRIL_WEBSOCKETMANAGER_H
#define AIRI_DESKTOPGRIL_WEBSOCKETMANAGER_H


#include <QObject>
#include <QtWebSockets/QWebSocket>

class WebSocketManager : public QObject
{
Q_OBJECT
public:
    explicit WebSocketManager(QObject *parent = nullptr);

    ~WebSocketManager();

    // 上传接口（传入本地文件路径）
    Q_INVOKABLE void uploadFile(const QString &filePath);

    // 连接服务器
    void connectToServer();

    // 断开服务器
    void disconnectFromServer();

    // 设置服务器地址get&set方法
    void setUrl(const QString &url);
    QString getUrl();

signals:
    // 上传进度（0-100）
    void uploadProgressChanged(int percent);
    // 下载进度（0-100）
    void downloadProgressChanged(int percent);
    // 文件处理完成（返回保存路径）
    void fileProcessed(const QString &filePath);
    // 错误通知
    void errorOccurred(const QString &message);

private slots:
    void onConnected();
    void onBinaryMessageReceived(const QByteArray &message);
    void onError(QAbstractSocket::SocketError error);

private:
    QWebSocket m_socket;
    QByteArray m_receivedData;
    qint64 m_totalFileSize = 0;
    QString url = "ws://localhost:8765";
};

#endif //AIRI_DESKTOPGRIL_WEBSOCKETMANAGER_H
