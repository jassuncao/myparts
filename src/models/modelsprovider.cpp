#include "modelsprovider.h"
#include "models/partssqltablemodel.h"
#include "models/storagetreemodel.h"
#include "models/customtablemodel.h"
#include "models/partstocktablemodel.h"
#include "models/categorytreemodel.h"
#include "models/customtablemodel.h"
#include "models/partsquerybuilder.h"
#include "models/partconditionmodel.h"
#include "models/treeitem.h"
#include <QSqlQueryModel>

ModelsProvider::ModelsProvider(QObject *parent) : QObject(parent)
{
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsModel = new PartsSqlTableModel(_partsQueryBuilder, this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    _categoriesModel = new CategoryTreeModel(this);
    _storageModel = new StorageTreeModel(this);
    _partConditionModel = PartConditionModel::createNew(this);
    _partDistributorModel = PartDistributorTableModel2::createNew(this);
    _partManufacturerModel = PartManufacturerTableModel2::createNew(this);

    connect(_categoriesModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInCategory(QVector<int>,TreeItem*)));
    connect(_storageModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInStorage(QVector<int>,TreeItem*)));
}

ModelsProvider::~ModelsProvider()
{    
    delete _partsQueryBuilder;
}

CategoryTreeModel * ModelsProvider::partCategoryModel() const
{
    return _categoriesModel;
}

StorageTreeModel * ModelsProvider::partStorageModel() const
{
    return _storageModel;
}

PartConditionModel * ModelsProvider::partConditionModel() const
{
    return _partConditionModel;
}

PartsSqlTableModel * ModelsProvider::partsModel() const
{
    return _partsModel;
}

void ModelsProvider::initModels()
{    
    _storageModel->select();
    _categoriesModel->select();
    _partConditionModel->select();
    _partDistributorModel->select();
    _partManufacturerModel->select();
}


void ModelsProvider::slotPartsDroppedInCategory(QVector<int> parts, TreeItem* item)
{
    _partsModel->updatePartsCategory(parts, item->id());
    _partsModel->select();
}

void ModelsProvider::slotPartsDroppedInStorage(QVector<int> parts, TreeItem* item)
{
    _partsModel->updatePartsStorage(parts, item->id());
    _partsModel->select();
}


