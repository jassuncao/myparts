#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QSqlTableModel>


OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->pageSelectionWidget->setCurrentRow(0);
    setupModels();
    setupConnections();

}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setupModels()
{
    _partUnits = new QSqlTableModel(this);
    _partUnits->setTable("part_unit");
    _partUnits->setEditStrategy(QSqlTableModel::OnRowChange);
    _partUnits->setHeaderData(1, Qt::Horizontal, tr("Name"));
    _partUnits->setHeaderData(2, Qt::Horizontal, tr("Abbreviation"));
    _partUnits->setHeaderData(3, Qt::Horizontal, tr("Default"));
    ui->unitsTableView->setModel(_partUnits);
    ui->unitsTableView->setColumnHidden(0, true);
    ui->unitsTableView->horizontalHeader()->moveSection(3,0);

    _partUnits->select();
}

void OptionsDialog::setupConnections()
{
    connect(ui->pageSelectionWidget, SIGNAL(currentRowChanged(int)), this, SLOT(slotCurrentPageChanged(int)));
}

void OptionsDialog::slotCurrentPageChanged(int currentRow)
{
    QString text = ui->pageSelectionWidget->item(currentRow)->text();
    ui->pageTitleLabel->setText(text);
}
