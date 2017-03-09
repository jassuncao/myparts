#include <QModelIndex>
#include "partdialog.h"
#include "ui_partdialog.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
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
#include <QDialogButtonBox>
#include <QDesktopServices>

#include "widgets/parametervaluedelegate.h"
#include "widgets/currencydelegate.h"
#include "widgets/priceitemdelegate.h"
#include "widgets/validatingitemdelegate.h"
#include "widgets/comboitemdelegate.h"
#include "widgets/datetimedelegate.h"
#include "dialogs/attachmentselectiondialog.h"
#include "models/partssqltablemodel.h"
#include "models/storagetreemodel.h"
#include "models/customtablemodel.h"
#include "models/partstocktablemodel.h"
#include "models/modelsrepository.h"
#include "models/categorytreemodel.h"
#include "models/partparametertablemodel.h"
#include "models/partdistributorproxymodel.h"

#include "utils.h"
#include "constants.h"
#include "currencyhelper.h"


inline static QVariant getColumnValue(QAbstractItemModel * model, int row, int column){
    return model->index(row, column).data(Qt::EditRole);
}

PartDialog::PartDialog(ModelsRepository * modelsProvider, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartDialog),
    _modelsProvider(modelsProvider),
    _partsModel(modelsProvider->partsModel()),
    _lastSelectedPackage(-1),
    _addMode(false)  
{

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    ui->partCategoryCombo->setMinimumContentsLength(22);
    ui->partCategoryCombo->setMaxVisibleItems(40);

    ui->quickStorageButton->setVisible(false);//Not supported yet
    _partParamsModel = new PartParameterTableModel(this);
    _partAttachmentModel = AttachmentTableModel3::createNewPartAttachmentModel(this);
    _partDistributorModel = PartDistributorTableModel2::createNew(this);
    _partManufacturerModel = PartManufacturerTableModel2::createNew(this);
    _partStockModel = PartStockTableModel::createNew(this);    

    _nextActionCheckbox = new QCheckBox(this);
    ui->buttonBox->addButton(_nextActionCheckbox, QDialogButtonBox::ResetRole);
    initCombos();

    _mapper = new QDataWidgetMapper(this);
    _mapper->setModel(_modelsProvider->partsModel());
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    _mapper->addMapping(ui->partNameEdit, PartsSqlTableModel::ColumnName);
    _mapper->addMapping(ui->partDescriptionEdit, PartsSqlTableModel::ColumnDescription);
    _mapper->addMapping(ui->minStockSpinBox, PartsSqlTableModel::ColumnMinStock);
    _mapper->addMapping(ui->partConditionCombo, PartsSqlTableModel::ColumnConditionId, "currentKey");
    _mapper->addMapping(ui->partUnitCombo, PartsSqlTableModel::ColumnPartUnitId, "currentKey");
    _mapper->addMapping(ui->partPackageCombo, PartsSqlTableModel::ColumnPackageId, "currentKey");
    _mapper->addMapping(ui->initialStockSpinBox,PartsSqlTableModel::ColumnActualStock, "value");
    _mapper->addMapping(ui->partCommentText, PartsSqlTableModel::ColumnComment);
    _mapper->addMapping(ui->partCustomPartNumberEdit, PartsSqlTableModel::ColumnCustomPartNumber);

    ui->partParametersTableView->setModel(_partParamsModel);
    ui->partParametersTableView->setItemDelegate(new ComboItemDelegate(this));
    ui->partParametersTableView->setItemDelegateForColumn(PartParameterTableModel::ColumnParameterValue, new ParameterValueDelegate(this));
    ui->partParametersTableView->setColumnWidth(PartParameterTableModel::ColumnParameter, 180);

    ui->partDistributorsTableView->setModel(_partDistributorModel);
    ui->partDistributorsTableView->setItemDelegateForColumn(PartDistributorTableModel2::ColumnUnitPrice, new PriceItemDelegate(false, this));
    ui->partDistributorsTableView->setItemDelegateForColumn(PartDistributorTableModel2::ColumnCurrency, new CurrencyDelegate(this));
    //ui->partDistributorsTableView->setItemDelegateForColumn(PartDistributorTableModel2::ColumnMinimumOrder, new ValidatingItemDelegate(new QIntValidator(), this));
    ui->partDistributorsTableView->setItemDelegateForColumn(PartDistributorTableModel2::ColumnDateTime, new DateDelegate(this));
    ui->partDistributorsTableView->setItemDelegate(new ComboItemDelegate(this));
    ui->partDistributorsTableView->hideColumn(PartDistributorTableModel2::ColumnDateTime);
    ui->partDistributorsTableView->setColumnWidth(PartDistributorTableModel2::ColumnDistributor, 180);
    ui->partDistributorsTableView->setColumnWidth(PartDistributorTableModel2::ColumnPartNumber, 180);
    ui->partDistributorsTableView->horizontalHeader()->swapSections(PartDistributorTableModel2::ColumnUnitPrice, PartDistributorTableModel2::ColumnCurrency);

    ui->partManufacturerTableView->setModel(_partManufacturerModel);
    ui->partManufacturerTableView->setItemDelegate(new ComboItemDelegate(this));
    ui->partManufacturerTableView->setColumnWidth(PartManufacturerTableModel2::ColumnManufacturer, 220);
    //ui->partManufacturerTableView->setColumnWidth(PartManufacturerTableModel2::ColumnPartNumber, 256);

    ui->partAttachmentsTableView->setModel(_partAttachmentModel);
    ui->partAttachmentsTableView->setColumnWidth(0, 512);

    QSettings settings;
    QString currency = settings.value(CURRENCY_CODE_KEY).toString();
    int format = settings.value(CURRENCY_FORMAT_KEY).toInt();
    if(format == QLocale::CurrencySymbol){
        currency = CurrencyHelper::currencySymbol(currency);
    }

    CurrencyHelper::CurrencySymbolPosition position = CurrencyHelper::symbolPosition();
    if(position == CurrencyHelper::Suffix)
        ui->priceSpinBox->setSuffix(currency);
    else
        ui->priceSpinBox->setPrefix(currency);

    ui->deleteParameterButton->setEnabled(false);
    ui->deleteDistributorButton->setEnabled(false);
    ui->deleteManufacturerButton->setEnabled(false);
    ui->deleteAttachmentButton->setEnabled(false);
    ui->viewAttachmentButton->setEnabled(false);

    connect(ui->partPackageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPackageChanged(int)));
    connect(ui->nonePackageRadioButton, SIGNAL(clicked()), this, SLOT(slotDeselectPackage()));
    connect(ui->usePackageRadioButton, SIGNAL(clicked()), this, SLOT(slotUsePackage()));
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

