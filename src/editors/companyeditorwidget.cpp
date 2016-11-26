#include "companyeditorwidget.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDataWidgetMapper>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include "models/basicentitytablemodel.h"

CompanyEditorWidget::CompanyEditorWidget(QWidget *parent) :
    AbstractEditor(parent)
{
    _nameLineEdit = new QLineEdit;
    _websiteLineEdit = new QLineEdit;
    _commentTextEdit = new QPlainTextEdit;
    _commentTextEdit->setTabChangesFocus(true);
    setFocusProxy(_nameLineEdit);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(2);

    QLabel * nameLabel = new QLabel(tr("Name"));
    nameLabel->setBuddy(_nameLineEdit);
    layout->addWidget(nameLabel);
    layout->addWidget(_nameLineEdit);
    layout->addSpacing(8);

    QLabel * siteLabel = new QLabel(tr("Website"));
    siteLabel->setBuddy(_websiteLineEdit);
    layout->addWidget(siteLabel);
    layout->addWidget(_websiteLineEdit);
    layout->addSpacing(8);

    QLabel * commentLabel = new QLabel(tr("Comment"));
    commentLabel->setBuddy(_commentTextEdit);
    layout->addWidget(commentLabel);
    layout->addWidget(_commentTextEdit);
    layout->addStretch(1);

    setLayout(layout);
    /*
    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), _nameLineEdit);
    formLayout->addRow(tr("Website:"), _websiteLineEdit);
    formLayout->addRow(tr("Comment:"), _commentTextEdit);
    setLayout(formLayout);
    */


    _mapper = new QDataWidgetMapper(this);    
    connect(_nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(_websiteLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(_commentTextEdit, SIGNAL(textChanged()), this, SLOT(slotContentChanged()));
}

CompanyEditorWidget::~CompanyEditorWidget()
{
}

/*
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
*/
void CompanyEditorWidget::setModel(QAbstractItemModel * model)
{   
    _mapper->setModel(model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, CompanyTableModel::ColumnName);
    _mapper->addMapping(_websiteLineEdit, CompanyTableModel::ColumnWebsite);
    _mapper->addMapping(_commentTextEdit, CompanyTableModel::ColumnComment);
}

void CompanyEditorWidget::setCurrentIndex(int row)
{
    _commentTextEdit->blockSignals(true);
    _mapper->setCurrentIndex(row);
    _commentTextEdit->blockSignals(false);
    setEnabled(row >= 0);
}

int CompanyEditorWidget::currentIndex() const {
    return _mapper->currentIndex();
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
