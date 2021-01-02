#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("OOP Group No.3");
    QCoreApplication::setApplicationName("QtMarkdown");
    QCoreApplication::setApplicationVersion("Insider Preview😎");
    MainWindow w;
    w.show();
    return a.exec();
}
