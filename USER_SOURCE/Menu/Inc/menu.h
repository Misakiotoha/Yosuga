#ifndef MENU_H
#define MENU_H

/**
 * @brief  菜单
 * @author Misaki
 *
 * 基于Ela UI的菜单控件
 */

#include "ElaMenu.h"
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QScopedPointer>   // 智能指针

#include "Setting.h"
#include "networkmanager.h"
#include "socketmanager.h"

class Menu : public ElaMenu
{
Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();
    void showMenu(const QPoint &pos);

signals:
    void closeMainWindow();  // 自定义关闭主窗口的信号
    void startPlay();        // 自定义开始播放的信号


private:
    void createMenu();
    QAction *toggleThe;         /// 切换主题(全局)
    QAction *startExchangeAction;   /// 开启对话
    QAction *settingsAction;    /// 设置
    QAction *closeAction;       /// 关闭

    QScopedPointer<Setting> settingWindow; // 使用智能指针管理 Setting 窗口

private slots:
    void toggleTheme();

    void startExchange();
};

#endif // MENU_H