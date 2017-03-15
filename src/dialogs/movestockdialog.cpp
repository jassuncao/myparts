#include "movestockdialog.h"
#include "ui_movestockdialog.h"
#include "models/storagetreemodel.h"
#include "models/partconditionmodel.h"
#include "models/modelsrepository.h"

MoveStockDialog::MoveStockDialog(ModelsRepository *modelsRepository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveStockDialog)
{
    ui->setupUi(this);
    ui->quantitySpinBox->setAccelerated(true);
    ui->commentTextEdit->setTabChangesFocus(true);
    ui->storageComboBox->setModel(modelsRepository->partStorageModel());
    ui->storageComboBox->setMinimumContentsLength(22);
    ui->storageComboBox->setMaxVisibleItems(20);

    ui->conditionComboBox->setModel(modelsRepository->partConditionModel());
    ui->conditionComboBox->setModelKeyColumn(PartConditionModel::ColumnId);
    ui->conditionComboBox->setModelColumn(PartConditionModel::ColumnValue);
    int defaultConditionRow = modelsRepository->partConditionModel()->findDefaultValueRow();
    ui->conditionComboBox->setCurrentIndex(defaultConditionRow);
}

MoveStockDialog::~MoveStockDialog()
{
    delete ui;
}

int MoveStockDialog::exec(){
    ui->quantitySpinBox->selectAll();
    return QDialog::exec();
}

void MoveStockDialog::setPartUnit(const QString & partUnit)
{
    QString suffix = QString(" %1").arg(partUnit);
    ui->quantitySpinBox->setSuffix(suffix);
}

void MoveStockDialog::setAvailableStock(double availableStock)
{
    ui->quantitySpinBox->setMaximum(availableStock);
}

double MoveStockDialog::stockToMove() const
{
    return ui->quantitySpinBox->value();
}

QVariant MoveStockDialog::selectedCondition() const
{
    return ui->conditionComboBox->currentKey();
}

void MoveStockDialog::setSelectedCondition(const QVariant & condition)
{
    ui->conditionComboBox->setCurrentKey(condition);
}

QVariant MoveStockDialog::selectedStorage() const
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

void MoveStockDialog::setSelectedStorage(const QVariant & storage)
{
    QModelIndex storageIndex;
    if(storage.isValid()){
        TreeItemModel * viewModel = static_cast<TreeItemModel*>(ui->storageComboBox->model());
        storageIndex = viewModel->findIndex(storage.toInt());
    }
    ui->storageComboBox->setCurrentIndex(storageIndex);
}

QString MoveStockDialog::comment() const
{
    return ui->commentTextEdit->document()->toPlainText();
}
