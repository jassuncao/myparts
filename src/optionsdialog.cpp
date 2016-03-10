#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "widgets/booleanitemdelegate.h"
#include "models/customtablemodel.h"
#include "models/partconditionmodel.h"
#include "models/modelsprovider.h"
#include "widgets/validatingitemdelegate.h"
#include "constants.h"
#include <QDebug>
#include <QSettings>

const int COLUMN_UNIT_DEFAULT = 0;
//const int COLUMN_CONDITION_DEFAULT = 0;

OptionsDialog::OptionsDialog(ModelsProvider * provider, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog), _modelsProvider(provider)
{
    ui->setupUi(this);    
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

    setupModels();
    setupConnections();
    setupGeneralSettings();
    ui->pageSelectionWidget->setCurrentRow(0);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setupModels()
{
    setupPartUnitsModel();
    setupParamsUnitsModel();
    setupPartConditionModel();
}

void OptionsDialog::setupPartUnitsModel()
{
    BooleanItemDelegate * defaultValueDelegate = new BooleanItemDelegate(QPixmap(":icons/default"),QPixmap(), this);

    QStringList fieldNames;
    fieldNames<<QLatin1String("defaultUnit")<<QLatin1String("name")<<QLatin1String("abbreviation");
    QStringList columnNames;
    columnNames<<QString()<<tr("Name")<<tr("Abbreviation");
    _partUnitsModel = new SimpleSqlTableModel("part_unit", fieldNames, columnNames, QString(), this);
    ui->partUnitsTableView->setModel(_partUnitsModel);
    ui->partUnitsTableView->setItemDelegateForColumn(COLUMN_UNIT_DEFAULT, defaultValueDelegate);
    ui->partUnitsTableView->setColumnWidth(COLUMN_UNIT_DEFAULT, defaultValueDelegate->widthHint());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    ui->partUnitsTableView->horizontalHeader()->setSectionResizeMode(COLUMN_UNIT_DEFAULT, QHeaderView::Fixed);
#else
    ui->partUnitsTableView->horizontalHeader()->setResizeMode(COLUMN_UNIT_DEFAULT, QHeaderView::Fixed);
#endif
    _partUnitsModel->select();
}

void OptionsDialog::setupParamsUnitsModel()
{
    QStringList fieldNames;
    fieldNames<<QLatin1String("name")<<QLatin1String("symbol");
    QStringList columnNames;
    columnNames<<tr("Name")<<tr("Symbol");
    _parameterUnitsModel = new SimpleSqlTableModel("unit", fieldNames, columnNames, QString(), this);
    ui->paramUnitsTableView->setModel(_parameterUnitsModel);
    _parameterUnitsModel->select();
}

void OptionsDialog::setupPartConditionModel()
{
    BooleanItemDelegate * defaultValueDelegate = new BooleanItemDelegate(QPixmap(":icons/default"),QPixmap(), this);

    //_partConditionModel = new SimpleSqlTableModel("condition", fieldNames, columnNames, QString(), this);
    _partConditionModel = _modelsProvider->partConditionModel();
    ui->partConditionTableView->setModel(_partConditionModel);
    ui->partConditionTableView->setItemDelegateForColumn(PartConditionModel::ColumnDefault, defaultValueDelegate);
    ui->partConditionTableView->setColumnWidth(PartConditionModel::ColumnDefault, defaultValueDelegate->widthHint());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    ui->partConditionTableView->horizontalHeader()->setSectionResizeMode(COLUMN_CONDITION_DEFAULT, QHeaderView::Fixed);
#else
    ui->partConditionTableView->horizontalHeader()->setResizeMode(PartConditionModel::ColumnDefault, QHeaderView::Fixed);
#endif
    _partConditionModel->select();
}

void OptionsDialog::setupConnections()
{
    connect(ui->pageSelectionWidget, SIGNAL(currentRowChanged(int)), this, SLOT(slotCurrentPageChanged(int)));

    QPushButton * applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(slotApplyChanges()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(ui->makeUnitDefaultButton, SIGNAL(clicked()), this, SLOT(slotMakeCurrentUnitDefault()));
    connect(ui->partUnitsTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this, SLOT(slotUnitSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->partUnitsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotUnitDoubleClick(QModelIndex)));
    connect(_partUnitsModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged()));    
    connect(ui->partUnitsTableView->itemDelegate(), SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)), this,SLOT(slotClosePartUnitEditor(QWidget*,QAbstractItemDelegate::EndEditHint)));

    connect(ui->addUnitButton, SIGNAL(clicked()), this, SLOT(slotAddUnit()));
    connect(ui->deleteUnitButton, SIGNAL(clicked()), this, SLOT(slotDeleteUnit()));

    connect(ui->paramUnitsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotParamUnitRowChanged(QModelIndex,QModelIndex)));
    connect(_parameterUnitsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged()));
    connect(ui->addParamUnitButton, SIGNAL(clicked()), this, SLOT(slotAddParamUnit()));
    connect(ui->deleteParamUnitButton, SIGNAL(clicked()), this, SLOT(slotDeleteParamUnit()));

    connect(ui->partConditionTableView->selectionModel(), SIGNAL(currentColumnChanged(QModelIndex,QModelIndex)), this, SLOT(slotPartConditionRowChanged(QModelIndex,QModelIndex)));
    connect(_partConditionModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged()));
    connect(ui->addConditionButton, SIGNAL(clicked()), this, SLOT(slotAddPartCondition()));
    connect(ui->deleteConditionButton, SIGNAL(clicked()), this, SLOT(slotDeletePartCondition()));
    connect(ui->makeConditionDefaultButton, SIGNAL(clicked()), this, SLOT(slotMakeCurrentConditionDefault()));
    connect(ui->partConditionTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotConditionDoubleClick(QModelIndex)));
    connect(ui->partConditionTableView->itemDelegate(), SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)), this,SLOT(slotClosePartConditionEditor(QWidget*,QAbstractItemDelegate::EndEditHint)));
}

