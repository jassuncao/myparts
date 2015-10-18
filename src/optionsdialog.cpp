#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "widgets/booleanitemdelegate.h"
#include "models/customtablemodel.h"
#include "widgets/validatingitemdelegate.h"
#include <QDebug>

const int COLUMN_UNIT_DEFAULT = 0;

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);    
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

    setupModels();
    setupConnections();

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
    connect(ui->partUnitsTableView->itemDelegate(), SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)), this,SLOT(slotCloseEditor(QWidget*,QAbstractItemDelegate::EndEditHint)));

    connect(ui->addUnitButton, SIGNAL(clicked()), this, SLOT(slotAddUnit()));
    connect(ui->deleteUnitButton, SIGNAL(clicked()), this, SLOT(slotDeleteUnit()));

    connect(ui->paramUnitsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotParamUnitRowChanged(QModelIndex,QModelIndex)));
    connect(_parameterUnitsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged()));
    connect(ui->addParamUnitButton, SIGNAL(clicked()), this, SLOT(slotAddParamUnit()));
    connect(ui->deleteParamUnitButton, SIGNAL(clicked()), this, SLOT(slotDeleteParamUnit()));

}

void OptionsDialog::slotCloseEditor(QWidget *, QAbstractItemDelegate::EndEditHint hint)
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
    bool res = _partUnitsModel->submitAll();
    res = res && _parameterUnitsModel->submitAll();
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



