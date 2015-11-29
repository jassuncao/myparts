#include <QModelIndex>
#include "partdialog.h"
#include "ui_partdialog.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include "models/partssqltablemodel.h"
#include "category/categorytreemodel.h"
#include <QTreeView>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>
#include <QSettings>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QIdentityProxyModel>
#include <QSqlRecord>
#include <QDateTime>
#include "quickaddstoragedialog.h"

#include "widgets/parametervaluedelegate.h"
#include "parts/partsdao.h"
#include "widgets/currencydelegate.h"
#include "attachmentselectiondialog.h"
#include <QDialogButtonBox>
#include <QDesktopServices>
#include "models/customtablemodel.h"

inline static QVariant getColumnValue(QAbstractItemModel * model, int row, int column){
    return model->index(row, column).data(Qt::EditRole);
}

PartDialog::PartDialog(PartsSqlTableModel *model, TreeItemModel *storageModel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartDialog),
    _partsModel(model),
    _storageModel(storageModel),
    _lastSelectedFootprint(-1),
    _addMode(false)
{
    ui->setupUi(this);
    ui->quickStorageButton->setVisible(false);//Not supported yet
    _partParamsModel = PartParametersTableModel3::createNew(this);
    _partDistributorModel = PartDistributorTableModel2::createNew(this);
    _partManufacturerModel = PartManufacturerTableModel2::createNew(this);
    _partAttachmentModel = AttachmentTableModel3::createNewPartAttachmentModel(this);

    _nextActionCheckbox = new QCheckBox(this);
    ui->buttonBox->addButton(_nextActionCheckbox, QDialogButtonBox::HelpRole);
    initCombos();

    _mapper = new QDataWidgetMapper(this);
    _mapper->setModel(_partsModel);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    _mapper->addMapping(ui->partNameEdit, PartsSqlTableModel::ColumnName);
    _mapper->addMapping(ui->partDescriptionEdit, PartsSqlTableModel::ColumnDescription);
    _mapper->addMapping(ui->minStockSpinBox, PartsSqlTableModel::ColumnMinStock);
    _mapper->addMapping(ui->partConditionCombo, PartsSqlTableModel::ColumnConditionId, "currentKey");
    _mapper->addMapping(ui->partUnitCombo, PartsSqlTableModel::ColumnPartUnitId, "currentKey");
    _mapper->addMapping(ui->partFootprintCombo, PartsSqlTableModel::ColumnFootprintId, "currentKey");
    _mapper->addMapping(ui->initialStockSpinBox,PartsSqlTableModel::ColumnActualStock, "value");

    ui->partParametersTableView->setModel(_partParamsModel);
    ui->partParametersTableView->setItemDelegate(new CustomTableRelationDelegate(ui->partParametersTableView));
    ui->partParametersTableView->setItemDelegateForColumn(PartParametersTableModel3::ColumnValue, new ParameterValueDelegate());

    ui->partDistributorsTableView->setModel(_partDistributorModel);
    ui->partDistributorsTableView->setItemDelegateForColumn(PartDistributorTableModel2::ColumnUnitPrice, new CurrencyDelegate(this));
    ui->partDistributorsTableView->setItemDelegate(new CustomTableRelationDelegate(ui->partDistributorsTableView));

    ui->partManufacturerTableView->setModel(_partManufacturerModel);
    ui->partManufacturerTableView->setItemDelegate(new CustomTableRelationDelegate(ui->partManufacturerTableView));  
    ui->partManufacturerTableView->setColumnWidth(PartManufacturerTableModel2::ColumnManufacturer, 256);
    ui->partManufacturerTableView->setColumnWidth(PartManufacturerTableModel2::ColumnPartNumber, 256);

    ui->partAttachmentsTableView->setModel(_partAttachmentModel);
    ui->partAttachmentsTableView->setColumnWidth(0, 512);

    QSettings settings;
    QString defaultCurrency(QChar(8364));
    QString currency = settings.value("currency/symbol", defaultCurrency).toString();
    bool currencyAfter = settings.value("currency/after",true).toBool();
    if(currencyAfter)
        ui->priceSpinBox->setSuffix(currency);
    else
        ui->priceSpinBox->setPrefix(currency);

    ui->deleteParameterButton->setEnabled(false);    

    connect(ui->partFootprintCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotFootprintChanged(int)));
    connect(ui->noneFootprintRadioButton, SIGNAL(clicked()), this, SLOT(slotDeselectFootprint()));
    connect(ui->useFootprintRadioButton, SIGNAL(clicked()), this, SLOT(slotUseFootprint()));
    connect(ui->addParameterButton, SIGNAL(clicked()), this, SLOT(slotAddParameter()));
    connect(ui->deleteParameterButton, SIGNAL(clicked()), this, SLOT(slotDeleteParameter()));        
    connect(ui->partParametersTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentPartParameterRowChanged(QModelIndex,QModelIndex)));
    connect(ui->addDistributorButton, SIGNAL(clicked()), this, SLOT(slotAddDistributor()));
    connect(ui->deleteDistributorButton, SIGNAL(clicked()), this, SLOT(slotDeleteDistributor()));
    connect(ui->partDistributorsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentPartDistributorRowChanged(QModelIndex,QModelIndex)));
    connect(ui->addManufacturerButton, SIGNAL(clicked()), this,SLOT(slotAddManufacturer()));
    connect(ui->deleteManufacturerButton, SIGNAL(clicked()), this,SLOT(slotDeleteManufacturer()));
    connect(ui->partManufacturerTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentPartManufacturerRowChanged(QModelIndex,QModelIndex)));
    connect(ui->addAttachmentButton, SIGNAL(clicked()), this, SLOT(slotAddAttachment()));
    connect(ui->deleteAttachmentButton, SIGNAL(clicked()), this, SLOT(slotDeleteAttachment()));
    connect(ui->partAttachmentsTableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentPartParameterRowChanged(QModelIndex,QModelIndex)));
    connect(ui->partAttachmentsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));
}