void OptionsDialog::setupGeneralSettings()
{
    QSettings settings;
    QString currency = settings.value(CURRENCY_SYMBOL_KEY).toString();
    bool currencyAfter = settings.value(CURRENCY_POSITION_KEY).toBool();
    ui->currencySymbolLineEdit->setText(currency);
    if(currencyAfter){
        ui->currencySymbAfterRadio->setChecked(true);
    }
    else{
     ui->currencySymbBeforeRadio->setChecked(true);
    }
}

void OptionsDialog::slotClosePartUnitEditor(QWidget *, QAbstractItemDelegate::EndEditHint hint)
{
    if(hint==QAbstractItemDelegate::EditNextItem){
        QModelIndex index = ui->partUnitsTableView->currentIndex();
        if(index.isValid() && index.column()==0){
            QModelIndex idx = _partUnitsModel->index(index.row(), 1);
            ui->partUnitsTableView->setCurrentIndex(idx);
            ui->partUnitsTableView->edit(idx);
        }
    }
}

void OptionsDialog::slotClosePartConditionEditor(QWidget *, QAbstractItemDelegate::EndEditHint hint)
{
    if(hint==QAbstractItemDelegate::EditNextItem){
        QModelIndex index = ui->partConditionTableView->currentIndex();
        if(index.isValid() && index.column()==0){
            QModelIndex idx = _partConditionModel->index(index.row(), 1);
            ui->partConditionTableView->setCurrentIndex(idx);
            ui->partConditionTableView->edit(idx);
        }
    }
}


void OptionsDialog::accept()
{
    slotApplyChanges();
    QDialog::accept();
}

void OptionsDialog::reject()
{
    QDialog::reject();
}

void OptionsDialog::slotCurrentPageChanged(int currentRow)
{
    QString text = ui->pageSelectionWidget->item(currentRow)->text();
    ui->pageTitleLabel->setText(text);
}

static void makeSelectedItemDefault(QAbstractItemModel * model, const int selectedRow, const int column){
    int rowCount = model->rowCount();
    for(int row=0; row<rowCount; ++row){
        QModelIndex index = model->index(row, column);
        QVariant var = model->data(index, Qt::EditRole);
        if(var.toBool() && row!=selectedRow){
            model->setData(index, QVariant(false));
        }
        if(row==selectedRow){
            model->setData(index, QVariant(true));
        }
    }
}

void OptionsDialog::slotMakeCurrentUnitDefault()
{
    QModelIndex currentIndex = ui->partUnitsTableView->currentIndex();
    if(!currentIndex.isValid())
        return;

    makeSelectedItemDefault(_partUnitsModel, currentIndex.row(), COLUMN_UNIT_DEFAULT);
}

