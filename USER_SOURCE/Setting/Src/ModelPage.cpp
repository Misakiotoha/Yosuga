//
// Created by Administrator on 2025/4/1.
//
#include "ModelPage.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>
#include <QHBoxLayout>
#include <QtWidgets>
#include "ElaComboBox.h"
#include "ElaMessageBar.h"
#include "ElaScrollPageArea.h"
#include "ElaText.h"

#include "LAppLive2DManager.hpp"

ModelPage::ModelPage(QWidget* parent)
        : BasePage(parent)
{
    // 预览窗口标题
    setWindowTitle("ModelPage");


    modelUrlEdit = new ElaLineEdit(this);
    modelUrlEdit->setFixedWidth(300);
    modelUrlEdit->setPlaceholderText("用于显示当前的模型Url");
    modelChoosePushButton = new ElaPushButton("选择模型", this);
    modelChoosePushButton->setToolTip("选择.model3.json结尾的文件");
    modelUsePushButton = new ElaPushButton("使用模型", this);
    modelUsePushButton->setToolTip("使用选择的模型或Url对应的模型");
    ElaScrollPageArea* modelSetArea = new ElaScrollPageArea(this);
    QHBoxLayout* modelSetLayout = new QHBoxLayout(modelSetArea);
    ElaText* modelSetText = new ElaText("模型设置", this);
    modelSetText->setTextPixelSize(15);
    modelSetLayout->addWidget(modelSetText);
    modelSetLayout->addWidget(modelUrlEdit);
    modelSetLayout->addStretch();
    modelSetLayout->addWidget(modelChoosePushButton);
    modelSetLayout->addWidget(modelUsePushButton);
    modelSetLayout->addSpacing(10);
    connect(modelChoosePushButton, &ElaPushButton::clicked, this, [this]() {
        // 获取当前exe所在目录的本地路径
        QString exeDir = QCoreApplication::applicationDirPath();
        // 转换为QUrl格式（自动处理路径分隔符）
        QUrl initialDir = QUrl::fromLocalFile(exeDir);

        // 打开文件选择对话框
        modelFileUrl = QFileDialog::getOpenFileUrl(
                this,
                "选择模型文件",
                initialDir, // 初始目录为当前目录
                "*.model3.json");

        // 检查url是否有效
        if(!modelFileUrl.isEmpty()){
            QString t = modelFileUrl.toLocalFile();
            std::pair<QString, QString> path = this->splitPath(t);
            this->modelFilePathFirst = path.first;
            this->modelFilePathSecond = path.second;
            this->modelUrlEdit->setText(t);
        }
        else{
            ElaMessageBar::information(ElaMessageBarType::BottomRight, "模型设置", "似乎并没有选择模型", 800.0, this);
        }
    });
    connect(modelUsePushButton, &ElaPushButton::clicked, this, [this]() {
        if(!modelFileUrl.isEmpty()){
            LAppLive2DManager::GetInstance()->LoadModelFromPath(this->modelFilePathFirst.toStdString(), this->modelFilePathSecond.toStdString());
        }
        else{
            ElaMessageBar::information(ElaMessageBarType::BottomRight, "模型设置", "似乎并没有选择模型", 800.0, this);
        }
    });


    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("模型商店");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addWidget(modelSetArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

}

// 返回 pair<目录路径, 文件名>
std::pair<QString, QString> ModelPage::splitPath(const QString& fullPath)
{
    QFileInfo fileInfo(fullPath);

    // 获取目录部分（自动处理末尾斜杠）
    QString dirPath = fileInfo.dir().absolutePath() + "/";

    // 获取文件名部分（如果是目录则返回空）
    QString fileName = fileInfo.fileName();

    return {dirPath, fileName};
}

ModelPage::~ModelPage()
{

}