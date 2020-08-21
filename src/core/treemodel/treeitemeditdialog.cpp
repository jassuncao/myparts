#include "treeitemeditdialog.h"
#include "core/iconsrepository.h"
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QComboBox>
#include <QDir>
#include <QStandardItemModel>
#include <QDebug>

TreeItemEditDialog::TreeItemEditDialog(IconsRepository * iconsRepo, QWidget *parent) :
    QDialog(parent)
{
    _nameEdit = new QLineEdit(this);
    _descriptionEdit = new QPlainTextEdit(this);
    QFontMetrics m (_descriptionEdit->font()) ;
    int rowHeight = m.lineSpacing() ;
    _descriptionEdit->setMaximumHeight(4 * rowHeight) ;
    _descriptionEdit->setTabChangesFocus(true);
    _iconCombo = new QComboBox(this);
    _iconCombo->setModel(iconsRepo->model(this));
    _defaultIconId = iconsRepo->getDefaultIcon();

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    _formLayout = new QFormLayout;
    _formLayout->addRow(tr("Name:"),_nameEdit);
    _formLayout->addRow(tr("Description:"),_descriptionEdit);
    _formLayout->addRow(tr("Icon:"), _iconCombo);

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

QString TreeItemEditDialog::itemIconId() const
{
    int idx = _iconCombo->currentIndex();
    if(idx >= 0){
        QVariant var = _iconCombo->model()->index(idx, 0).data(Qt::EditRole);
        return var.toString();
    }
    return QString();
}

void TreeItemEditDialog::setItemIconId(QString iconId){
    QAbstractItemModel * iconsModel = _iconCombo->model();
    QModelIndex startIndex = iconsModel->index(0, 0);
    QString toFind = iconId.isEmpty() ? _defaultIconId : iconId;
    QModelIndexList res = iconsModel->match(startIndex, Qt::EditRole, toFind);
    if(!res.isEmpty()){
        _iconCombo->setCurrentIndex(res.first().row());
    }
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
