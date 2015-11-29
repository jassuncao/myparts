#include "addstockdialog.h"
#include "ui_addstockdialog.h"
#include <QSettings>

AddStockDialog::AddStockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddStockDialog)
{
    ui->setupUi(this);
    QSettings settings;
    QString defaultCurrency(QChar(8364));
    QString currency = settings.value("currency/symbol", defaultCurrency).toString();
    bool currencyAfter = settings.value("currency/after",true).toBool();    
    if(currencyAfter)
        ui->priceSpinBox->setSuffix(currency);
    else
        ui->priceSpinBox->setPrefix(currency);
}

AddStockDialog::~AddStockDialog()
{
    delete ui;
}

void AddStockDialog::setPartUnit(const QString & partUnit)
{
    QString suffix = QString(" %1").arg(partUnit);
    ui->quantitySpinBox->setSuffix(suffix);
}

int AddStockDialog::getStockChange() const
{
    return ui->quantitySpinBox->value();
}

double AddStockDialog::getPartPrice() const
{
    double price = ui->priceSpinBox->value();
    if(!ui->priceKindCheckBox->isChecked()){
        double change = getStockChange();
        if(change!=0)
            price = price / change;
       else
            price = 0;
    }
    return price;
}

QString AddStockDialog::getComment() const
{
    return ui->commentTextEdit->document()->toPlainText();
}
