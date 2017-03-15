#include "removestockdialog.h"
#include "ui_removestockdialog.h"

RemoveStockDialog::RemoveStockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveStockDialog)
{
    ui->setupUi(this);
    ui->quantitySpinBox->setAccelerated(true);
    ui->commentTextEdit->setTabChangesFocus(true);
}

RemoveStockDialog::~RemoveStockDialog()
{
    delete ui;
}

int RemoveStockDialog::exec(){
    ui->quantitySpinBox->selectAll();
    return QDialog::exec();
}

void RemoveStockDialog::setPartUnit(const QString & partUnit)
{
    QString suffix = QString(" %1").arg(partUnit);
    ui->quantitySpinBox->setSuffix(suffix);
}

void RemoveStockDialog::setAvailableStock(double availableStock)
{
    ui->quantitySpinBox->setMaximum(availableStock);
}

double RemoveStockDialog::getStockChange() const
{
    return ui->quantitySpinBox->value();
}

QString RemoveStockDialog::getComment() const
{
    return ui->commentTextEdit->document()->toPlainText();
}
