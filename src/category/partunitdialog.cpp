#include "partunitdialog.h"
#include <QDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDataWidgetMapper>
#include <QPushButton>
#include <QMessageBox>


PartUnitDialog::PartUnitDialog(QAbstractItemModel *model, QWidget *parent) :
    QDialog(parent)
{
    _nameEdit = new QLineEdit(this);
    _abbreviationEdit = new QLineEdit(this);
    _defaultCheckBox = new QCheckBox(tr("Use as default unit"),this);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout * mainLayout = new QVBoxLayout;
    _formLayout = new QFormLayout;
    _formLayout->addRow(tr("Name: "),_nameEdit);
    _formLayout->addRow(tr("Abbreviation: "),_abbreviationEdit);
    _formLayout->addRow(QString(),_defaultCheckBox);
    _formLayout->setSpacing(4);

    mainLayout->addItem(_formLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    _widgetMapper = new QDataWidgetMapper(this);
    _widgetMapper->setModel(model);
    _widgetMapper->addMapping(_nameEdit, 0);
    _widgetMapper->addMapping(_abbreviationEdit, 1);
    _widgetMapper->addMapping(_defaultCheckBox,2);
    _widgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    resize(300,100);
}

void PartUnitDialog::setRootIndex(const QModelIndex &index)
{
    _widgetMapper->setRootIndex(index);
}

void PartUnitDialog::setCurrentIndex(const int index)
{
    _widgetMapper->setCurrentIndex(index);
}

void PartUnitDialog::setCurrentModelIndex(const QModelIndex &index)
{
    _widgetMapper->setCurrentModelIndex(index);
}

void PartUnitDialog::boldifyField(QWidget * widget){
    QFont font = _formLayout->labelForField(widget)->font();
    font.setBold(true);
    _formLayout->labelForField(widget)->setFont(font);
}

void PartUnitDialog::accept()
{
    if(_nameEdit->text().isEmpty()){
        boldifyField(_nameEdit);
        QMessageBox::critical(this,tr("Invalid unit"),tr("Unit name is required"),QMessageBox::NoButton);
        return;
    }
    if(_abbreviationEdit->text().isEmpty()){
        boldifyField(_abbreviationEdit);
        QMessageBox::critical(this,tr("Invalid unit"),tr("Unit abbreviation is required"),QMessageBox::NoButton);
        return;
    }
    qDebug("Submit");
    _widgetMapper->submit();
    QDialog::accept();
}

void PartUnitDialog::reject(){
    QDialog::reject();
}

