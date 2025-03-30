//
// Created by Administrator on 2025/3/30.
//

#include "RenderPage.h"

#include <QHBoxLayout>
#include <QtWidgets>
#include "ElaComboBox.h"
#include "ElaMessageBar.h"
#include "ElaScrollPageArea.h"
#include "ElaText.h"

#include "GLCore.h"
#include "AppContext.h"
RenderPage::RenderPage(QWidget* parent)
        : BasePage(parent)
{
    // 预览窗口标题
    setWindowTitle("RenderPage");

    frameRateComboBox = new ElaComboBox(this);
    QStringList frameRateComboList = GLCore::getFrameRateList();
    frameRateComboBox->addItems(frameRateComboList);
    ElaScrollPageArea* frameRateComboBoxArea = new ElaScrollPageArea(this);
    QHBoxLayout* frameRateComboBoxLayout = new QHBoxLayout(frameRateComboBoxArea);
    ElaText* frameRateComboBoxText = new ElaText("帧率设置", this);
    frameRateComboBoxText->setTextPixelSize(15);
    frameRateComboBoxLayout->addWidget(frameRateComboBoxText);
    frameRateComboBoxLayout->addWidget(frameRateComboBox);
    frameRateComboBoxLayout->addStretch();
    frameRateComboBoxLayout->addSpacing(10);
    connect(frameRateComboBox, &ElaComboBox::currentTextChanged, this, [this](const QString& text) {
        AppContext::GetGLCore()->setFrameRate(GLCore::getFrameRateMap().value(text));
    });


    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("渲染设置");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addWidget(frameRateComboBoxArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

}

RenderPage::~RenderPage()
{
}




