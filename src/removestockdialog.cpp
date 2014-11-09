#include "removestockdialog.h"
#include "ui_removestockdialog.h"

RemoveStockDialog::RemoveStockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveStockDialog)
{
    ui->setupUi(this);
}

RemoveStockDialog::~RemoveStockDialog()
{
    delete ui;
}

void RemoveStockDialog::setPartUnit(const QString & partUnit)
{
    QString suffix = QString(" %1").arg(partUnit);
    ui->quantitySpinBox->setSuffix(suffix);
}

void RemoveStockDialog::setAvailableStock(int availableStock)
{
    ui->quantitySpinBox->setMaximum(availableStock);
}

int RemoveStockDialog::getStockChange() const
{
    return ui->quantitySpinBox->value()*-1;
}

QString RemoveStockDialog::getComment() const
{
    return ui->commentTextEdit->document()->toPlainText();
}