PartDialog::~PartDialog()
{
    delete ui;
}

int PartDialog::exec()
{
    return QDialog::exec();
}

int PartDialog::addNewPart()
{
    setWindowTitle(tr("Add Part"));
    _addMode = true;    
    _nextActionCheckbox->setText(tr("Create blank item after save"));
    internalAddPart(QModelIndex());
    return exec();
}

int PartDialog::editPart(const QModelIndex &index)
{
    setWindowTitle(tr("Edit Part"));
    _nextActionCheckbox->setText(tr("Create copy after save"));
    _addMode = false;
    ui->priceLabel->setHidden(true);
    ui->priceSpinBox->setHidden(true);
    ui->priceSpinBox->setEnabled(false);
    ui->pricePerItemCheckBox->setHidden(true);
    ui->initialStockLabel->setHidden(true);
    ui->initialStockSpinBox->setHidden(true);    
    setCurrentModelIndex(index);
    _currentPartId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnId);

    _partParamsModel->setCurrentPartId(_currentPartId);
    _partParamsModel->select();

    _partDistributorModel->setCurrentPartId(_currentPartId);
    _partDistributorModel->select();

    _partManufacturerModel->setCurrentPartId(_currentPartId);
    _partManufacturerModel->select();

    _partAttachmentModel->setCurrentForeignKey(_currentPartId);
    _partAttachmentModel->select();
    return exec();
}

int PartDialog::duplicatePart(const QModelIndex &index)
{
    _addMode = true;
    internalAddPart(index);
    return exec();
}

