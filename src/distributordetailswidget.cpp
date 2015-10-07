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
    : QWidget(parent),
      _dirty(false)
{    
    _nameLineEdit = new QLineEdit;
    _websiteLineEdit = new QLineEdit;
    _commentTextEdit =new QTextEdit;
    QFormLayout * layout1 = new QFormLayout;
    layout1->addRow(tr("Name:"), _nameLineEdit);
    layout1->addRow(tr("Website:"), _websiteLineEdit);
    layout1->addRow(tr("Comment:"), _commentTextEdit);

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Discard);
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    QPushButton* btnNoKey = _buttonBox->button(QDialogButtonBox::Discard);
    connect(btnNoKey, SIGNAL(clicked()), SLOT(slotReject()));

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    _mapper = new QDataWidgetMapper(this);
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_websiteLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_commentTextEdit, SIGNAL(textChanged()), this, SLOT(slotSetDirty()));

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
        _nameLineEdit->setFocus();
        slotSetDirty();
    }
    else{
        slotResetDirty();
    }
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
   _dirty = true;
   _buttonBox->setEnabled(true);
}

void DistributorDetailsWidget::slotResetDirty()
{
    _newRecord =false;
    _dirty = false;
   _buttonBox->setEnabled(false);
}

void DistributorDetailsWidget::slotAccept()
{
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("The distributor must have a name"), QMessageBox::Close);
        return;
    }
    qDebug()<<"Saving changes to distributor with model index" << _currentIndex ;
    if(!_mapper->submit()){
        qWarning()<<"Failed to submit changes ";
        QSqlTableModel* tableModel = dynamic_cast<QSqlTableModel*>(_model);
        if(tableModel){
            qWarning("Reason is %s", qPrintable(tableModel->lastError().text()));
        }
    }
    slotResetDirty();
    emit accepted();
}

void DistributorDetailsWidget::slotReject()
{
    qDebug()<<"Reject changes to distributor with model index" << _currentIndex ;
    if(!isNew()){
        _mapper->revert();
    }
    slotResetDirty();
    emit rejected();

}

