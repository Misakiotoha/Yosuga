//
// Created by Administrator on 2025/3/4.
//

#ifndef AIRI_DESKTOPGRIL_AUDIOPAGE_H
#define AIRI_DESKTOPGRIL_AUDIOPAGE_H

#include "BasePage.h"

class ElaToggleSwitch;
class ElaToggleButton;
class ElaComboBox;
class ElaMultiSelectComboBox;
class ElaMessageButton;
class ElaCheckBox;
class ElaSpinBox;
class ElaSlider;
class ElaRadioButton;
class ElaProgressBar;
class AudioPage : public BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit AudioPage(QWidget* parent = nullptr);
    ~AudioPage();

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    ElaComboBox* audioInputDeviceComboBox = nullptr;
    ElaSpinBox* audioInputSpinBox = nullptr;
    ElaProgressBar* audioInputProgressBar = nullptr;
};



#endif //AIRI_DESKTOPGRIL_AUDIOPAGE_H