void PartDialog::internalAddPart(const QModelIndex &index){
    int newRow = _partsModel->rowCount();

    if(index.isValid()){
        //When add mode is used and the index is valid, duplicate the part

        //Make a copy of the existing part parameters
        QVariant partId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnId);

        //Make a copy of the part attributes
        QSqlRecord copy = _partsModel->record(index.row());
        copy.setNull(PartsSqlTableModel::ColumnId);
        copy.setNull(PartsSqlTableModel::ColumnActualStock);

        //Set the createDate field
        QDateTime now = QDateTime::currentDateTimeUtc();
        QVariant t = QVariant(now.toTime_t());
        copy.setValue(PartsSqlTableModel::ColumnCreateDate, t);
        //Add a new record with values from the duplicate part
        _partsModel->insertRecord(newRow,copy);

        setCurrentModelIndex(_partsModel->index(newRow, PartsSqlTableModel::ColumnId));
        QVariant invalidId;
        _partParamsModel->setCurrentPartId(partId);
        _partParamsModel->select();
        _partParamsModel->cloneData();
        _partParamsModel->setCurrentForeignKey(invalidId);

        _partDistributorModel->setCurrentPartId(partId);
        _partDistributorModel->select();
        _partDistributorModel->cloneData();
        _partDistributorModel->setCurrentForeignKey(invalidId);

        _partManufacturerModel->setCurrentPartId(partId);
        _partManufacturerModel->select();
        _partManufacturerModel->cloneData();
        _partManufacturerModel->setCurrentForeignKey(invalidId);

        _partAttachmentModel->setCurrentForeignKey(partId);
        _partAttachmentModel->select();
        _partAttachmentModel->cloneData();
        _partAttachmentModel->setCurrentForeignKey(invalidId);
    }
    else{
        _partsModel->insertRow(newRow);        
        setCurrentModelIndex(_partsModel->index(newRow, PartsSqlTableModel::ColumnId));
        reset();
    }
}

int findDefaultValueIndex(const QSqlQueryModel * model, int column)
{
    QModelIndex start = model->index(0, column);
    QModelIndexList res = model->match(start, Qt::EditRole, QVariant(1));
    if(!res.isEmpty())
        return res.first().row();
    qWarning("Default value not found");
    return -1;
}

void PartDialog::initCombos()
{
    ui->partStorageCombo->setModel(_storageModel);
    connect(ui->partStorageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPartStorageChanged(int)));

    _partConditionModel = new QSqlQueryModel();
    _partConditionModel->setQuery("SELECT id, value, defaultCondition FROM part_condition ORDER BY value ASC");
    ui->partConditionCombo->setModel(_partConditionModel);
    ui->partConditionCombo->setModelKeyColumn(0);
    ui->partConditionCombo->setModelColumn(1);

    _partUnitsModel = new QSqlQueryModel();
    _partUnitsModel->setQuery("SELECT id, name, defaultUnit FROM part_unit ORDER BY name ASC");
    ui->partUnitCombo->setModel(_partUnitsModel);
    ui->partUnitCombo->setModelKeyColumn(0);
    ui->partUnitCombo->setModelColumn(1);

    _footprintsModel = new QSqlQueryModel();
    _footprintsModel->setQuery("SELECT id, name FROM part_footprint ORDER BY name ASC");
    ui->partFootprintCombo->setModel(_footprintsModel);
    ui->partFootprintCombo->setModelKeyColumn(0);
    ui->partFootprintCombo->setModelColumn(1);
}

void PartDialog::reset()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->partNameEdit->setFocus();    
    ui->partCategoryCombo->setCurrentIndex(-1);
    ui->partStorageCombo->setCurrentIndex(QModelIndex());

    QVariant invalidId;
    _partParamsModel->setCurrentPartId(invalidId);
    _partParamsModel->select();

    _partDistributorModel->setCurrentPartId(invalidId);
    _partDistributorModel->select();

    _partManufacturerModel->setCurrentPartId(invalidId);
    _partManufacturerModel->select();

    _partAttachmentModel->setCurrentForeignKey(invalidId);
    _partAttachmentModel->select();

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

void PartDialog::setCurrentModelIndex(const QModelIndex &index)
{
    _currentModelIndex = index;
    _mapper->setCurrentModelIndex(index);

    int row = index.isValid() ? index.row() : -1;
    _lastSelectedFootprint = ui->partFootprintCombo->currentIndex();
    if(_lastSelectedFootprint<0){
        ui->noneFootprintRadioButton->setChecked(true);
    }
    else{
        ui->useFootprintRadioButton->setChecked(true);
    }

    QVariant fkey = getColumnValue(_partsModel, row, PartsSqlTableModel::ColumnStorageId);
    if(!fkey.isNull() ){                
        QModelIndex treeIndex = _storageModel->findIndex(fkey.toInt());
        if(treeIndex.isValid()){            
            ui->partStorageCombo->setCurrentIndex(treeIndex);
        }       
    }  
}

