#ifndef MODELSPROVIDER_H
#define MODELSPROVIDER_H

#include <QObject>
#include <QVector>

class CategoryTreeModel;
class StorageTreeModel;
class PartsSqlTableModel;
class PartsQueryBuilder;
class PartConditionModel;
class TreeItem;
class PartDistributorTableModel2;
class PartManufacturerTableModel2;

class ModelsProvider : public QObject
{
    Q_OBJECT
public:
    explicit ModelsProvider(QObject *parent = 0);
    virtual ~ModelsProvider();
    void initModels();
    CategoryTreeModel * partCategoryModel() const;
    StorageTreeModel * partStorageModel() const;
    PartConditionModel *partConditionModel() const;
    PartsSqlTableModel *partsModel() const;
signals:

private slots:
    void slotPartsDroppedInCategory(QVector<int> parts, TreeItem* item);
    void slotPartsDroppedInStorage(QVector<int> parts, TreeItem* item);
private:
    CategoryTreeModel * _categoriesModel;
    StorageTreeModel * _storageModel;
    PartsSqlTableModel * _partsModel;
    PartsQueryBuilder * _partsQueryBuilder;
    PartConditionModel * _partConditionModel;
    PartDistributorTableModel2 * _partDistributorModel;
    PartManufacturerTableModel2 * _partManufacturerModel;
};

#endif // MODELSPROVIDER_H
