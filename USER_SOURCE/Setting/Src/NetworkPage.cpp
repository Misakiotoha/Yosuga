//
// Created by Administrator on 2025/3/2.
//

#include "NetworkPage.h"


#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaMessageButton.h"
#include "ElaMultiSelectComboBox.h"
#include "ElaPlainTextEdit.h"
#include "ElaProgressBar.h"
#include "ElaRadioButton.h"
#include "ElaScrollPageArea.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "ElaToggleButton.h"
#include "ElaToggleSwitch.h"

#include "socketmanager.h"
#include <QDebug>
#include <QHostAddress>
#include "ElaMessageBar.h"
NetWorkPage::NetWorkPage(QWidget* parent)
        : BasePage(parent)
{
    // 预览窗口标题
    setWindowTitle("NetworkPage");

    // ip
    ipPushButton = new ElaPushButton("设定",this);
    ipLineEdit = new ElaLineEdit(this);
    ElaScrollPageArea* ipToggleSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout* ipToggleSwitchLayout = new QHBoxLayout(ipToggleSwitchArea);
    ElaText* ipToggleSwitchText = new ElaText("服务端IP:", this);
    ipToggleSwitchText->setTextPixelSize(15);
    ipToggleSwitchLayout->addWidget(ipToggleSwitchText);
    ipToggleSwitchLayout->addWidget(ipLineEdit);
    ipToggleSwitchLayout->addStretch();
    connect(ipPushButton, &ElaPushButton::clicked, this, [=]() {
        // 我爱lambda函数
        QString ip_temp = ipLineEdit->text();
        auto f_temp = [=](const QString &ip) -> bool {
                QHostAddress addr;
                if (addr.setAddress(ip) && addr.protocol() == QAbstractSocket::IPv4Protocol) {
                    return true;
                }
                return false;
        };
        if(!f_temp(ip_temp)){
            ElaMessageBar::error(ElaMessageBarType::TopLeft, "连接设置", "服务端IP格式错误", 800.0, this);
        }
        else{
            SocketManager::getInstance()->setIp(ip_temp);
            ElaMessageBar::success(ElaMessageBarType::TopRight, "连接设置", "服务端IP设置成功", 800.0, this);
        }
    });
    ipToggleSwitchLayout->addWidget(ipPushButton);
    ipToggleSwitchLayout->addSpacing(10);

    // 端口
    portPushButton = new ElaPushButton("设定",this);
    portLineEdit = new ElaLineEdit(this);
    ElaScrollPageArea* portToggleSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout* portToggleSwitchLayout = new QHBoxLayout(portToggleSwitchArea);
    ElaText* portToggleSwitchText = new ElaText("服务端端口:", this);
    portToggleSwitchText->setTextPixelSize(15);
    portToggleSwitchLayout->addWidget(portToggleSwitchText);
    portToggleSwitchLayout->addWidget(portLineEdit);
    portToggleSwitchLayout->addStretch();
    connect(portPushButton, &ElaPushButton::clicked, this, [=]() {
        QString port_temp = portLineEdit->text();
        auto f_temp = [=](const QString &port) -> bool {
            return port.toInt() > 0 && port.toInt() < 65535;
        };
        if(!f_temp(port_temp)){
            ElaMessageBar::error(ElaMessageBarType::TopLeft, "连接设置", "服务端端口格式错误", 800.0, this);
        }
        else{
            SocketManager::getInstance()->setPort(port_temp.toInt());
            ElaMessageBar::success(ElaMessageBarType::TopRight, "连接设置", "服务端端口设置成功", 800.0, this);
        }
    });
    portToggleSwitchLayout->addWidget(portPushButton);
    portToggleSwitchLayout->addSpacing(10);

    connectTestPushButton = new ElaPushButton("连通测试",this);
    connectPushButton = new ElaPushButton("连接",this);
    disconnectPushButton = new ElaPushButton("断开",this);
    ElaScrollPageArea* connectTestArea = new ElaScrollPageArea(this);
    QHBoxLayout* connectTestLayout = new QHBoxLayout(connectTestArea);
    connectTestLayout->addWidget(connectTestPushButton);
    connectTestLayout->addStretch();
    connect(connectTestPushButton, &ElaPushButton::clicked, this, [=]() {
        if(SocketManager::getInstance()->state() == QAbstractSocket::ConnectedState){
            // 如果已连接
            ElaMessageBar::success(ElaMessageBarType::TopRight, "连通测试", "已连通", 800.0, this);
        }
        else{
            SocketManager::getInstance()->connectToServer();
            // TODO:等待很短的时间，等tcp握手完成

            if(SocketManager::getInstance()->state() == QAbstractSocket::ConnectedState){
                ElaMessageBar::success(ElaMessageBarType::TopRight, "连通测试", "已连通", 800.0, this);
            }
            else{
                ElaMessageBar::error(ElaMessageBarType::TopLeft, "连通测试", "未连通，请检查服务器是否开启或IP和端口信息是否正确", 1500.0, this);
            }
        }

    });
    connect(connectPushButton, &ElaPushButton::clicked, this, [=]() {
        if(SocketManager::getInstance()->state() == QAbstractSocket::ConnectedState){
            // 如果已连接
            ElaMessageBar::success(ElaMessageBarType::TopRight, "连通测试", "已连接", 800.0, this);
        }
        if(SocketManager::getInstance()->state() == QAbstractSocket::UnconnectedState){
            // 如果未连接
            SocketManager::getInstance()->connectToServer();
            if(SocketManager::getInstance()->state() == QAbstractSocket::ConnectedState){
                ElaMessageBar::success(ElaMessageBarType::TopRight, "连通测试", "连接成功", 800.0, this);
            }
            else{
                ElaMessageBar::error(ElaMessageBarType::TopLeft, "连通测试", "连接失败，请检查服务器是否开启或IP和端口信息是否正确", 1500.0, this);
            }
        }
    });
    connect(disconnectPushButton, &ElaPushButton::clicked, this, [=]() {
        if(SocketManager::getInstance()->state() == QAbstractSocket::ConnectedState){
            // 如果已连接，则断开
            SocketManager::getInstance()->disconnectFromServer();
            ElaMessageBar::success(ElaMessageBarType::TopRight, "连通测试", "断开成功", 800.0, this);
        }
        else{
            // 如果未连接，则提示
            ElaMessageBar::information(ElaMessageBarType::BottomRight, "连通测试", "似乎并没有连接到服务器", 800.0, this);
        }
    });
    connectTestLayout->addWidget(disconnectPushButton);
    connectTestLayout->addWidget(connectPushButton);
    connectTestLayout->addSpacing(10);



    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("连接设置");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addWidget(ipToggleSwitchArea);
    centerLayout->addWidget(portToggleSwitchArea);
    centerLayout->addWidget(connectTestArea);

    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

}

NetWorkPage::~NetWorkPage()
{
}


