#ifndef MODELSPROVIDER_H
#define MODELSPROVIDER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QIcon>

#include "project/model/projecttablemodel.h"
#include "stock/partstockmimedata.h"

class CategoryTreeModel;
class StorageTreeModel;
class PartsSqlTableModel;
class PartsQueryBuilder;
class PartConditionModel;
class TreeItem;
class PartDistributorTableModel;
class PartManufacturerTableModel;
class CompanyTableModel;
class PackageTableModel;
class QStandardItemModel;
class IconsRepository;
class QAbstractItemModel;
class PartStockTableModel2;
class PartUnitCache;

class ModelsRepository : public QObject
{
    Q_OBJECT
public:
    explicit ModelsRepository(QObject *parent = 0);
    virtual ~ModelsRepository();
    void initModels();
    CategoryTreeModel * partCategoryModel() const;
    StorageTreeModel * partStorageModel() const;
    PartConditionModel * partConditionModel() const;
    PartsSqlTableModel * partsModel() const;
    CompanyTableModel * distributorModel() const;
    CompanyTableModel * manufacturerModel() const;
    PackageTableModel * packageModel() const;
    ProjectTableModel * projectModel() const;
    IconsRepository * storageIconsRepository() const;
    IconsRepository * categoryIconsRepository() const;
    PartUnitCache * partUnitCache() const;
    void partsDroppedInStorage(QVector<int> parts, TreeItem* item);
signals:

private slots:
    void slotPartsDroppedInCategory(QVector<int> parts, TreeItem* item);    
    void slotStockDroppedInStorage(const QList<PartStockItem> & items, const QVariant & storageId);
private:
    void loadTreeIcons();
    CategoryTreeModel * _categoriesModel;
    StorageTreeModel * _storageModel;
    PartsSqlTableModel * _partsModel;
    PartsQueryBuilder * _partsQueryBuilder;
    PartConditionModel * _partConditionModel;
    PartDistributorTableModel * _partDistributorModel;
    PartManufacturerTableModel * _partManufacturerModel;
    CompanyTableModel * _distributorModel;
    CompanyTableModel * _manufacturerModel;
    PackageTableModel * _packageModel;    
    ProjectTableModel * _projectModel;
    IconsRepository * _categoryIconsRepository;
    IconsRepository * _storageIconsRepository;
    PartStockTableModel2 * _stockModelHelper;
    PartUnitCache * _partUnitCache;
};

#endif // MODELSPROVIDER_H
