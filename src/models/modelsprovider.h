#ifndef MODELSPROVIDER_H
#define MODELSPROVIDER_H

#include <QObject>

class CategoryTreeModel;
class StorageTreeModel;
class PartsSqlTableModel;
class PartsQueryBuilder;
class PartConditionModel;

class ModelsProvider : public QObject
{
    Q_OBJECT
public:
    explicit ModelsProvider(QObject *parent = 0);
    virtual ~ModelsProvider();
    void initModels();
    CategoryTreeModel * partCategoryModel() const;
    StorageTreeModel * storageTreeModel() const;
    PartConditionModel *partConditionModel() const;
signals:

public slots:
private:
    CategoryTreeModel * _categoriesModel;
    StorageTreeModel * _storageModel;
    PartsSqlTableModel * _partsModel;
    PartsQueryBuilder * _partsQueryBuilder;
    PartConditionModel * _partConditionModel;
};

#endif // MODELSPROVIDER_H
