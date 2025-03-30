//
// Created by Administrator on 2025/3/30.
//

#ifndef YOSUGA_RENDERPAGE_H
#define YOSUGA_RENDERPAGE_H

#include "BasePage.h"
#include "ElaPushButton.h"
#include "ElaLineEdit.h"
#include "ElaComboBox.h"
class RenderPage : public BasePage
{
Q_OBJECT
public:
    Q_INVOKABLE explicit RenderPage(QWidget* parent = nullptr);
    ~RenderPage();


private:
    // 帧率设置
    ElaComboBox* frameRateComboBox = nullptr;



};



#endif //YOSUGA_RENDERPAGE_H
