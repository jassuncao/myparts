#include "tableexportdialog.h"
#include "ui_tableexportdialog.h"

TableExportDialog::TableExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableExportDialog)
{
    ui->setupUi(this);
}

TableExportDialog::~TableExportDialog()
{
    delete ui;
}
