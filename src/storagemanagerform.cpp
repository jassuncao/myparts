#include "storagemanagerform.h"
#include "ui_storagemanagerform.h"

StorageManagerForm::StorageManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StorageManagerForm)
{
    ui->setupUi(this);
}

StorageManagerForm::~StorageManagerForm()
{
    delete ui;
}
