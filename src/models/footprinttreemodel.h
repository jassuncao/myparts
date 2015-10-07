#ifndef FOOTPRINTTREEMODEL_H
#define FOOTPRINTTREEMODEL_H
#include "treeitemmodel.h"

class FootprintTreeModel : public TreeItemModel
{
public:
    FootprintTreeModel(const QVector<QVariant> columns, QObject *parent = 0);
    ~FootprintTreeModel();
};

#endif // FOOTPRINTTREEMODEL_H
