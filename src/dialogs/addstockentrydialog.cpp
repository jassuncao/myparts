#include "addstockentrydialog.h"
#include "ui_addstockentrydialog.h"
#include <QSettings>
#include <QLocale>
#include "currencyhelper.h"
#include "constants.h"
#include "models/modelsrepository.h"
#include "models/storagetreemodel.h"
#include "models/partconditionmodel.h"
#include "utils.h"
#include "widgets/relationalcombobox.h"


AddStockEntryDialog::AddStockEntryDialog(ModelsRepository *modelsRepository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddStockEntryDialog),
    _modelsRepository(modelsRepository)
{
    ui->setupUi(this);
    ui->quantitySpinBox->setAccelerated(true);
    ui->commentTextEdit->setTabChangesFocus(true);

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

    ui->storageComboBox->setModel(_modelsRepository->partStorageModel());
    ui->storageComboBox->setMinimumContentsLength(22);
    ui->storageComboBox->setMaxVisibleItems(20);

    ui->conditionComboBox->setModel(_modelsRepository->partConditionModel());
    ui->conditionComboBox->setModelKeyColumn(PartConditionModel::ColumnId);
    ui->conditionComboBox->setModelColumn(PartConditionModel::ColumnValue);
    int defaultConditionRow = _modelsRepository->partConditionModel()->findDefaultValueRow();
    ui->conditionComboBox->setCurrentIndex(defaultConditionRow);
}

AddStockEntryDialog::~AddStockEntryDialog()
{
    delete ui;
}

int AddStockEntryDialog::exec(){
    ui->quantitySpinBox->selectAll();
    return QDialog::exec();
}

QVariant AddStockEntryDialog::selectedStorage() const
{
    TreeItemModel* model = static_cast<TreeItemModel*>(ui->storageComboBox->model());
    QModelIndex currIndex = ui->storageComboBox->view()->currentIndex();
    int itemId;
    if(currIndex.isValid()){
        itemId = model->getItemId(currIndex);
    }
    else{
        itemId = model->rootItemId();
    }
    return QVariant(itemId);
}

void AddStockEntryDialog::setSelectedStorage(const QVariant & storage)
{
    QModelIndex storageIndex;
    if(storage.isValid()){
        TreeItemModel * viewModel = static_cast<TreeItemModel*>(ui->storageComboBox->model());
        storageIndex = viewModel->findIndex(storage.toInt());
    }
    ui->storageComboBox->setCurrentIndex(storageIndex);
}

QVariant AddStockEntryDialog::selectedCondition() const
{
    return ui->conditionComboBox->currentKey();
}

void AddStockEntryDialog::setSelectedCondition(const QVariant & condition)
{
    ui->conditionComboBox->setCurrentKey(condition);
}

void AddStockEntryDialog::setPartUnit(const QString & partUnit)
{
    QString suffix = QString(" %1").arg(partUnit);
    ui->quantitySpinBox->setSuffix(suffix);
}

int AddStockEntryDialog::stockChange() const
{
    return ui->quantitySpinBox->value();
}

double AddStockEntryDialog::partPrice() const
{
    double price = ui->priceSpinBox->value();
    if(!ui->priceKindCheckBox->isChecked()){
        double change = stockChange();
        if(change!=0)
            price = price / change;
       else
            price = 0;
    }
    return price;
}

QString AddStockEntryDialog::comment() const
{
    return ui->commentTextEdit->document()->toPlainText();
}
