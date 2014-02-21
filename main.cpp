#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
    QCoreApplication::setOrganizationName ("Efremenko_A_V") ;
    QCoreApplication::setApplicationName ("AreaPixAnalizer") ;
    MainWindow w;
    w.show();

    return a.exec();
}
