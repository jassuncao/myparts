#ifndef MODELWITHFOREIGNKEY_H
#define MODELWITHFOREIGNKEY_H

class QAbstractItemModel;

class IModelWithForeignKey {
public:
    virtual ~IModelWithForeignKey() {}
    virtual QAbstractItemModel * relationModel(const int column) const = 0;
};

#endif // MODELWITHFOREIGNKEY_H
