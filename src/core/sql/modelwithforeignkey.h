#ifndef MODELWITHFOREIGNKEY_H
#define MODELWITHFOREIGNKEY_H

#include <qnamespace.h>

class QAbstractItemModel;

class IModelWithForeignKey {
public:
    static const int ForeignKeyRole = Qt::UserRole+1;
    virtual ~IModelWithForeignKey() {}
    virtual QAbstractItemModel * relationModel(const int column) const = 0;
};

#endif // MODELWITHFOREIGNKEY_H
