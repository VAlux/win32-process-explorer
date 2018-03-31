#include "mainwindow.h"
#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    a.setStyle(new QProxyStyle(QStyleFactory::create("fusion")));
    w.show();
    return a.exec();
}