void PartDialog::accept()
{
    commitChanges();
    if(_nextActionCheckbox->isChecked()){
        if(_addMode){
            //In add mode we create a blank item            
            internalAddPart(QModelIndex());
            reset();
        }
        else{
            reset();
            internalAddPart(_currentModelIndex);            
            //In edit mode we make a duplicate
        }
    }
    else{
        QDialog::accept();
    }
}

void PartDialog::reject()
{
    discardChanges();
    QDialog::reject();
}

void PartDialog::commitChanges()
{    
    _partsModel->database().transaction();
    _mapper->submit();
    if(_addMode){
        qDebug("Commiting changes to new part");
        qDebug()<<"Setting avg price as "<<partPrice();
        QModelIndex priceIdx = _partsModel->index(_currentModelIndex.row(), PartsSqlTableModel::ColumnAvgPrice);
        _partsModel->setData(priceIdx, partPrice());
        _partsModel->submitAll();
        QVariant partId = _partsModel->lastInsertedId();
        if(partId.isValid()){
            qDebug()<<"New Part id is "<<partId;
            if(initialStock()>0){
                qDebug("Adding initial stock");
                PartsDAO::addStock(partId, initialStock(), partPrice(), QString());
            }
            _partParamsModel->setCurrentPartId(partId);      
            _partDistributorModel->setCurrentPartId(partId);
            _partManufacturerModel->setCurrentPartId(partId);
            _partAttachmentModel->setCurrentForeignKey(partId);
        }
    }
    else{
        QVariant partId = getColumnValue(_partsModel, _currentModelIndex.row(), PartsSqlTableModel::ColumnId);
        qDebug()<<"Commiting changes to existing part "<<partId;
        _partsModel->submitAll();             
    }
    qDebug("Saving part manufacturers");
    _partManufacturerModel->submitAll();
    qDebug("Saving part distributors");
    _partDistributorModel->submitAll();
    qDebug("Saving part attachments");
    _partAttachmentModel->submitAll();
    qDebug("Saving part parameters");
    _partParamsModel->submitAll();

    _partsModel->database().commit();
}

void PartDialog::discardChanges()
{
    _partsModel->revertAll();
}

void PartDialog::on_quickStorageButton_clicked()
{
//    QuickAddStorageDialog dlg(this);
//    dlg.setModal(true);
//    dlg.setWindowTitle(tr("Add Storage Container"));
//    if(dlg.exec()==QDialog::Accepted){
//       if(!dlg.addedStorageId().isNull()){
//           ui->partStorageCombo->setCurrentIndex(QModelIndex());
//           QSqlQueryModel * model = static_cast<QSqlQueryModel*>(ui->partStorageCombo->model());
//           model->setQuery("SELECT id, name FROM part_storage ORDER BY name ASC");
//           setIndexForFkey(ui->partStorageCombo, dlg.addedStorageId());
//       }
//    }
}

void PartDialog::slotFootprintChanged(int index)
{
    if(index<0){
        ui->noneFootprintRadioButton->setChecked(true);
    }
    else{
        ui->useFootprintRadioButton->setChecked(true);
    }
}

void PartDialog::slotDeselectFootprint(){   
    _lastSelectedFootprint = ui->partFootprintCombo->currentIndex();
    ui->partFootprintCombo->setCurrentIndex(-1);
}

void PartDialog::slotUseFootprint(){
    ui->partFootprintCombo->setCurrentIndex(_lastSelectedFootprint);
}

void PartDialog::slotAddParameter(){    
    int rowCount = _partParamsModel->rowCount();
    if(_partParamsModel->insertRow(rowCount)){
        QModelIndex index = _partParamsModel->index(rowCount,PartParametersTableModel3::ColumnName);
        ui->partParametersTableView->setCurrentIndex(index);
        ui->partParametersTableView->edit(index);
    }    
}

