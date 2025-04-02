//
// Created by Administrator on 2025/1/21.
//

#ifndef AIRI_DESKTOPGRIL_SETTING_H
#define AIRI_DESKTOPGRIL_SETTING_H

#include <ElaWidget.h>
#include <ElaWindow.h>
#include <ElaPushButton.h>
#include <ElaScrollPage.h>
#include <QStackedWidget>

#include "HomePage.h"
#include "NetworkPage.h"
#include "UISetting.h"
#include "AudioPage.h"
#include "RenderPage.h"
#include "ModelPage.h"
class Setting : public ElaWindow
{
Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

private:
    /**
     * 初始化所有页面指针
     * @author : Misaki
     */
    void initPages();

    /**
     * 初始化导航栏
     * @author : Misaki
     */
    void initNavigationBar();

    /**
     * 初始化上下文切换
     * @author : Misaki
     */
    void initContent();

private slots:
    void toggleTheme();

private:
    // 页面指针
    HomePage *homePage;
    NetWorkPage *networkPage;
    UISetting *uiSetting;
    AudioPage *audioPage;
    RenderPage *renderPage;
    ModelPage *modelPage;

    // 节点键值
    QString basePageKey;

    //
    ElaPushButton *themeToggleButton;

};


#endif //AIRI_DESKTOPGRIL_SETTING_H
