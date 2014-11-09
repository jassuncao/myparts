#include <QModelIndex>
#include "partdialog.h"
#include "ui_partdialog.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include "parts/partssqlquerymodel2.h"
#include "category/categorytreemodel.h"
#include <QTreeView>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>
#include <QSettings>
#include <QComboBox>
#include "quickaddstoragedialog.h"

PartDialog::PartDialog(QSqlTableModel *model, bool addMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartDialog)
{
    ui->setupUi(this);

    QSqlQueryModel * conditionModel = new QSqlQueryModel();
    conditionModel->setQuery("SELECT id, value, defaultCondition FROM part_condition ORDER BY value ASC");
    ui->partConditionCombo->setModel(conditionModel);
    ui->partConditionCombo->setModelColumn(1);

    QSqlQueryModel * unitsModel = new QSqlQueryModel();
    unitsModel->setQuery("SELECT id, name, defaultUnit FROM part_unit ORDER BY name ASC");
    ui->partUnitCombo->setModel(unitsModel);
    ui->partUnitCombo->setModelColumn(1);

    QSqlQueryModel * storageModel = new QSqlQueryModel();
    storageModel->setQuery("SELECT id, name FROM part_storage ORDER BY name ASC");
    ui->partStorageCombo->setModel(storageModel);
    ui->partStorageCombo->setModelColumn(1);
    //ui->partStorageCombo->setEditable(true);
    ui->partStorageCombo->setAutoCompletion(true);
    //ui->partStorageCombo->setAutoCompletionCaseSensitivity(Qt::CaseInsensitive);

    _mapper = new QDataWidgetMapper(this);
    _mapper->setModel(model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    _mapper->addMapping(ui->partNameEdit, PartsSqlQueryModel2::ColumnName);
    _mapper->addMapping(ui->partDescriptionEdit, PartsSqlQueryModel2::ColumnDescription);    
    _mapper->addMapping(ui->minStockSpinBox, PartsSqlQueryModel2::ColumnMinStock);

    if(addMode){
        QSettings settings;
        QString defaultCurrency(QChar(8364));
        QString currency = settings.value("currency/symbol", defaultCurrency).toString();
        bool currencyAfter = settings.value("currency/after",true).toBool();
        if(currencyAfter)
            ui->priceSpinBox->setSuffix(currency);
        else
            ui->priceSpinBox->setPrefix(currency);
        _mapper->addMapping(ui->initialStockSpinBox,PartsSqlQueryModel2::ColumnActualStock,"value");

        /*
        //Find the defaut unit in partUnitCombo
        QModelIndex start = unitsModel->index(0, 2);
        QModelIndexList res = unitsModel->match(start,Qt::EditRole,QVariant(1));
        if(res.size()>0){            
            qDebug()<<"I="<<res.first().row();
            ui->partUnitCombo->setCurrentIndex(res.first().row());
            qDebug()<<"curr="<< ui->partUnitCombo->currentIndex();
        }        

        //Do the same for the condition combo
        start = conditionModel->index(0, 2);
        res = conditionModel->match(start,Qt::EditRole,QVariant(1));
        if(res.size()>0){
            qDebug()<<"c="<<res.first().row();
            ui->partConditionCombo->setCurrentIndex(res.first().row());
        }
        */
    }
    else{
        ui->priceLabel->setHidden(true);
        ui->priceSpinBox->setHidden(true);
        ui->priceSpinBox->setEnabled(false);
        ui->pricePerItemCheckBox->setHidden(true);
        ui->initialStockLabel->setHidden(true);
        ui->initialStockSpinBox->setHidden(true);
    }
}

PartDialog::~PartDialog()
{
    delete ui;
}

int PartDialog::initialStock() const
{
    return ui->initialStockSpinBox->value();
}


double PartDialog::partPrice() const
{
    double price = ui->priceSpinBox->value();
    if(!ui->pricePerItemCheckBox->isChecked()){
        double quantity = initialStock();
        if(quantity!=0)
            price = price / quantity;
       else
            price = 0;
    }
    return price;
}

void setIndexForFkey(QComboBox * combo, QVariant fkey){
    int comboIdx = -1;
    if(!fkey.isNull() ){
        QModelIndex start = combo->model()->index(0,0);
        QModelIndexList res = combo->model()->match(start, Qt::EditRole, fkey);
        if(!res.isEmpty()){
            comboIdx = res.first().row();
        }
    }
    combo->setCurrentIndex(comboIdx);
}

void setIndexForFkey(QComboBox * combo, QModelIndex index, int column){
    int comboIdx = -1;
    const QAbstractItemModel * mainModel = index.model();
    QModelIndex fkeyIdx = mainModel->index(index.row(), column);
    QVariant fkey = fkeyIdx.data(Qt::EditRole);
    if(!fkey.isNull() ){
        QModelIndex start = combo->model()->index(0,0);
        QModelIndexList res = combo->model()->match(start, Qt::EditRole, fkey);
        if(!res.isEmpty()){
            comboIdx = res.first().row();
        }
    }
    combo->setCurrentIndex(comboIdx);
}

void saveToModel(QComboBox * combo, QAbstractItemModel * mainModel, QModelIndex index, int column){
    int row = combo->currentIndex();
    if(row<0){
        return;
    }
    QVariant value = combo->model()->index(row, 0).data(Qt::EditRole);
    QModelIndex fkeyIdx = mainModel->index(index.row(), column);
    mainModel->setData(fkeyIdx, value);
}

//Not used
QModelIndex findIndexForFkey(QComboBox * combo, QModelIndex index, int column){
    const QAbstractItemModel * mainModel = index.model();
    QModelIndex fkeyIdx = mainModel->index(index.row(), column);
    QVariant fkey = fkeyIdx.data(Qt::EditRole);
    qDebug()<<"Fkey: "<<fkey;
    QModelIndex start = combo->model()->index(0,2);
    qDebug()<<"d: "<<start.data();
    QModelIndexList res = combo->model()->match(start, Qt::EditRole, fkey);
    if(!res.isEmpty()){
        return res.first();
    }
    return QModelIndex();
}

void PartDialog::setCurrentModelIndex(const QModelIndex &index)
{
    _currentModelIndex = index;
    _mapper->setCurrentModelIndex(index);    

    setIndexForFkey(ui->partUnitCombo, index, PartsSqlQueryModel2::ColumnPartUnitId);
    setIndexForFkey(ui->partConditionCombo, index, PartsSqlQueryModel2::ColumnConditionId);
    setIndexForFkey(ui->partStorageCombo, index, PartsSqlQueryModel2::ColumnStorageId);
    /*
    QModelIndex catIndex = findIndexForFkey(ui->partCategoryCombo, index, PartsSqlQueryModel2::ColumnCategoryId);
    if(catIndex.isValid()){
        qDebug()<<"catIndex is "<<catIndex;
        ui->partCategoryCombo->setRootModelIndex(catIndex.parent());
        ui->partCategoryCombo->setCurrentIndex(catIndex.row());
    }
    */
}

void PartDialog::on_buttonBox_accepted()
{
    //Check if we are in "add" mode
    if(ui->priceSpinBox->isEnabled()/* && ui->priceSpinBox->value()>0.0*/){
        qDebug()<<"Setting avg price as "<<partPrice();
        QModelIndex priceIdx = _mapper->model()->index(_currentModelIndex.row(), PartsSqlQueryModel2::ColumnAvgPrice);
        _mapper->model()->setData(priceIdx, partPrice());
    }
    saveToModel(ui->partConditionCombo, _mapper->model(), _currentModelIndex, PartsSqlQueryModel2::ColumnConditionId);
    saveToModel(ui->partUnitCombo, _mapper->model(), _currentModelIndex, PartsSqlQueryModel2::ColumnPartUnitId);
    saveToModel(ui->partStorageCombo, _mapper->model(), _currentModelIndex, PartsSqlQueryModel2::ColumnStorageId);
    _mapper->submit();
    //_mapper->model()->submit();
}

void PartDialog::on_quickStorageButton_clicked()
{
    QuickAddStorageDialog dlg(this);
    dlg.setModal(true);
    dlg.setWindowTitle(tr("Add Storage Container"));
    if(dlg.exec()==QDialog::Accepted){
       if(!dlg.addedStorageId().isNull()){
           ui->partStorageCombo->setCurrentIndex(-1);
           QSqlQueryModel * model = static_cast<QSqlQueryModel*>(ui->partStorageCombo->model());
           model->setQuery("SELECT id, name FROM part_storage ORDER BY name ASC");
           setIndexForFkey(ui->partStorageCombo, dlg.addedStorageId());
       }
    }
}
