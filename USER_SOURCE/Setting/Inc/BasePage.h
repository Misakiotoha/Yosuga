//
// Created by Administrator on 2025/2/27.
//

#ifndef AIRI_DESKTOPGRIL_BASEPAGE_H
#define AIRI_DESKTOPGRIL_BASEPAGE_H

#include <ElaScrollPage.h>
class QVBoxLayout;
class BasePage : public ElaScrollPage
{
Q_OBJECT
public:
    Q_INVOKABLE explicit BasePage(QWidget* parent = nullptr);
    ~BasePage();

protected:
    void createCustomWidget(QString desText);
};

#endif //AIRI_DESKTOPGRIL_BASEPAGE_H
