#include "manufacturersdialog.h"
#include "ui_manufacturersdialog.h"

ManufacturersDialog::ManufacturersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManufacturersDialog)
{
    ui->setupUi(this);
}

ManufacturersDialog::~ManufacturersDialog()
{
    delete ui;
}
