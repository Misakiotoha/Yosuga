//
// Created by Administrator on 2025/1/21.
//


#include <ElaTheme.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSvgRenderer>
#include <QPainter>


#include "Setting.h"

#include "socketmanager.h"
Setting::Setting(QWidget *parent)
        : ElaWindow(parent)
{
    // 设置窗口标题
    setWindowTitle("设置");
    // 设置窗口图标
    setWindowIcon(QIcon("Resource/Pic/Airi/Airi_s.svg"));
    // 初始化窗口
    resize(1000, 740);
    // 禁用窗口缩放按钮
    setWindowButtonFlag(ElaAppBarType::MaximizeButtonHint, false); // 隐藏最大化按钮
    setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false); // 隐藏最小化按钮
    // 移动窗口到屏幕中心
    this->moveToCenter();

    this->setWindowFlag(Qt::Tool); // 隐藏应用程序图标
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // 默认设置窗口始终在顶部
    // 设置用户信息卡
    this->setUserInfoCardTitle("Yosuga");
    this->setUserInfoCardSubTitle("联系维度的桥梁!");
    // 加载 SVG 图片
    QSvgRenderer renderer(QString("Resource/Pic/Airi/Airi.svg"));
    // 创建 QPixmap 并绘制 SVG
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent); // 设置透明背景
    QPainter painter(&pixmap);
    renderer.render(&painter);
    this->setUserInfoCardPixmap(pixmap);

    // 初始化页面
    initPages();

    // 创建导航栏和内容区域
    initNavigationBar();

    // 设置初始主题
    eTheme->setThemeMode(ElaThemeType::Dark);
}

Setting::~Setting()
{

}

void Setting::initPages()
{
    homePage = new HomePage(this);
    networkPage = new NetWorkPage(this);
    uiSetting = new UISetting(this);
    audioPage = new AudioPage(this);
}

void Setting::initNavigationBar()
{
    // 添加主页节点（顶级节点）
    addPageNode("主页", homePage, ElaIconType::House);

    // TODO 添加Live2D商店节点
    addPageNode("Live2D商店", nullptr, ElaIconType::Shop);

    // 添加网络连接设置节点
    addPageNode("连接设置", networkPage, ElaIconType::NetworkWired);
    // 添加音频设置节点
    addPageNode("音频设置", audioPage,  ElaIconType::MusicNote);

    QString expanderKey2;
    this->addExpanderNode("高级", expanderKey2, ElaIconType::Paintbrush);

    QString uiSettingKey;
    addFooterNode("UI设置", uiSetting, uiSettingKey, 0, ElaIconType::GearComplex);
}


void Setting::toggleTheme()
{
    if (eTheme->getThemeMode() == ElaThemeType::Light) {
        eTheme->setThemeMode(ElaThemeType::Dark);
    } else {
        eTheme->setThemeMode(ElaThemeType::Light);
    }
}

