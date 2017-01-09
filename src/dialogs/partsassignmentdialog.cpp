#include "partsassignmentdialog.h"
#include "ui_partsassignmentdialog.h"

PartsAssignmentDialog::PartsAssignmentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartsAssignmentDialog)
{
    ui->setupUi(this);
}

PartsAssignmentDialog::~PartsAssignmentDialog()
{
    delete ui;
}
