#include "modelsrepository.h"
#include "models/partssqltablemodel.h"
#include "models/storagetreemodel.h"
#include "models/customtablemodel.h"
#include "models/partstocktablemodel.h"
#include "models/categorytreemodel.h"
#include "models/customtablemodel.h"
#include "models/partsquerybuilder.h"
#include "models/partconditionmodel.h"
#include "models/treeitem.h"
#include "models/basicentitytablemodel.h"
#include <QSqlQueryModel>

ModelsRepository::ModelsRepository(QObject *parent) : QObject(parent)
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

    connect(_categoriesModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInCategory(QVector<int>,TreeItem*)));
    connect(_storageModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInStorage(QVector<int>,TreeItem*)));
}

ModelsRepository::~ModelsRepository()
{    
    delete _partsQueryBuilder;
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


