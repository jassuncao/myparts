#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDir>
#include "entities/entities.h"

class QAbstractItemModel;

namespace Utils {

    class StandardUnit {
    public:
        inline QString name() {return _name;}
        inline QString symbol() {return _symbol;}

        StandardUnit(const QString name, const QString symbol);
        StandardUnit();
    private:
       QString _name;
       QString _symbol;
    };

    QString copyFileToDir(const QString &srcPath, const QDir & targetDir);
    int findDefaultValueRow(const QAbstractItemModel *model, int column);
    StandardUnit getStandardUnit(Entities::StandardUnit unit);
    QString getStandardUnitSymbol(Entities::StandardUnit unit);
}


#endif // UTILS_H
