#include "partcategorydialog.h"
#include <QtGui>
#include <QDebug>
#include <QDataWidgetMapper>
#include "entities/categoriesdao.h"

PartCategoryDialog::PartCategoryDialog(QAbstractItemModel *model, QWidget *parent) :
    QDialog(parent)
{
    _nameEdit = new QLineEdit(this);
    _descriptionEdit = new QPlainTextEdit(this);
    QFontMetrics m (_descriptionEdit->font()) ;
    int RowHeight = m.lineSpacing() ;
    _descriptionEdit->setMaximumHeight(4 * RowHeight) ;

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    _formLayout = new QFormLayout;
    _formLayout->addRow(tr("Name:"),_nameEdit);
    _formLayout->addRow(tr("Description:"),_descriptionEdit);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(_formLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    _widgetMapper = new QDataWidgetMapper(this);
    _widgetMapper->setModel(model);
    _widgetMapper->addMapping(_nameEdit, Entities::CategoriesDAO::NAME_COL);
    _widgetMapper->addMapping(_descriptionEdit, Entities::CategoriesDAO::DESCRIPTION_COL);
    _widgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    //qDebug("Spacing %d", _formLayout->verticalSpacing());
}

void PartCategoryDialog::setRootIndex(const QModelIndex &index)
{
    _widgetMapper->setRootIndex(index);
}

void PartCategoryDialog::setCurrentIndex(const int index)
{
    _widgetMapper->setCurrentIndex(index);
}

void PartCategoryDialog::setCurrentModelIndex(const QModelIndex &index)
{
    _widgetMapper->setCurrentModelIndex(index);
}

void PartCategoryDialog::accept()
{
    if(_nameEdit->text().isEmpty()){
        QFont font = _formLayout->labelForField(_nameEdit)->font();
        font.setBold(true);
        _formLayout->labelForField(_nameEdit)->setFont(font);
        QMessageBox::critical(this,tr("Invalid category name"),tr("Category name is required"),QMessageBox::NoButton);
    }
    else{
        qDebug("Submit");
        _widgetMapper->submit();
        QDialog::accept();
    }
}

void PartCategoryDialog::reject(){   
    QDialog::reject();
}