void OptionsDialog::slotUnitSelectionChanged(const QItemSelection &selected, const QItemSelection)
{
    ui->deleteUnitButton->setEnabled(selected.size()>0);
    ui->makeUnitDefaultButton->setEnabled(selected.size()>0);
}

void OptionsDialog::slotUnitDoubleClick(const QModelIndex &index)
{
    if(index.isValid() && index.column() == COLUMN_UNIT_DEFAULT){
        makeSelectedItemDefault(_partUnitsModel, index.row(), COLUMN_UNIT_DEFAULT);
    }
}

void OptionsDialog::slotParamUnitRowChanged(const QModelIndex &current, const QModelIndex &)
{
    ui->deleteParamUnitButton->setEnabled(current.isValid());
}

void OptionsDialog::slotDataChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

void OptionsDialog::slotApplyChanges()
{
    QSettings settings;
    settings.setValue(CURRENCY_SYMBOL_KEY, ui->currencySymbolLineEdit->text());
    bool currencyAfter = ui->currencySymbAfterRadio->isChecked();
    settings.setValue(CURRENCY_POSITION_KEY, currencyAfter);

    bool res = _partUnitsModel->submitAll();
    res = res && _parameterUnitsModel->submitAll();
    res = res && _partConditionModel->submitAll();
    if(res){
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    }
}


void OptionsDialog::slotAddUnit()
{
    int newRow = _partUnitsModel->rowCount();
    _partUnitsModel->insertRow(newRow);
    QModelIndex idx = _partUnitsModel->index(newRow,1);
    ui->partUnitsTableView->setCurrentIndex(idx);
    ui->partUnitsTableView->edit(idx);
}

void OptionsDialog::slotDeleteUnit()
{
    QModelIndex currentIdx = ui->partUnitsTableView->currentIndex();
    if(!currentIdx.isValid())
        return;
    if(_partUnitsModel->removeRow(currentIdx.row())){
        slotDataChanged();
    }    
}

void OptionsDialog::slotAddParamUnit()
{
    int newRow = _parameterUnitsModel->rowCount();
    _parameterUnitsModel->insertRow(newRow);
    QModelIndex idx = _parameterUnitsModel->index(newRow, 0);
    ui->paramUnitsTableView->setCurrentIndex(idx);
    ui->paramUnitsTableView->edit(idx);
}

void OptionsDialog::slotDeleteParamUnit()
{
    QModelIndex currentIdx = ui->paramUnitsTableView->currentIndex();
    if(!currentIdx.isValid())
        return;
    if(_parameterUnitsModel->removeRow(currentIdx.row())){
        slotDataChanged();
    }
}

void OptionsDialog::slotPartConditionRowChanged(const QModelIndex &current, const QModelIndex &)
{
    ui->deleteConditionButton->setEnabled(current.isValid());
    ui->makeConditionDefaultButton->setEnabled(current.isValid());
}

void OptionsDialog::slotAddPartCondition()
{
    int newRow = _partConditionModel->rowCount();
    _partConditionModel->insertRow(newRow);
    QModelIndex idx = _partConditionModel->index(newRow, 0);
    ui->partConditionTableView->setCurrentIndex(idx);
    ui->partConditionTableView->edit(idx);
}


void OptionsDialog::slotDeletePartCondition()
{
    QModelIndex currentIdx = ui->partConditionTableView->currentIndex();
    if(!currentIdx.isValid())
        return;
    if(_partConditionModel->removeRow(currentIdx.row())){
        slotDataChanged();
    }
}

void OptionsDialog::slotMakeCurrentConditionDefault()
{
    QModelIndex currentIndex = ui->partConditionTableView->currentIndex();
    if(!currentIndex.isValid())
        return;
    _partConditionModel->makeSelectedItemDefault(currentIndex.row());
    //makeSelectedItemDefault(_partConditionModel, currentIndex.row(), COLUMN_CONDITION_DEFAULT);
}

void OptionsDialog::slotConditionDoubleClick(const QModelIndex &index)
{
    if(index.isValid() && index.column() == PartConditionModel::ColumnDefault){
        _partConditionModel->makeSelectedItemDefault(index.row());
        //makeSelectedItemDefault(_partConditionModel, index.row(), COLUMN_CONDITION_DEFAULT);
    }
}