void PartDialog::slotDeleteParameter(){    
    QModelIndex index = ui->partParametersTableView->currentIndex();
    if(index.isValid()){
        qDebug()<<"Removing row";
        bool res = ui->partParametersTableView->model()->removeRow(index.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
    }    
}

void PartDialog::slotCurrentPartParameterRowChanged(const QModelIndex &current, const QModelIndex&)
{   
    ui->deleteParameterButton->setEnabled(current.isValid());
}

void PartDialog::slotAddDistributor(){
    int rowCount = _partDistributorModel->rowCount();
    if(_partDistributorModel->insertRow(rowCount)){        
        QModelIndex index = _partDistributorModel->index(rowCount, PartDistributorTableModel2::ColumnDistributor);
        ui->partDistributorsTableView->setCurrentIndex(index);
        ui->partDistributorsTableView->edit(index);
    }
}

void PartDialog::slotDeleteDistributor(){
    QModelIndex index = ui->partDistributorsTableView->currentIndex();
    if(index.isValid()){
        qDebug()<<"Removing row";
        bool res = ui->partDistributorsTableView->model()->removeRow(index.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
    }
}

void PartDialog::slotCurrentPartDistributorRowChanged(const QModelIndex &current, const QModelIndex&)
{
    ui->deleteDistributorButton->setEnabled(current.isValid());
}

void PartDialog::slotAddManufacturer(){
    int rowCount = _partManufacturerModel->rowCount();
    if(_partManufacturerModel->insertRow(rowCount)){
        QModelIndex index = _partManufacturerModel->index(rowCount, PartManufacturerTableModel2::ColumnManufacturer);
        ui->partManufacturerTableView->setCurrentIndex(index);
        ui->partManufacturerTableView->edit(index);
    }    
}

void PartDialog::slotDeleteManufacturer(){
    QModelIndex index = ui->partManufacturerTableView->currentIndex();
    if(index.isValid()){
        qDebug()<<"Removing row";
        bool res = ui->partManufacturerTableView->model()->removeRow(index.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
    }
}

void PartDialog::slotCurrentPartManufacturerRowChanged(const QModelIndex &current, const QModelIndex&)
{
    ui->deleteManufacturerButton->setEnabled(current.isValid());
}

void PartDialog::slotAddAttachment(){
    AttachmentSelectionDialog dlg(this);
    if(dlg.exec()){
        QUrl resourceUrl = dlg.value();
        _partAttachmentModel->appendRow(resourceUrl.toString(), QString());
    }
}

void PartDialog::slotDeleteAttachment(){
    QModelIndex index = ui->partAttachmentsTableView->currentIndex();
    if(index.isValid()){
        bool res = _partAttachmentModel->removeRow(index.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
    }    
}

void PartDialog::slotCurrentPartAttachmentRowChanged(const QModelIndex &current, const QModelIndex&)
{
    ui->deleteAttachmentButton->setEnabled(current.isValid());
    ui->viewAttachmentButton->setEnabled(current.isValid());
}

void PartDialog::slotPartStorageChanged(int)
{
    QVariant value;
    QComboBox* combo = static_cast<QComboBox*>(sender());
    QModelIndex currIndex = combo->view()->currentIndex();
    if(currIndex.isValid()){        
        int storageId = _storageModel->getItemId(currIndex);
        qDebug()<<"Storage id is "<<storageId;
        value.setValue(storageId);
    }
    QModelIndex storageColIdx = _partsModel->index(_currentModelIndex.row(),PartsSqlTableModel::ColumnStorageId);
    _partsModel->setData(storageColIdx, value);
}

void PartDialog::slotAttachmentDoubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    if(index.column()==0){
        const QModelIndex & urlColIndex = _partAttachmentModel->index(index.row(), AttachmentTableModel3::ColumnURL);
        QString url = urlColIndex.data(Qt::EditRole).toString();
        QDesktopServices::openUrl(QUrl::fromUserInput(url));
    }
}

