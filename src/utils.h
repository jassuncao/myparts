#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDir>

class QAbstractItemModel;

namespace Utils {
    QString copyFileToDir(const QString &srcPath, const QDir & targetDir);
    int findDefaultValueRow(const QAbstractItemModel *model, int column);
}


#endif // UTILS_H