void PartDialog::setCurrentCategory(const QVariant & category)
{
    _currentCategory = category;
}

void PartDialog::setCurrentStorage(const QVariant & storage)
{
    _currentStorage = storage;
}

int PartDialog::exec()
{
    ui->partNameEdit->setFocus();
    return QDialog::exec();
}

int PartDialog::addNewPart()
{
    setWindowTitle(tr("Add Part"));
    _addMode = true;    
    _nextActionCheckbox->setText(tr("Create blank item after save"));

    QSqlRecord initialData = _partsModel->record();
    initialData.setValue(PartsSqlTableModel::ColumnConditionId, _defaultCondition);
    initialData.setValue(PartsSqlTableModel::ColumnPartUnitId, _defaultUnit);
    initialData.setValue(PartsSqlTableModel::ColumnCategoryId, _currentCategory);
    initialData.setValue(PartsSqlTableModel::ColumnStorageId, _currentStorage);
    const QModelIndex insertedIndex = insertNewPart(initialData);
    setCurrentModelIndex(insertedIndex);
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

int PartDialog::duplicatePart(const QModelIndex &index, bool allData)
{
    setWindowTitle(tr("Add Part"));
    _nextActionCheckbox->setText(tr("Create blank item after save"));
    _addMode = true;
    QSqlRecord initialData;
    if(allData){
        initialData = copyAllData(index);
        QVariant partId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnId);

        QVariant invalidId;
        _partParamsModel->setCurrentPartId(partId);
        _partParamsModel->select();
        _partParamsModel->cloneData();
        _partParamsModel->setCurrentPartId(invalidId);

        _partDistributorModel->setCurrentPartId(partId);
        _partDistributorModel->select();
        _partDistributorModel->cloneData();
        _partDistributorModel->setCurrentPartId(invalidId);

        _partManufacturerModel->setCurrentPartId(partId);
        _partManufacturerModel->select();
        _partManufacturerModel->cloneData();
        _partManufacturerModel->setCurrentPartId(invalidId);

        _partAttachmentModel->setCurrentForeignKey(partId);
        _partAttachmentModel->select();
        _partAttachmentModel->cloneData();
        _partAttachmentModel->setCurrentPartId(invalidId);
    }
    else{
        initialData = copySomeData(index);
    }
    const QModelIndex insertedIndex = insertNewPart(initialData);
    setCurrentModelIndex(insertedIndex);
    return exec();
}

