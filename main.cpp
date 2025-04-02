#include <QtWidgets/QApplication>
#include "ElaApplication.h"
#include <QFontDatabase>
#include <QDir>
#include <QMessageBox>
#include "GLCore.h"
#include <QImage>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    eApp->init();
    // 设置云母效果图片
    eApp->setMicaImagePath("MicaBase.png");
    GLCore w(360, 480);
    w.show();
    return a.exec();
}
