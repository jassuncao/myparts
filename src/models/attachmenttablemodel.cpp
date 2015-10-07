#include "attachmenttablemodel.h"
#include <QDebug>
#include <QSqlError>
#include "utils.h"

AttachmentTableModel::AttachmentTableModel(int filenameColumnIdx, QString targetPrefix, QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    _filenameColumnIdx(filenameColumnIdx),
    _targetPrefix(targetPrefix)
{
    int idx = _targetPrefix.indexOf(':');
    QStringList paths = QDir::searchPaths(_targetPrefix.left(idx));
    _targetDir = QDir(paths.first());
}

AttachmentTableModel::~AttachmentTableModel()
{
}

bool AttachmentTableModel::insertRowIntoTable(const QSqlRecord &values)
{
    /*
     * Before doing the actual insert, we check if the filename starts with the prefix "footprints:"
     * meaning the associated file is already stored in the footprints directory.
     * If is not, we move the file to the footprints directory and update the filename
     */

#ifdef QT_DEBUG
    for(int i=0; i<values.count(); ++i){
        qDebug()<<"Value to insert "<<values.fieldName(i)<<" to "<< values.value(i);
    }
#endif
    QVariant filename = values.value(_filenameColumnIdx);
    if(!filename.isNull() && filename.isValid()){
        QString fName = filename.toString();
        if(fName.startsWith(_targetPrefix) || fName.startsWith(":")  || fName.isEmpty()){
            //nothing to do
        }
        else{
            QString finalFilename = copyFileToDir(fName, _targetDir);
            finalFilename = QString("%1:%2").arg(_targetPrefix, finalFilename);
            qDebug()<<"Final filename is "<<finalFilename;
            //We need to create a copy due to const
            QSqlRecord rec(values);
            rec.setValue(_filenameColumnIdx, QVariant(finalFilename));
            bool res = QSqlTableModel::insertRowIntoTable(rec);
            if(!res){
                qWarning("Failed to insert record. Reason %s",qPrintable(lastError().text()));
            }
            return res;
        }
    }
    return QSqlTableModel::insertRowIntoTable(values);
}

bool AttachmentTableModel::updateRowInTable(int row, const QSqlRecord &values)
{
#ifdef QT_DEBUG
    for(int i=0; i<values.count(); ++i){
        qDebug()<<"Value to update "<<values.fieldName(i)<<" to "<< values.value(i);
    }
#endif
    //If the footprint image changed we might need to delete the previous one. We need the original value to handle this situation
    QModelIndex index = QSqlQueryModel::index(row, _filenameColumnIdx);
    QVariant oldFilename = QSqlQueryModel::data(index);
    QVariant newFilename = values.value(_filenameColumnIdx);
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
            if(fName.startsWith("footprints:") || fName.startsWith(":") || fName.isEmpty()){
                //nothing to do
            }
            else{
                QString finalFilename = copyFileToDir(fName, _targetDir);
                finalFilename = QString("%1:%2").arg(_targetPrefix, finalFilename);
                qDebug()<<"Final filename is "<<finalFilename;
                //We need to create a copy due to const
                QSqlRecord rec(values);
                rec.setValue(_filenameColumnIdx, QVariant(finalFilename));
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

bool AttachmentTableModel::deleteRowFromTable(int row)
{
    QModelIndex index = QSqlQueryModel::index(row, _filenameColumnIdx);
    QVariant filename = QSqlQueryModel::data(index);
    bool res = QSqlTableModel::deleteRowFromTable(row);
    if (res){
        //We need to remove the file from the filesystem
        if(!filename.isNull() && filename.isValid()){
            QString fName = filename.toString();
            if(fName.startsWith(_targetPrefix)){
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

