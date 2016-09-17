#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include <QtGui>
#include <QDir>
#include <QMessageBox>
//#include <QWindowsXPStyle>
#include "manhattanstyle.h"
#include "stylehelper.h"
#include <QStyleFactory>
#include <QStyle>
#include <QDebug>
#include "models/databasehelper.h"

bool initDatabase(QSqlDatabase& db){
    //db.setDatabaseName(":memory:");
    db.setDatabaseName("myparts.sqlite");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                     "This example needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }
    DatabaseHelper helper(db.database());
    helper.createDatabase();
    helper.loadInitialData();
    return true;
}

int main(int argc, char *argv[])
{

    QStringList keys = QStyleFactory::keys();
    QLatin1String fusionStyleName("Fusion");
    QLatin1String plastiqueStyleName("Plastique");
    //QLatin1String plastiqueStyleName("Oxygen");
    bool fusionFound = false;
    bool plastiqueFound = false;
    for(int i=0; i<keys.size(); ++i){
        if(keys.at(i)==fusionStyleName){
           fusionFound=true;
        }
        else if(keys.at(i)==plastiqueStyleName){
            plastiqueFound = true;
        }
        qDebug()<<"Style: "<< keys.at(i);
    }

    QApplication a(argc, argv);   
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    initDatabase(db);

    QCoreApplication::setOrganizationName("joaoassuncao");
    QCoreApplication::setOrganizationDomain("joaoassuncao.com");
    QCoreApplication::setApplicationName("MyParts");
    //TODO: Fetch values from settings
    //QLocale curLocale(QLocale("pt_PT"));
    //QLocale::setDefault(curLocale);

    QDir::addSearchPath("packages", "/home/jassuncao/MyProjects/myparts/images/packages");
    QDir::addSearchPath("attachments", "/home/jassuncao/MyProjects/myparts/attachments");
    QDir::addSearchPath("tmp","/home/jassuncao/MyProjects/myparts/images/tmp");
    MainWindow w;
    if(fusionFound){
        QApplication::setStyle(fusionStyleName);
    }
    else if(plastiqueFound){
         QApplication::setStyle(plastiqueStyleName);
    }
    //QApplication::setStyle(QLatin1String("fusion"));
    //QApplication::setStyle(QLatin1String("Plastique"));
    ManhattanStyle * style = new ManhattanStyle(QApplication::style()->objectName());
    Manhattan::Utils::StyleHelper::setBaseColor(QColor(0x66,0x66,0x66));
    QApplication::setStyle(style);
    w.show();
    int res = a.exec();    
    db.close();;
    return res;
}


