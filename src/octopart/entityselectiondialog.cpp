#include "entityselectiondialog.h"
#include "ui_entityselectiondialog.h"
#include <QDebug>

static const int MODEL_COLUMN = 1;

EntitySelectionDialog::EntitySelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntitySelectionDialog)
{    
    ui->setupUi(this);
    setModal(true);    
    connect(ui->useExistingButton, SIGNAL(toggled(bool)), this, SLOT(slotHandleRadioButtons(bool)));
    ui->createNewButton->setChecked(true);
    ui->comboBox->setEnabled(false);
}

EntitySelectionDialog::~EntitySelectionDialog()
{
    delete ui;    
}

void EntitySelectionDialog::setModel(QAbstractItemModel *model)
{
    ui->comboBox->setModel(model);
    qDebug()<<"Row count" << model->rowCount();
    ui->comboBox->setModelColumn(MODEL_COLUMN);
}

void EntitySelectionDialog::slotHandleRadioButtons(bool checked)
{
    ui->comboBox->setEnabled(checked);
}

EntitySelectionDialog::SelectedOption EntitySelectionDialog::selectedOption() const
{
    if(ui->useExistingButton->isChecked()){
        return EntitySelectionDialog::USE_EXISTING;
    }
    else{
        return EntitySelectionDialog::CREATE_NEW;
    }
}

QModelIndex EntitySelectionDialog::selectedIndex(int modelColumn) const
{
    int row = ui->comboBox->currentIndex();    
    return ui->comboBox->model()->index(row, modelColumn);
}

void EntitySelectionDialog::setTitle(const QString &title)
{
    ui->titleLabel->setText(title);
    setWindowTitle(title);
}

void EntitySelectionDialog::setMessage(const QString &message)
{
    ui->detailLabel->setText(message);
}



