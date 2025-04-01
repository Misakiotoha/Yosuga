//
// Created by Administrator on 2025/2/28.
//
#include "HomePage.h"


#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

#include "ElaAcrylicUrlCard.h"
#include "ElaFlowLayout.h"
#include "ElaImageCard.h"
#include "ElaMenu.h"
#include "ElaMessageBar.h"
#include "ElaNavigationRouter.h"
#include "ElaPopularCard.h"
#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaToolTip.h"
HomePage::HomePage(QWidget* parent)
        : BasePage(parent)
{
    // 预览窗口标题
    setWindowTitle("Home");

    setTitleVisible(false);
    setContentsMargins(2, 2, 0, 0);
    // 标题卡片区域
    ElaText* desText = new ElaText("UI By ElaWidgetTools", this);
    desText->setTextPixelSize(18);
    ElaText* titleText = new ElaText("Yosuga!", this);
    titleText->setTextPixelSize(35);

    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->setContentsMargins(30, 60, 0, 0);
    titleLayout->addWidget(desText);
    titleLayout->addWidget(titleText);

    ElaImageCard* backgroundCard = new ElaImageCard(this);
    backgroundCard->setBorderRadius(10);
    backgroundCard->setFixedHeight(400);
    backgroundCard->setMaximumAspectRatio(1.7);
    backgroundCard->setCardImage(QImage("Resource/Pic/Airi/Airi_Background.png"));

    ElaAcrylicUrlCard* urlCard1 = new ElaAcrylicUrlCard(this);
    urlCard1->setCardPixmapSize(QSize(62, 62));
    urlCard1->setFixedSize(195, 225);
    urlCard1->setTitlePixelSize(17);
    urlCard1->setTitleSpacing(25);
    urlCard1->setSubTitleSpacing(13);
    urlCard1->setUrl("https://github.com/Misakiotoha/Yosuga");
    urlCard1->setCardPixmap(QPixmap("Resource/Pic/Others/img.png"));
    urlCard1->setTitle("Yosuga Github");
    urlCard1->setSubTitle("Star++!");
    ElaToolTip* urlCard1ToolTip = new ElaToolTip(urlCard1);
    urlCard1ToolTip->setToolTip("点击前往本项目GitHub");

    ElaAcrylicUrlCard* urlCard2 = new ElaAcrylicUrlCard(this);
    urlCard2->setCardPixmapSize(QSize(62, 62));
    urlCard2->setFixedSize(195, 225);
    urlCard2->setTitlePixelSize(17);
    urlCard2->setTitleSpacing(25);
    urlCard2->setSubTitleSpacing(13);
    urlCard2->setUrl("https://space.bilibili.com/140315806");
    urlCard2->setCardPixmap(QPixmap("Resource/Pic/Others/Misaki.jpg"));
    urlCard2->setTitle("Misaki");
    urlCard2->setSubTitle("1841738040@qq.com");
    ElaToolTip* urlCard2ToolTip = new ElaToolTip(urlCard2);
    urlCard2ToolTip->setToolTip("点击前往 Misaki 的个人主页");

    ElaScrollArea* cardScrollArea = new ElaScrollArea(this);
    cardScrollArea->setWidgetResizable(true);
    cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setIsGrabGesture(true, 0);
    cardScrollArea->setIsOverShoot(Qt::Horizontal, true);
    QWidget* cardScrollAreaWidget = new QWidget(this);
    cardScrollAreaWidget->setStyleSheet("background-color:transparent;");
    cardScrollArea->setWidget(cardScrollAreaWidget);
    QHBoxLayout* urlCardLayout = new QHBoxLayout();
    urlCardLayout->setSpacing(15);
    urlCardLayout->setContentsMargins(30, 0, 0, 6);
    urlCardLayout->addWidget(urlCard1);
    urlCardLayout->addWidget(urlCard2);
    urlCardLayout->addStretch();
    QVBoxLayout* cardScrollAreaWidgetLayout = new QVBoxLayout(cardScrollAreaWidget);
    cardScrollAreaWidgetLayout->setContentsMargins(0, 0, 0, 0);
    cardScrollAreaWidgetLayout->addStretch();
    cardScrollAreaWidgetLayout->addLayout(urlCardLayout);

    QVBoxLayout* backgroundLayout = new QVBoxLayout(backgroundCard);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);
    backgroundLayout->addLayout(titleLayout);
    backgroundLayout->addWidget(cardScrollArea);

    // 推荐卡片
    ElaText* flowText = new ElaText("快速转到", this);
    flowText->setTextPixelSize(20);
    QHBoxLayout* flowTextLayout = new QHBoxLayout();
    flowTextLayout->setContentsMargins(33, 0, 0, 0);
    flowTextLayout->addWidget(flowText);
    // ElaFlowLayout
    ElaPopularCard* ModeShopCard = new ElaPopularCard(this);
    connect(ModeShopCard, &ElaPopularCard::popularCardButtonClicked, this, [=]() {
        QDesktopServices::openUrl(QUrl("https://github.com/Liniyous/ElaWidgetTools"));
    });
    ModeShopCard->setCardPixmap(QPixmap("Resource/Pic/Others/Live2D.png"));
    ModeShopCard->setTitle("模型商店");
    ModeShopCard->setSubTitle("属于你的Live2D模型");
    ModeShopCard->setInteractiveTips("By Misaki");
    ModeShopCard->setDetailedText("选择你喜欢的Live2D模型，模型来自多个作者，多个平台，有免费也有收费的");

    ElaPopularCard* AudioSettingCard = new ElaPopularCard(this);
    connect(AudioSettingCard, &ElaPopularCard::popularCardButtonClicked, this, [=]() { Q_EMIT elaScreenNavigation(); });
    AudioSettingCard->setTitle("音频设置");
    AudioSettingCard->setSubTitle("录音与播放的设置");
    AudioSettingCard->setCardPixmap(QPixmap("Resource/Pic/control/AutomationProperties.png"));
    AudioSettingCard->setInteractiveTips("By Misaki");
    AudioSettingCard->setDetailedText("自定义音频与播放的相关设定，打造最舒适的交流环境。");


    ElaFlowLayout* flowLayout = new ElaFlowLayout(0, 5, 5);
    flowLayout->setContentsMargins(30, 0, 0, 0);
    flowLayout->setIsAnimation(true);
    flowLayout->addWidget(ModeShopCard);
    flowLayout->addWidget(AudioSettingCard);


    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Home");
    QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setSpacing(0);
    centerVLayout->setContentsMargins(0, 0, 0, 0);
    centerVLayout->addWidget(backgroundCard);
    centerVLayout->addSpacing(20);
    centerVLayout->addLayout(flowTextLayout);
    centerVLayout->addSpacing(10);
    centerVLayout->addLayout(flowLayout);
    centerVLayout->addStretch();
    addCentralWidget(centralWidget);

    // 初始化提示
    ElaMessageBar::success(ElaMessageBarType::BottomRight, "Success", "初始化成功!", 2000);
    qDebug() << "初始化成功";
}

HomePage::~HomePage()
{

}


