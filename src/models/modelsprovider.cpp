#include "modelsprovider.h"
#include "models/partssqltablemodel.h"
#include "models/storagetreemodel.h"
#include "models/customtablemodel.h"
#include "models/partstocktablemodel.h"
#include "models/categorytreemodel.h"
#include "models/customtablemodel.h"
#include "models/partsquerybuilder.h"
#include "models/partconditionmodel.h"
#include <QSqlQueryModel>

ModelsProvider::ModelsProvider(QObject *parent) : QObject(parent)
{
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsModel = new PartsSqlTableModel(_partsQueryBuilder, this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    _categoriesModel = new CategoryTreeModel(this);
    _storageModel = new StorageTreeModel(this);
    _partConditionModel = PartConditionModel::createNew(this);

}

ModelsProvider::~ModelsProvider()
{
    delete _partsQueryBuilder;
}

CategoryTreeModel * ModelsProvider::partCategoryModel() const
{
    return _categoriesModel;
}

StorageTreeModel * ModelsProvider::storageTreeModel() const
{
    return _storageModel;
}

PartConditionModel * ModelsProvider::partConditionModel() const
{
    return _partConditionModel;
}

void ModelsProvider::initModels()
{    
    _storageModel->select();
    _categoriesModel->select();
    _partConditionModel->select();

}


