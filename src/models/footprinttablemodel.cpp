#include "footprinttablemodel.h"
#include <QDebug>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include "utils.h"

FootprintTableModel::FootprintTableModel(QObject *parent, QSqlDatabase db) :
                                         QSqlTableModel(parent, db)
{
    setTable("part_footprint");
}

FootprintTableModel::~FootprintTableModel()
{
}

/*
QString FootprintTableModel::selectStatement() const
{
    QString query = QLatin1String("SELECT id, name, description, imageFilename, imageFilename AS imageFilenameOriginal FROM part_footprint ");
    if (!filter().isEmpty())
        query.append(QLatin1String(" WHERE ")).append(filter());
    QString orderBy(orderByClause());
    if (!orderBy.isEmpty())
        query.append(QLatin1Char(' ')).append(orderBy);
    return query;
}
*/
/*
bool FootprintTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role==Qt::EditRole && index.column()==ColumnFilename){

    }
}
*/

QString copyFileToFootprintsDir(const QString & srcPath)
{
    QDir targetDir = QDir(QDir::searchPaths("footprints").first());
    return copyFileToDir(srcPath, targetDir);
}

bool FootprintTableModel::insertRowIntoTable(const QSqlRecord &values)
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
    QVariant filename = values.value(ColumnFilename);
    if(!filename.isNull() && filename.isValid()){
        QString fName = filename.toString();
        if(fName.startsWith("footprints:") || fName.startsWith(":")  || fName.isEmpty()){
            //nothing to do
        }
        else{
            QString finalFilename = copyFileToFootprintsDir(fName);
            finalFilename = QString("footprints:%1").arg(finalFilename);
            qDebug()<<"Final filename is "<<finalFilename;
            //We need to create a copy due to const
            QSqlRecord rec(values);
            rec.setValue(ColumnFilename, QVariant(finalFilename));
            bool res = QSqlTableModel::insertRowIntoTable(rec);
            if(!res){
                qWarning()<<"Error saving footprint:"<<lastError().text();
            }
            return res;
        }
    }
    return QSqlTableModel::insertRowIntoTable(values);
}

/*
void FootprintTableModel::handleFilenameChanges(QSqlRecord &rec)
{
    QString fileName = rec.value(ColumnFilename).toString();
    if(fileName.startsWith("footprints:") || fileName.startsWith(":")){
        //nothing to do
    }
    else {
        QString targetDir = QDir::searchPaths("footprints").first();
        qDebug()<<"Copying file "<<fileName<<" to footprints dir "<<targetDir;
        //If the file is in temporary directory we move it
        bool moveFile = fileName.startsWith("tmp:");
        QFile srcFile(fileName);
        QString finalFilename = copyFileToDir(srcFile, targetDir, moveFile);
        finalFilename = QString("footprints:%1").arg(finalFilename);
        qDebug()<<"Final filename is "<<finalFilename;
        rec.setValue(ColumnFilename, QVariant(finalFilename));
    }
}
*/

bool FootprintTableModel::updateRowInTable(int row, const QSqlRecord &values)
{
#ifdef QT_DEBUG
    for(int i=0; i<values.count(); ++i){
        qDebug()<<"Value to update "<<values.fieldName(i)<<" to "<< values.value(i);
    }
#endif
    //If the footprint image changed we might need to delete the previous one. We need the original value to handle this situation
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
            if(fName.startsWith("footprints:") || fName.startsWith(":") || fName.isEmpty()){
                //nothing to do
            }
            else{
                QString finalFilename = copyFileToFootprintsDir(fName);
                finalFilename = QString("footprints:%1").arg(finalFilename);
                qDebug()<<"Final filename is "<<finalFilename;
                //We need to create a copy due to const
                QSqlRecord rec(values);
                rec.setValue(ColumnFilename, QVariant(finalFilename));
                bool res = QSqlTableModel::updateRowInTable(row, rec);
                if(!res){
                    qWarning()<<"Update Error in row "<<row<<" "<<lastError().text();
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

bool FootprintTableModel::deleteRowFromTable(int row)
{
    QModelIndex index = QSqlQueryModel::index(row, ColumnFilename);
    QVariant filename = QSqlQueryModel::data(index);
    bool res = QSqlTableModel::deleteRowFromTable(row);
    if (res){
        //We need to remove the file from the filesystem
        if(!filename.isNull() && filename.isValid()){
            QString fName = filename.toString();
            if(fName.startsWith("footprints:")){
                QFile file(fName);
                qDebug("Removing file");
                if(!file.remove()){
                    qWarning()<<"Failed to remove file "<<fName<<" error "<< file.errorString();
                }
            }
        }
    }
    else{
        qWarning()<<"Failed to remove row";
    }
    return res;
}

