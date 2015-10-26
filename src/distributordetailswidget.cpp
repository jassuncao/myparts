#include "distributordetailswidget.h"
#include "qsqltablemodel.h"
#include "qsqlerror.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QDataWidgetMapper>
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>

DistributorDetailsWidget::DistributorDetailsWidget(QWidget *parent)
    : QWidget(parent)
{    
    _nameLineEdit = new QLineEdit;
    _websiteLineEdit = new QLineEdit;
    _commentTextEdit =new QTextEdit;
    _commentTextEdit->setTabChangesFocus(true);

    QFormLayout * layout1 = new QFormLayout;
    layout1->addRow(tr("Name:"), _nameLineEdit);
    layout1->addRow(tr("Website:"), _websiteLineEdit);
    layout1->addRow(tr("Comment:"), _commentTextEdit);

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    _cancelButton = _buttonBox->button(QDialogButtonBox::Cancel);
    _saveButton = _buttonBox->button(QDialogButtonBox::Ok);
    _deleteButton = new QPushButton(tr("Delete Distributor"));
    _buttonBox->addButton(_deleteButton, QDialogButtonBox::ResetRole);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    _mapper = new QDataWidgetMapper(this);
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_websiteLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_commentTextEdit, SIGNAL(textChanged()), this, SLOT(slotSetDirty()));

    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(slotReject()));
    connect(_deleteButton, SIGNAL(clicked()), this, SLOT(slotDelete()));
}

DistributorDetailsWidget::~DistributorDetailsWidget()
{
}

QModelIndex DistributorDetailsWidget::currentModelIndex() const
{
    return _currentIndex;
}

void DistributorDetailsWidget::setCurrentModelIndex(const QModelIndex & modelIndex, bool newRecord)
{
    _currentIndex = modelIndex;
    _newRecord = newRecord;
    _mapper->setCurrentModelIndex(modelIndex);
    setEnabled(_currentIndex.isValid());
    if(newRecord){                
        _saveButton->setText(tr("Save Distributor"));
        _nameLineEdit->setFocus();
    }
    else{        
        _saveButton->setText(tr("Save Changes"));
    }
    _saveButton->setEnabled(newRecord);
    _cancelButton->setEnabled(newRecord);
    _deleteButton->setEnabled(!newRecord);
}

void DistributorDetailsWidget::setModel(QAbstractItemModel * model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, ColumnName);
    _mapper->addMapping(_websiteLineEdit, ColumnWebsite);
    _mapper->addMapping(_commentTextEdit, ColumnComment);
}

void DistributorDetailsWidget::slotSetDirty()
{
   _saveButton->setEnabled(true);
   _cancelButton->setEnabled(true);
}

/*
void DistributorDetailsWidget::slotResetDirty()
{
    _newRecord =false;
    _dirty = false;
    _saveButton->setEnabled(false);
    _cancelButton->setEnabled(false);
}
*/

void DistributorDetailsWidget::slotAccept()
{
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("The distributor must have a name"), QMessageBox::Close);
        return;
    }
    _mapper->submit();
    emit accepted();
}

void DistributorDetailsWidget::slotReject()
{
    qDebug()<<"Reject changes to distributor with model index" << _currentIndex ;
    if(!isNew()){
        _mapper->revert();
    }
    emit rejected();
}

void DistributorDetailsWidget::slotDelete()
{
    if(_currentIndex.isValid()){
        emit deleted(_currentIndex);
    }
}

