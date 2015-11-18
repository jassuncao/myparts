#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include <QtGui>
#include <QDir>
#include <QMessageBox>
#include <dquest.h>
#include "entities/entities.h"
#include "partcategorydialog.h"
//#include <QWindowsXPStyle>
#include "manhattanstyle.h"
#include "stylehelper.h"
#include <QStyleFactory>
#include <QStyle>
#include <QDebug>

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
    connection.addModel<Entities::FootprintEntity>();
    connection.addModel<Entities::PartEntity>();
    connection.addModel<Entities::StockChangeEntity>();
    connection.addModel<Entities::SiPrefix>();
    connection.addModel<Entities::Unit>();
    connection.addModel<Entities::UnitPrefix>();
    connection.addModel<Entities::PartParameterEntity>();
    connection.addModel<Entities::DistributorEntity>();
    connection.addModel<Entities::PackagingEntity>();
    connection.addModel<Entities::PartDistributorEntity>();
    connection.addModel<Entities::ManufacturerEntity>();
    connection.addModel<Entities::PartManufacturerEntity>();
    connection.addModel<Entities::PartAttachmentEntity>();
    connection.addModel<Entities::FootprintAttachmentEntity>();
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

    QStringList keys = QStyleFactory::keys();
    QLatin1String fusionStyleName("Fusion");
    QLatin1String plastiqueStyleName("Plastique");
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
    //TODO: Fetch values from settings
    QLocale curLocale(QLocale("pt_PT"));
    QLocale::setDefault(curLocale);
    QDir::addSearchPath("footprints", "/home/jassuncao/MyProjects/myparts/images/footprints");
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
    connection.close();
    return res;
    //*/
}


