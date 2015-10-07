#include "utils.h"
#include <QFileInfo>
#include <QDebug>

QString copyFileToDir(const QString &srcPath, const QDir & targetDir)
{
    qDebug()<<"Copying file "<<srcPath<<" to footprints dir "<<targetDir.absolutePath();

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
