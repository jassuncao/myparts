#include "quickaddstoragedialog.h"
#include <QtGui>
#include <QtSql>

QuickAddStorageDialog::QuickAddStorageDialog(QWidget *parent) :
    QDialog(parent)
{
    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    _buttonBox->setOrientation(Qt::Horizontal);
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    _nameText = new QLineEdit;
    connect(_nameText,SIGNAL(textChanged(QString)), this, SLOT(on_nameChanged(QString)));
    connect(_nameText,SIGNAL(returnPressed()), this, SLOT(accept()));

    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), _nameText);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);
    setMinimumSize(300,100);
}


void QuickAddStorageDialog::accept()
{
    QString name = _nameText->text();
    if(name.isEmpty())
        return;
    QSqlQuery query;
    if(query.prepare("INSERT INTO part_storage (name) VALUES (?)")){
        query.bindValue(0, name);
        if(query.exec()){
            _addedStorageId = query.lastInsertId();
            QDialog::accept();
            return;
        }
    }
    QMessageBox::warning(this,
                         tr("Error saving changes"),
                         tr("The database reported an error: %1").arg(query.lastError().text()));
}

void QuickAddStorageDialog::on_nameChanged(const QString & text)
{
    _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
