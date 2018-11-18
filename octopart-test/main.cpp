#include "mainwindow.h"
#include <QApplication>
#include <QNetworkProxy>
#include <QStyleFactory>
#include <QStyle>
#include "octopartimportdialog.h"

int main(int argc, char *argv[])
{
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, QString("127.0.0.1"), 3128);
    QNetworkProxy::setApplicationProxy(proxy);
    QApplication a(argc, argv);
    QStringList keys = QStyleFactory::keys();
    for(int i=0; i<keys.size(); ++i){

        qDebug()<<"Style: "<< keys.at(i);
    }

    QApplication::setStyle("Plastique");
    //QApplication::setStyle("Fusion");
    //QApplication::setStyle("Oxygen");
    QApplication::setStyle("Cleanlooks");
    //QApplication::setStyle("GTK+");
    //QApplication::setStyle("Phase");
    qDebug()<<"In use "<< QApplication::style()->objectName();

    //MainWindow w;
    //w.show();
    OctopartImportDialog dlg;
    dlg.show();

    return a.exec();
}
