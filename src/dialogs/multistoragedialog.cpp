#include "multistoragedialog.h"
#include "ui_multistoragedialog.h"

MultiStorageDialog::MultiStorageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiStorageDialog)
{
    ui->setupUi(this);    
    connect(ui->templateLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotUpdatePreview()));
    connect(ui->initialValueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotUpdatePreview()));
    connect(ui->countSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotUpdatePreview()));
    slotUpdatePreview();
}

MultiStorageDialog::~MultiStorageDialog()
{
    delete ui;
}

void MultiStorageDialog::slotUpdatePreview()
{
    const QChar fillChar('0');
    int fieldWidth = 0;

    QString nameTemplate = ui->templateLineEdit->text();
    int idx = nameTemplate.indexOf('#');

    if(idx >= 0){
        //Check if the user escaped the # character.
        if(idx > 0){
            if(nameTemplate.at(idx-1) == '\\'){
                nameTemplate.remove(idx-1, 1);
            }
        }
        //Find the width of the field
        int endIdx = idx;
        while(endIdx < nameTemplate.length() && nameTemplate.at(endIdx) == '#'){
            ++endIdx;
        }
        fieldWidth = endIdx - idx;
        if(fieldWidth > 0){
            nameTemplate.replace(idx, fieldWidth, "%1");
        }
    }
    int count = ui->countSpinBox->value();
    int n = ui->initialValueSpinBox->value();
    _previewList.clear();
    while(count > 0){
        QString name = nameTemplate.arg(n, fieldWidth, 10, fillChar);
        _previewList.append(name);
        count--;
        n++;
    }
    ui->previewListWidget->clear();
    ui->previewListWidget->addItems(_previewList);
}

QStringList MultiStorageDialog::listOfNames() const
{
    return _previewList;
}
