#include "modelsprovider.h"
#include "models/partssqltablemodel.h"
#include "models/storagetreemodel.h"
#include "models/customtablemodel.h"
#include "models/partstocktablemodel.h"
#include "models/categorytreemodel.h"
#include "models/customtablemodel.h"
#include <QSqlQueryModel>

ModelsProvider::ModelsProvider(QObject *parent) : QObject(parent)
{
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsModel = new PartsSqlTableModel(_partsQueryBuilder, this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    _categoriesModel = new CategoryTreeModel(this);
    _storageModel = new StorageTreeModel(this);

}

virtual ModelsProvider::~ModelsProvider()
{
    delete _partsQueryBuilder;
}

CategoryTreeModel partCategoryModel() const
{
    return _categoriesModel;
}

void ModelsProvider::initModels()
{
    setupPartConditionModel();
    _storageModel->select();
    _categoriesModel->select();

}

void ModelsProvider::setupPartConditionModel()
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<QLatin1String("defaultCondition")<<QLatin1String("value")<<QLatin1String("Description");
    columnNames<<QString()<<tr("Name")<<tr("Description");
    _partConditionModel = new SimpleSqlTableModel("condition", fieldNames, columnNames, QString(), this);
}



