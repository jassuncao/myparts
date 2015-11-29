#include "attachmentselectiondialog.h"
#include "ui_attachmentselectiondialog.h"
#include <QFileDialog>

AttachmentSelectionDialog::AttachmentSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AttachmentSelectionDialog)
{
    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(slotBrowseButtonClicked()));
}

AttachmentSelectionDialog::~AttachmentSelectionDialog()
{
    delete ui;
}

void AttachmentSelectionDialog::slotBrowseButtonClicked()
{
    QFileDialog dlg(this);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFile);
    if(dlg.exec()){
        QStringList fileNames = dlg.selectedFiles();
        if(fileNames.size()>0){
            QString selectedFile = fileNames.first();
            ui->filenameLineEdit->setText(selectedFile);
        }
    }
}

AttachmentSelectionDialog::ResourceKind AttachmentSelectionDialog::resourceKind()
{
    if(ui->localResRadioButton->isChecked()){
        return LocalResource;
    }
    else {
        return RemoteResource;
    }
}

QUrl AttachmentSelectionDialog::value() const
{
    if(ui->localResRadioButton->isChecked()){
        return QUrl::fromLocalFile(ui->filenameLineEdit->text());
    }
    else{
        return QUrl::fromUserInput(ui->urlLineEdit->text());
    }
}