QModelIndex PartDialog::insertNewPart(QSqlRecord initialData)
{
    //Set the createDate field
    QDateTime now = QDateTime::currentDateTimeUtc();
    QVariant t = QVariant(now.toTime_t());
    initialData.setValue(PartsSqlTableModel::ColumnCreateDate, t);

    int newRow = _partsModel->rowCount();
    bool success = _partsModel->insertRecord(newRow, initialData);
    if(success) {
        return  _partsModel->index(newRow, PartsSqlTableModel::ColumnId);
    }
    else {
        qWarning("Failed to insert new part");
        return QModelIndex();
    }
}

QSqlRecord PartDialog::copySomeData(const QModelIndex & index)
{
    QVariant name = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnName);
    QVariant description = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnDescription);
    QVariant storageId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnStorageId);
    QVariant categoryId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnCategoryId);
    QVariant partUnitId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnPartUnitId);
    QVariant packageId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnPackageId);
    QVariant conditionId = getColumnValue(_partsModel, index.row(), PartsSqlTableModel::ColumnConditionId);

    QSqlRecord initialData =_partsModel->record();
    initialData.setValue(PartsSqlTableModel::ColumnStorageId, name);
    initialData.setValue(PartsSqlTableModel::ColumnStorageId, description);
    initialData.setValue(PartsSqlTableModel::ColumnStorageId, storageId);
    initialData.setValue(PartsSqlTableModel::ColumnCategoryId, categoryId);
    initialData.setValue(PartsSqlTableModel::ColumnPartUnitId, partUnitId);
    initialData.setValue(PartsSqlTableModel::ColumnPackageId, packageId);
    initialData.setValue(PartsSqlTableModel::ColumnConditionId, conditionId);
    return initialData;
}

QSqlRecord PartDialog::copyAllData(const QModelIndex & index)
{
    QSqlRecord initialData = _partsModel->record(index.row());
    initialData.setNull(PartsSqlTableModel::ColumnId);
    initialData.setNull(PartsSqlTableModel::ColumnActualStock);
    return initialData;
}

void PartDialog::initCombos()
{
    ui->partCategoryCombo->setModel(_modelsProvider->partCategoryModel());
    ui->partStorageCombo->setModel(_modelsProvider->partStorageModel());
    connect(ui->partCategoryCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPartCategoryChanged(int)));
    connect(ui->partStorageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPartStorageChanged(int)));

    _partConditionModel = new QSqlQueryModel(this);
    _partConditionModel->setQuery("SELECT id, value, defaultCondition FROM condition ORDER BY value ASC");

    int row = Utils::findDefaultValueRow(_partConditionModel, 2);
    _defaultCondition = _partConditionModel->index(row,0).data(Qt::EditRole);

    ui->partConditionCombo->setModel(_partConditionModel);
    ui->partConditionCombo->setModelKeyColumn(0);
    ui->partConditionCombo->setModelColumn(1);        


    _partUnitsModel = new QSqlQueryModel(this);
    _partUnitsModel->setQuery("SELECT id, name, defaultUnit FROM part_unit ORDER BY name ASC");

    row = Utils::findDefaultValueRow(_partUnitsModel, 2);
    _defaultUnit = _partUnitsModel->index(row,0).data(Qt::EditRole);

    ui->partUnitCombo->setModel(_partUnitsModel);
    ui->partUnitCombo->setModelKeyColumn(0);
    ui->partUnitCombo->setModelColumn(1);


    _packagesModel = new QSqlQueryModel(this);
    _packagesModel->setQuery("SELECT id, name FROM package ORDER BY name ASC");
    ui->partPackageCombo->setModel(_packagesModel);
    ui->partPackageCombo->setModelKeyColumn(0);
    ui->partPackageCombo->setModelColumn(1);
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

