//
// Created by Administrator on 2025/2/28.
//

#ifndef AIRI_DESKTOPGRIL_HOMEPAGE_H
#define AIRI_DESKTOPGRIL_HOMEPAGE_H

#include "BasePage.h"
class ElaMenu;
class HomePage : public BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit HomePage(QWidget* parent = nullptr);
    ~HomePage();
    Q_SIGNALS:
        Q_SIGNAL void audioNavigation();
        Q_SIGNAL void modelShopNavigation();
};



#endif //AIRI_DESKTOPGRIL_HOMEPAGE_H
