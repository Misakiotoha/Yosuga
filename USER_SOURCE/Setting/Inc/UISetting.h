//
// Created by Administrator on 2025/3/2.
//

#ifndef AIRI_DESKTOPGRIL_UISETTING_H
#define AIRI_DESKTOPGRIL_UISETTING_H

#include "BasePage.h"
class ElaRadioButton;
class ElaToggleSwitch;
class ElaComboBox;
class UISetting : public BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit UISetting(QWidget* parent = nullptr);
    ~UISetting();

private:
    ElaComboBox* _themeComboBox{nullptr};
    ElaToggleSwitch* _micaSwitchButton{nullptr};
    ElaToggleSwitch* _logSwitchButton{nullptr};
    ElaRadioButton* _minimumButton{nullptr};
    ElaRadioButton* _compactButton{nullptr};
    ElaRadioButton* _maximumButton{nullptr};
    ElaRadioButton* _autoButton{nullptr};
};

#endif //AIRI_DESKTOPGRIL_UISETTING_H
