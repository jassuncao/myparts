#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDir>
#include <QSqlError>

class QAbstractItemModel;

namespace Utils {

inline QByteArray asByteArray(const QString & str)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return str.toLatin1();
#else
    return str.toAscii();
#endif
}

    QString getDocumentsDirectory();
    QString copyFileToDir(const QString &srcPath, const QDir & targetDir);
    int findDefaultValueRow(const QAbstractItemModel *model, int column);
    void reportDatabaseError(QWidget *parent, const QString &title, const QString &text, const QSqlError &sqlError);
}


#endif // UTILS_H
