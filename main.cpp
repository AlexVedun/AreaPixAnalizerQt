#include <QtPlugin>
#include <QtWidgets/QApplication>
#include "mainwindow.h"

#ifdef QT_STATIC
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QSvgPlugin)
Q_IMPORT_PLUGIN(QSvgIconPlugin)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName ("Efremenko_A_V") ;
    QCoreApplication::setApplicationName ("AreaPixAnalizer") ;
    MainWindow w;
    w.show();

    return a.exec();
}
