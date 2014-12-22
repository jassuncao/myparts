#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include <QtGui>
#include <dquest.h>
#include "entities/entities.h"
#include "partcategorydialog.h"
#include <QWindowsXPStyle>

bool createTables(DQConnection & connection)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
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
    connection.open(db); // Establish the connection to database. It will become the "default connection" shared by all DQModel    
    connection.addModel<Entities::PartUnitEntity>();
    connection.addModel<Entities::PartStorageEntity>();
    connection.addModel<Entities::PartCategoryEntity>();
    connection.addModel<Entities::PartConditionEntity>();
    connection.addModel<Entities::PartFootprintEntity>();
    connection.addModel<Entities::PartEntity>();
    connection.addModel<Entities::StockChangeEntity>();
    if(connection.createTables()){
        DQIndex<Entities::PartCategoryEntity> lft_idx("lft_idx");
        DQIndex<Entities::PartCategoryEntity> rgt_idx("rgt_idx");
        lft_idx<<"lft";
        rgt_idx<<"rgt";
        connection.createIndex(lft_idx);
        connection.createIndex(rgt_idx);
        return true;
    }
    return false;

}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    DQConnection connection;
    if(createTables(connection)){

        //createSomeData();
        //listSomeData();
        //createSampleCategories();
    }
    connection.query().exec("PRAGMA foreign_keys = ON");
    QCoreApplication::setOrganizationName("joaoassuncao");
    QCoreApplication::setOrganizationDomain("joaoassuncao.com");
    QCoreApplication::setApplicationName("MyParts");
    MainWindow w;   
    //QApplication::setStyle(new ManhattanStyle(QApplication::style()->objectName()));
    w.show();
    int res = a.exec();    
    connection.close();
    return res;
    //*/
}


