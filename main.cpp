#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName ("Efremenko_A_V") ;
    QCoreApplication::setApplicationName ("AreaPixAnalizer") ;
    MainWindow w;
    w.show();

    return a.exec();
}
