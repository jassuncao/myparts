#include "octopartimportdialog.h"
#include "ui_octopartimportdialog.h"

OctopartImportDialog::OctopartImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OctopartImportDialog)
{
    ui->setupUi(this);
}

OctopartImportDialog::~OctopartImportDialog()
{
    delete ui;
}
