#ifndef MODELSPROVIDER_H
#define MODELSPROVIDER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QIcon>

class CategoryTreeModel;
class StorageTreeModel;
class PartsSqlTableModel;
class PartsQueryBuilder;
class PartConditionModel;
class TreeItem;
class PartDistributorTableModel2;
class PartManufacturerTableModel2;
class CompanyTableModel;
class PackageTableModel;
class QStandardItemModel;
class IconsRepository;
class QAbstractItemModel;

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
    QAbstractItemModel * storageIconsModel();
    QAbstractItemModel * categoriesIconsModel();
signals:

private slots:
    void slotPartsDroppedInCategory(QVector<int> parts, TreeItem* item);
    void slotPartsDroppedInStorage(QVector<int> parts, TreeItem* item);
private:
    void loadTreeIcons();
    CategoryTreeModel * _categoriesModel;
    StorageTreeModel * _storageModel;
    PartsSqlTableModel * _partsModel;
    PartsQueryBuilder * _partsQueryBuilder;
    PartConditionModel * _partConditionModel;
    PartDistributorTableModel2 * _partDistributorModel;
    PartManufacturerTableModel2 * _partManufacturerModel;
    CompanyTableModel * _distributorModel;
    CompanyTableModel * _manufacturerModel;
    PackageTableModel * _packageModel;    
    IconsRepository * _categoryIconsRepository;
    QAbstractItemModel * _categoryIconsModel;
    IconsRepository * _storageIconsRepository;
    QAbstractItemModel * _storageIconsModel;
};

#endif // MODELSPROVIDER_H
