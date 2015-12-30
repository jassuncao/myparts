#include "treeitemeditdialog.h"
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

TreeItemEditDialog::TreeItemEditDialog(QWidget *parent) :
    QDialog(parent)
{
    _nameEdit = new QLineEdit(this);
    _descriptionEdit = new QPlainTextEdit(this);
    QFontMetrics m (_descriptionEdit->font()) ;
    int rowHeight = m.lineSpacing() ;
    _descriptionEdit->setMaximumHeight(4 * rowHeight) ;
    _descriptionEdit->setTabChangesFocus(true);

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
}


TreeItemEditDialog::~TreeItemEditDialog()
{
}

void TreeItemEditDialog::setItemName(const QString & name)
{
    _nameEdit->setText(name);
}

QString TreeItemEditDialog::itemName() const
{
    return _nameEdit->text();
}

void TreeItemEditDialog::setItemDescription(const QString & description)
{
    _descriptionEdit->setPlainText(description);
}

QString TreeItemEditDialog::itemDescription() const
{
    return _descriptionEdit->toPlainText();
}

void TreeItemEditDialog::accept()
{
    if(_nameEdit->text().isEmpty()){
        QFont font = _formLayout->labelForField(_nameEdit)->font();
        font.setBold(true);
        _formLayout->labelForField(_nameEdit)->setFont(font);
        QMessageBox::critical(this,tr("Invalid input"),tr("Name is a required field"),QMessageBox::NoButton);
    }
    else{
        qDebug("Submit");
        QDialog::accept();
    }
}

void TreeItemEditDialog::reject(){
    QDialog::reject();
}
