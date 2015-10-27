#include "companyeditorwidget.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDataWidgetMapper>
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>

CompanyEditorWidget::CompanyEditorWidget(QWidget *parent) :
    AbstractEditor(parent)
{
    _nameLineEdit = new QLineEdit;
    _websiteLineEdit = new QLineEdit;
    _commentTextEdit =new QTextEdit;
    _commentTextEdit->setTabChangesFocus(true);
    setFocusProxy(_nameLineEdit);

    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), _nameLineEdit);
    formLayout->addRow(tr("Website:"), _websiteLineEdit);
    formLayout->addRow(tr("Comment:"), _commentTextEdit);

    setLayout(formLayout);

    _mapper = new QDataWidgetMapper(this);    
    connect(_nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(_websiteLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(_commentTextEdit, SIGNAL(textChanged()), this, SLOT(slotContentChanged()));
}

CompanyEditorWidget::~CompanyEditorWidget()
{
}

QModelIndex CompanyEditorWidget::currentModelIndex() const
{
    return _currentIndex;
}

void CompanyEditorWidget::setCurrentModelIndex(const QModelIndex & modelIndex)
{
    _currentIndex = modelIndex;
    _commentTextEdit->blockSignals(true);
    _mapper->setCurrentModelIndex(modelIndex);
    _commentTextEdit->blockSignals(false);
    setEnabled(_currentIndex.isValid());        
}

void CompanyEditorWidget::setModel(QAbstractItemModel * model)
{   
    _mapper->setModel(model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, ColumnName);
    _mapper->addMapping(_websiteLineEdit, ColumnWebsite);
    _mapper->addMapping(_commentTextEdit, ColumnComment);
}

bool CompanyEditorWidget::validate()
{
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("Please fill the name field"), QMessageBox::Close);
        return false;
    }
    return true;
}

void CompanyEditorWidget::submit()
{   
    _mapper->submit();    
}

void CompanyEditorWidget::revert()
{
    _mapper->revert();
}

/*
void CompanyEditorWidget::slotContentChanged(){
    emit contentChanged();
}*/
