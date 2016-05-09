#ifndef MODELWITHFOREIGNKEY_H
#define MODELWITHFOREIGNKEY_H

class QAbstractListModel;

class IModelWithForeignKey {
public:
    virtual ~IModelWithForeignKey() {}
    virtual QAbstractListModel * relationModel(const int column) const = 0;
};

#endif // MODELWITHFOREIGNKEY_H
