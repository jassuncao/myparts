#include "packagetablemodel.h"
#include <QDebug>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include "utils.h"

PackageTableModel::PackageTableModel(QObject *parent, QSqlDatabase db) :
                                         QSqlTableModel(parent, db)
{
    setTable("package");
    _targetDir = QDir(QDir::searchPaths("packages").first());
}

PackageTableModel::~PackageTableModel()
{
}

bool PackageTableModel::insertRowIntoTable(const QSqlRecord &values)
{    
    /*
     * Before doing the actual insert, we check if the filename starts with the prefix "packages:"
     * meaning the associated file is already stored in the packages directory.
     * If is not, we move the file to the packages directory and update the filename
     */

#ifdef QT_DEBUG
    for(int i=0; i<values.count(); ++i){
        qDebug()<<"Value to insert "<<values.fieldName(i)<<" to "<< values.value(i);
    }
#endif
    QVariant filename = values.value(ColumnFilename);
    if(!filename.isNull() && filename.isValid()){
        QString fName = filename.toString();
        if(fName.startsWith("packages:") || fName.startsWith(":")  || fName.isEmpty()){
            //nothing to do
        }
        else{
            QString finalFilename = copyFileToDir(fName, _targetDir);
            finalFilename = QString("packages:%1").arg(finalFilename);
            qDebug()<<"Final filename is "<<finalFilename;
            //We need to create a copy due to const
            QSqlRecord rec(values);
            rec.setValue(ColumnFilename, QVariant(finalFilename));
            bool res = QSqlTableModel::insertRowIntoTable(rec);
            if(!res){
                qWarning()<<"Error saving package:"<<lastError().text();
            }
            return res;
        }
    }
    return QSqlTableModel::insertRowIntoTable(values);
}

bool PackageTableModel::updateRowInTable(int row, const QSqlRecord &values)
{
#ifdef QT_DEBUG
    for(int i=0; i<values.count(); ++i){
        qDebug()<<"Value to update "<<values.fieldName(i)<<" to "<< values.value(i);
    }
#endif
    //If the packages image changed we might need to delete the previous one. We need the original value to handle this situation
    QModelIndex index = QSqlQueryModel::index(row, ColumnFilename);
    QVariant oldFilename = QSqlQueryModel::data(index);
    QVariant newFilename = values.value(ColumnFilename);
    if(oldFilename!=newFilename){
#ifdef QT_DEBUG
        qDebug("Old filename is %s", qPrintable(oldFilename.toString()));
        qDebug("New filename is %s", qPrintable(newFilename.toString()));
        qDebug("Filename column changed");
#endif
        if(oldFilename.isValid()){
            QFile oldFile(oldFilename.toString());
            if(oldFile.exists()){
                qDebug("Removing old file");
                bool r = oldFile.remove();
                if(!r){
                    qWarning("Failed to remove file. Reason:%s", qPrintable(oldFile.errorString()));
                }
                else{
                    qDebug("Removed");
                }
            }
        }

        if(!newFilename.isNull() && newFilename.isValid()){
            QString fName = newFilename.toString();
            if(fName.startsWith("packages:") || fName.startsWith(":") || fName.isEmpty()){
                //nothing to do
            }
            else{
                QString finalFilename = copyFileToDir(fName, _targetDir);
                finalFilename = QString("packages:%1").arg(finalFilename);
                qDebug()<<"Final filename is "<<finalFilename;
                //We need to create a copy due to const
                QSqlRecord rec(values);
                rec.setValue(ColumnFilename, QVariant(finalFilename));
                bool res = QSqlTableModel::updateRowInTable(row, rec);
                if(!res){
                    qWarning("Failed to update record. Reason %s",qPrintable(lastError().text()));
                }
                else{
                    qDebug()<<"Update SUCCESS ";
                }
                return res;
            }
        }
    }
    return QSqlTableModel::updateRowInTable(row, values);
}

bool PackageTableModel::deleteRowFromTable(int row)
{
    QModelIndex index = QSqlQueryModel::index(row, ColumnFilename);
    QVariant filename = QSqlQueryModel::data(index);
    bool res = QSqlTableModel::deleteRowFromTable(row);
    if (res){
        //We need to remove the file from the filesystem
        if(!filename.isNull() && filename.isValid()){
            QString fName = filename.toString();
            if(fName.startsWith("packages:")){
                QFile file(fName);
                qDebug("Removing file");
                if(!file.remove()){
                    qWarning("Failed to remove file %s. Reason %s",qPrintable(fName), qPrintable(file.errorString()));
                }
            }
        }
    }
    else{
        qWarning()<<"Failed to remove row";
    }
    return res;
}

