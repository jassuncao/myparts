#ifndef MODELSPROVIDER_H
#define MODELSPROVIDER_H

#include <QObject>

class CategoryTreeModel;
class StorageTreeModel;
class PartsSqlTableModel;
class PartsQueryBuilder;
class SimpleSqlTableModel;

class ModelsProvider : public QObject
{
    Q_OBJECT
public:
    explicit ModelsProvider(QObject *parent = 0);
    virtual ~ModelsProvider();
    void initModels();
    void setupPartConditionModel();
    CategoryTreeModel partCategoryModel() const;
signals:

public slots:
private:
    CategoryTreeModel * _categoriesModel;
    StorageTreeModel * _storageModel;
    PartsSqlTableModel * _partsModel;
    PartsQueryBuilder * _partsQueryBuilder;
    SimpleSqlTableModel * _partConditionModel;
};

#endif // MODELSPROVIDER_H