static void setIndexForFkey(QComboBox * combo, QVariant fkey){
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

static void setTreeViewComboBoxIndex(TreeViewComboBox * combo, QVariant key)
{
    if(!key.isNull()){
        TreeItemModel * viewModel = static_cast<TreeItemModel*>(combo->model());
        QModelIndex treeIndex = viewModel->findIndex(key.toInt());
        if(treeIndex.isValid()){
            bool blocked = combo->blockSignals(true);
            combo->setCurrentIndex(treeIndex);
            combo->blockSignals(blocked);
        }
    }
}

void PartDialog::setCurrentModelIndex(const QModelIndex &index)
{
    _currentModelIndex = index;
    _mapper->setCurrentModelIndex(index);

    int row = index.isValid() ? index.row() : -1;
    _lastSelectedPackage = ui->partPackageCombo->currentIndex();
    if(_lastSelectedPackage<0){
        ui->nonePackageRadioButton->setChecked(true);
    }
    else{
        ui->usePackageRadioButton->setChecked(true);
    }

    QVariant fkey = getColumnValue(_partsModel, row, PartsSqlTableModel::ColumnStorageId);
    setTreeViewComboBoxIndex(ui->partStorageCombo, fkey);

    fkey = getColumnValue(_partsModel, row, PartsSqlTableModel::ColumnCategoryId);
    setTreeViewComboBoxIndex(ui->partCategoryCombo, fkey);
}

void PartDialog::accept()
{    
    if(_nextActionCheckbox->isChecked()){
        /*If the user chooses to keep adding parts we need to copy some attributes before
         commiting changes, because after the commit, the current model index becomes invalid and can't be
         used to access data
        */
        QSqlRecord initialData;
        if(_addMode){
            //In add mode we create a new item with some minimal data
            initialData = copySomeData(_currentModelIndex);
            //We discard the data from the auxiliary tables by using an invalid
            QVariant invalidId;
            _partParamsModel->setCurrentPartId(invalidId);
            _partParamsModel->select();

            _partDistributorModel->setCurrentPartId(invalidId);
            _partDistributorModel->select();

            _partManufacturerModel->setCurrentPartId(invalidId);
            _partManufacturerModel->select();

            _partAttachmentModel->setCurrentPartId(invalidId);
            _partAttachmentModel->select();

        }
        else{
            //In edit mode we make a duplicate with all the data
            initialData = copyAllData(_currentModelIndex);
            //We "clone" the data from the auxiliary tables
            QVariant invalidId;
            _partParamsModel->cloneData();
            _partParamsModel->setCurrentPartId(invalidId);

            _partDistributorModel->cloneData();
            _partDistributorModel->setCurrentPartId(invalidId);

            _partManufacturerModel->cloneData();
            _partManufacturerModel->setCurrentPartId(invalidId);

            _partAttachmentModel->cloneData();
            _partAttachmentModel->setCurrentPartId(invalidId);
        }

        commitChanges();
        //Insert a new record with initial data
        QModelIndex insertIndex = insertNewPart(initialData);
        setCurrentModelIndex(insertIndex);

        ui->priceSpinBox->setValue(0.0);
        ui->pricePerItemCheckBox->setChecked(false);
        ui->tabWidget->setCurrentIndex(0);
        ui->partNameEdit->setFocus();
    }
    else{
        commitChanges();
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
                _partStockModel->setCurrentPartId(partId);
                _partStockModel->appendRow(initialStock(), partPrice(), QString());
                _partStockModel->submitAll();
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

void PartDialog::slotPackageChanged(int index)
{
    if(index<0){
        ui->nonePackageRadioButton->setChecked(true);
    }
    else{
        ui->usePackageRadioButton->setChecked(true);
    }
}

void PartDialog::slotDeselectPackage(){
    _lastSelectedPackage = ui->partPackageCombo->currentIndex();
    ui->partPackageCombo->setCurrentIndex(-1);
}

void PartDialog::slotUsePackage(){
    ui->partPackageCombo->setCurrentIndex(_lastSelectedPackage);
}

void PartDialog::slotAddParameter(){    
    int rowCount = _partParamsModel->rowCount();
    if(_partParamsModel->insertRow(rowCount)){
        QModelIndex index = _partParamsModel->index(rowCount,PartParameterTableModel::ColumnParameter);
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

void PartDialog::slotPartStorageChanged(int idx)
{
    qDebug()<<"IDX:"<<idx;
    QVariant value;
    QComboBox* combo = static_cast<QComboBox*>(sender());
    QModelIndex currIndex = combo->view()->currentIndex();
    if(currIndex.isValid()){
        TreeItemModel* model = static_cast<TreeItemModel*>(combo->model());
        int storageId = model->getItemId(currIndex);
        qDebug()<<"Storage id is "<<storageId;
        value.setValue(storageId);
    }
    qDebug()<<"Changing part storage to "<<value;
    QModelIndex storageColIdx = _partsModel->index(_currentModelIndex.row(),PartsSqlTableModel::ColumnStorageId);
    _partsModel->setData(storageColIdx, value);
}

void PartDialog::slotPartCategoryChanged(int idx)
{
    qDebug()<<"IDX:"<<idx;
    QVariant value;
    QComboBox* combo = static_cast<QComboBox*>(sender());
    QModelIndex currIndex = combo->view()->currentIndex();
    if(currIndex.isValid()){
        TreeItemModel* model = static_cast<TreeItemModel*>(combo->model());
        int categoryId = model->getItemId(currIndex);
        qDebug()<<"Category id is "<<categoryId;
        value.setValue(categoryId);
    }
    qDebug()<<"Changing part category to "<<value;
    QModelIndex categoryColIdx = _partsModel->index(_currentModelIndex.row(),PartsSqlTableModel::ColumnCategoryId);
    _partsModel->setData(categoryColIdx, value);
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

