#include "utils.h"
#include <QFileInfo>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

namespace Utils {

StandardUnit::StandardUnit(const QString name, const QString symbol) :
    _name(name),
    _symbol(symbol)
{ }

StandardUnit::StandardUnit() :
    _name(QString()),
    _symbol(QString())
{
}

QString copyFileToDir(const QString &srcPath, const QDir & targetDir)
{
    qDebug()<<"Copying file "<<srcPath<<" to dir "<<targetDir.absolutePath();

    int i =0;

    QFileInfo fInfo(srcPath);
    QString ext = fInfo.completeSuffix();
    QString basename = fInfo.baseName();
    QString fileName = fInfo.fileName();// ex: file.jpg

    //If the file already exists in the target dir attempt to generate a new one with a numeric suffix
    while(targetDir.exists(fileName)){
        fileName = QString("%1_%2.%3").arg(basename).arg(i).arg(ext);
        ++i;
    }

    QFile srcFile(srcPath);
    const QString & targetPath = targetDir.filePath(fileName);
    if(srcFile.copy(targetPath)){
        qDebug()<<"Copied file from "<<srcPath<<" to "<<targetPath;
        //If the original file is in a temporary directory we will move it
        if(srcPath.startsWith("tmp:")){
            qDebug("Removing file");
            if(!srcFile.remove()){
                qWarning()<<"Failed to remove file "<<srcPath<<" error "<< srcFile.errorString();
            }
        }
        return fileName;
    }
    else{
        qWarning()<<"Failed to copy file "<<srcPath<<" to "<<targetPath;
        return QString();
    }
}

int findDefaultValueRow(const QAbstractItemModel *model, int column)
{
    QModelIndex start = model->index(0, column);
    QModelIndexList res = model->match(start, Qt::EditRole, QVariant(1));
    if(!res.isEmpty())
        return res.first().row();
    qWarning("Default value not found");
    return -1;
}

StandardUnit getStandardUnit(Entities::StandardUnit unit)
{
    QSqlQuery query("SELECT name, symbol FROM unit WHERE id=?");
    query.bindValue(0, QVariant(unit));
    if(query.exec()){
        if(query.next()){
            QString name = query.value(0).toString();
            QString symbol = query.value(1).toString();
            return StandardUnit(name, symbol);
        }
        else{
            qWarning("Unit symbol not found for id %d",unit);
        }
    }
    else{
        qCritical()<<"Failed to execute query. Reason:"<<query.lastError();
    }
    return StandardUnit();
}

QString getStandardUnitSymbol(Entities::StandardUnit unit)
{
    QSqlQuery query("SELECT symbol FROM unit WHERE id=?");
    query.bindValue(0, QVariant(unit));
    if(query.exec()){
        if(query.next()){
            return query.value(0).toString();
        }
        else{
            qWarning("Unit symbol not found for id %d",unit);
        }
    }
    else{
        qCritical()<<"Failed to execute query. Reason:"<<query.lastError();
    }
    return QString();
}

}//namespace
