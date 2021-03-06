#include "utils.h"
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <QModelIndex>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

namespace Utils {

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

void reportDatabaseError(QWidget* parent, const QString &title, const QString& text, const QSqlError& sqlError)
{
    QMessageBox::critical(parent, title, text);
}

QString getDocumentsDirectory()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
}

}//namespace
