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
    ElaToggleSwitch* _toggleSwitch{nullptr};
    ElaToggleButton* _toggleButton{nullptr};
    ElaComboBox* _comboBox{nullptr};
    ElaMultiSelectComboBox* _multiSelectComboBox{nullptr};
    ElaMessageButton* _messageButton{nullptr};
    ElaMessageButton* _infoMessageButton{nullptr};
    ElaMessageButton* _warningMessageButton{nullptr};
    ElaMessageButton* _errorMessageButton{nullptr};
    ElaCheckBox* _checkBox{nullptr};
    ElaSpinBox* _spinBox{nullptr};
    ElaSlider* _slider{nullptr};
    ElaRadioButton* _radioButton{nullptr};
    ElaProgressBar* _progressBar{nullptr};
};



#endif //AIRI_DESKTOPGRIL_AUDIOPAGE_H
