#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QMessageBox>
#include "modelsrepository.h"

#include "core/sql/customtablemodel.h"
#include "core/sql/customtablemodel.h"
#include "core/treemodel/treeitem.h"
#include "core/sql/basicentitytablemodel.h"
#include "core/iconsrepository.h"
#include "project/model/projecttablemodel.h"
#include "stock/model/partstocktablemodel2.h"
#include "stock/model/partstocklogtablemodel.h"
#include "models/partunitcache.h"
#include "part/model/partssqltablemodel.h"
#include "part/model/storagetreemodel.h"
#include "part/model/partdistributortablemodel.h"
#include "part/model/partmanufacturertablemodel.h"
#include "part/model/partsquerybuilder.h"
#include "part/model/partconditionmodel.h"
#include "part/model/categorytreemodel.h"


ModelsRepository::ModelsRepository(QObject *parent) : QObject(parent)
//    _storageIconsModel(0),
//    _categoryIconsModel(0)
{
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsModel = new PartsSqlTableModel(_partsQueryBuilder, this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    _categoriesModel = new CategoryTreeModel(this);
    _storageModel = new StorageTreeModel(this);
    _partConditionModel = PartConditionModel::createNew(this);
    _partConditionModel->setObjectName("Part condition model");
    _partDistributorModel = PartDistributorTableModel::createNew(this);
    _partManufacturerModel = PartManufacturerTableModel::createNew(this);

    _distributorModel = new CompanyTableModel(this);
    _distributorModel->setObjectName("Distributor model");
    _distributorModel->setTable("distributor");
    _distributorModel->setSort(_distributorModel->fieldIndex("name"), Qt::AscendingOrder);

    _manufacturerModel = new CompanyTableModel(this);
    _manufacturerModel->setObjectName("Manufacturer model");
    _manufacturerModel->setTable("manufacturer");

    _packageModel = new PackageTableModel(this);
    _packageModel->setObjectName("Package model");
    _packageModel->setTable("package");
    _packageModel->setSort(_packageModel->fieldIndex("name"), Qt::AscendingOrder);

    _projectModel = new ProjectTableModel(this);
    _projectModel->setSort(_projectModel->fieldIndex("name"), Qt::AscendingOrder);

    _storageIconsRepository = new IconsRepository(":/storage/storage.xml");
    _storageModel->setIconsRepository(_storageIconsRepository);

    _categoryIconsRepository = new IconsRepository(":/parts/category.xml");
    _categoriesModel->setIconsRepository(_categoryIconsRepository);

    _stockModelHelper = PartStockTableModel2::createNew(this);

    _partUnitCache = new PartUnitCache;

    connect(_categoriesModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInCategory(QVector<int>,TreeItem*)));
    //connect(_storageModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInStorage(QVector<int>,TreeItem*)));
    connect(_storageModel, SIGNAL(stockDropped(QList<PartStockItem>,QVariant)), this, SLOT(slotStockDroppedInStorage(QList<PartStockItem>,QVariant)));
}

ModelsRepository::~ModelsRepository()
{    
    delete _partsQueryBuilder;
    delete _storageIconsRepository;
    delete _categoryIconsRepository;
    delete _partUnitCache;
}

CategoryTreeModel * ModelsRepository::partCategoryModel() const
{
    return _categoriesModel;
}

StorageTreeModel * ModelsRepository::partStorageModel() const
{
    return _storageModel;
}

PartConditionModel * ModelsRepository::partConditionModel() const
{
    return _partConditionModel;
}

PartsSqlTableModel * ModelsRepository::partsModel() const
{
    return _partsModel;
}

CompanyTableModel* ModelsRepository::distributorModel() const
{
    return _distributorModel;
}

CompanyTableModel* ModelsRepository::manufacturerModel() const
{
    return _manufacturerModel;
}

PackageTableModel * ModelsRepository::packageModel() const
{
    return _packageModel;
}

ProjectTableModel * ModelsRepository::projectModel() const
{
    return _projectModel;
}

IconsRepository * ModelsRepository::categoryIconsRepository() const
{
    return _categoryIconsRepository;
}

PartUnitCache *ModelsRepository::partUnitCache() const
{
    return _partUnitCache;
}

IconsRepository * ModelsRepository::storageIconsRepository() const
{
    return _storageIconsRepository;
}

void ModelsRepository::initModels()
{    
    _storageModel->select();
    _categoriesModel->select();
    _partConditionModel->select();
    _partDistributorModel->select();
    _partManufacturerModel->select();
}

void ModelsRepository::slotPartsDroppedInCategory(QVector<int> parts, TreeItem* item)
{
    _partsModel->updatePartsCategory(parts, item->id());
    _partsModel->select();
}

void ModelsRepository::partsDroppedInStorage(QVector<int> parts, TreeItem* item)
{    
    bool res = false;
    QVariant storageId = item->id();
    foreach (int partId, parts) {
        res = res | _stockModelHelper->rawMovePartToStorage(partId, storageId);
    }
    if(res){
        _partsModel->select();
    }
}

void ModelsRepository::slotStockDroppedInStorage(const QList<PartStockItem> & items, const QVariant & storageId)
{
    bool res = false;
    foreach (PartStockItem item, items) {
        res = res | _stockModelHelper->rawMoveStockToStorage(item, storageId);
    }
    if(res){
        _partsModel->select();
    }
}

void ModelsRepository::loadTreeIcons()
{

}

