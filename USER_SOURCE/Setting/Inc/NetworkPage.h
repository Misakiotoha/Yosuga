//
// Created by Administrator on 2025/3/2.
//

#ifndef AIRI_DESKTOPGRIL_NETWORKPAGE_H
#define AIRI_DESKTOPGRIL_NETWORKPAGE_H

#include "BasePage.h"
#include "ElaPushButton.h"
#include "ElaLineEdit.h"

class ElaPushButton;
class ElaLineEdit;
class NetWorkPage : public BasePage
{
Q_OBJECT
public:
    Q_INVOKABLE explicit NetWorkPage(QWidget* parent = nullptr);
    ~NetWorkPage();



private:
    // IP控件
    ElaPushButton* ipPushButton{nullptr};
    ElaLineEdit* ipLineEdit{nullptr};

    // 端口控件
    ElaPushButton* portPushButton{nullptr};
    ElaLineEdit* portLineEdit{nullptr};

    // 连接测试
    ElaPushButton* connectTestPushButton{nullptr};
    // 连接
    ElaPushButton* connectPushButton{nullptr};
    // 断开
    ElaPushButton* disconnectPushButton{nullptr};

};


#endif //AIRI_DESKTOPGRIL_NETWORKPAGE_H
