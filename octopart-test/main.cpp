#include "mainwindow.h"
#include <QApplication>
#include <QNetworkProxy>

int main(int argc, char *argv[])
{
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, QString("127.0.0.1"), 3128);
    QNetworkProxy::setApplicationProxy(proxy);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
