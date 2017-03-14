#include <QSqlQueryModel>
#include <QStandardItemModel>
#include "modelsrepository.h"
#include "models/partssqltablemodel.h"
#include "models/storagetreemodel.h"
#include "models/customtablemodel.h"
#include "models/partstocklogtablemodel.h"
#include "models/categorytreemodel.h"
#include "models/customtablemodel.h"
#include "models/partsquerybuilder.h"
#include "models/partconditionmodel.h"
#include "models/treeitem.h"
#include "models/basicentitytablemodel.h"
#include "models/iconsrepository.h"
#include "models/projecttablemodel.h"


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
    _partDistributorModel = PartDistributorTableModel2::createNew(this);
    _partManufacturerModel = PartManufacturerTableModel2::createNew(this);

    _distributorModel = new CompanyTableModel(this);
    _distributorModel->setTable("distributor");
    _distributorModel->setSort(_distributorModel->fieldIndex("name"), Qt::AscendingOrder);

    _manufacturerModel = new CompanyTableModel(this);
    _manufacturerModel->setTable("manufacturer");

    _packageModel = new PackageTableModel(this);
    _packageModel->setTable("package");
    _packageModel->setSort(_packageModel->fieldIndex("name"), Qt::AscendingOrder);

    _projectModel = new ProjectTableModel(this);
    _projectModel->setSort(_projectModel->fieldIndex("name"), Qt::AscendingOrder);

    _storageIconsRepository = new IconsRepository(":/storage/storage.xml");
    _storageModel->setIconsRepository(_storageIconsRepository);

    _categoryIconsRepository = new IconsRepository(":/parts/category.xml");
    _categoriesModel->setIconsRepository(_categoryIconsRepository);

    connect(_categoriesModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInCategory(QVector<int>,TreeItem*)));
    connect(_storageModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInStorage(QVector<int>,TreeItem*)));
}

ModelsRepository::~ModelsRepository()
{    
    delete _partsQueryBuilder;
    delete _storageIconsRepository;
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

IconsRepository * ModelsRepository::storageIconsRepository() const
{
    return _storageIconsRepository;
}

/*
QAbstractItemModel * ModelsRepository::storageIconsModel()
{
    if(_storageIconsModel == 0){
        _storageIconsModel = _storageIconsRepository->model(this);
    }
    return _storageIconsModel;
}

QAbstractItemModel * ModelsRepository::categoriesIconsModel()
{
    if(_categoryIconsModel == 0){
        _categoryIconsModel = _categoryIconsRepository->model(this);
    }
    return _categoryIconsModel;
}
*/

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

void ModelsRepository::slotPartsDroppedInStorage(QVector<int> parts, TreeItem* item)
{
    _partsModel->updatePartsStorage(parts, item->id());
    _partsModel->select();
}

void ModelsRepository::loadTreeIcons()
{

}

