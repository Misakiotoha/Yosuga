#include <QtWidgets/QApplication>
#include <QFontDatabase>
#include <QDir>
#include <QMessageBox>
#include "GLCore.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GLCore w(360, 480);
    w.show();
    return a.exec();
}
