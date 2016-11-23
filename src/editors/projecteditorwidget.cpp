#include "projecteditorwidget.h"
#include "dialogs/attachmentselectiondialog.h"
#include "models/customtablemodel.h"
#include "models/projecttablemodel.h"
#include "ui_projecteditorform.h"
#include "utils.h"
#include "dialogs/imageviewer.h"
#include <QDebug>
#include <QToolButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QAction>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QDataWidgetMapper>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QIdentityProxyModel>
#include <QSqlError>
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QDesktopServices>

ProjectEditorWidget::ProjectEditorWidget(QWidget *parent) :
    AbstractEditor(parent),
    ui(new Ui::ProjectEditorForm),
    _model(0)
{
    ui->setupUi(this);

    _attachmentModel = AttachmentTableModel3::createNewPackageAttachmentModel(this);
    _mapper = new QDataWidgetMapper(this);
    connect(ui->nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(ui->descriptionTextEdit, SIGNAL(textChanged()), this, SLOT(slotContentChanged()));
}

void ProjectEditorWidget::setModel(QAbstractItemModel * model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);        
    _mapper->addMapping(ui->nameLineEdit, ProjectTableModel::ColumnName);
    _mapper->addMapping(ui->descriptionTextEdit, ProjectTableModel::ColumnDescription);
}

void ProjectEditorWidget::setCurrentIndex(int row)
{
    _mapper->setCurrentIndex(row);
    setEnabled(row >= 0);
    QVariant packageId = _model->index(row, ProjectTableModel::ColumnId).data(Qt::EditRole);
    qDebug()<<"project ID is "<< packageId;
    _attachmentModel->setCurrentForeignKey(packageId);
    _attachmentModel->select();
}

int ProjectEditorWidget::currentIndex() const {
    return _mapper->currentIndex();
}

bool ProjectEditorWidget::validate()
{
    if(ui->nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("Please fill the name field"), QMessageBox::Close);
        return false;
    }
    return true;
}

void ProjectEditorWidget::submit()
{
    _mapper->submit();
}

void ProjectEditorWidget::submitChilds(const QVariant & id)
{
   _attachmentModel->setCurrentForeignKey(id);
   _attachmentModel->submitAll();
}

void ProjectEditorWidget::revert()
{
    _mapper->revert();
    _attachmentModel->select();
}

void ProjectEditorWidget::slotAddAttachment()
{
    AttachmentSelectionDialog dlg(this);
    if(dlg.exec()){
        QUrl resourceUrl = dlg.value();
        int row = _attachmentModel->rowCount();
        _attachmentModel->insertRow(row);
        const QModelIndex & index = _attachmentModel->index(row, AttachmentTableModel3::ColumnURL);
        _attachmentModel->setData(index, resourceUrl.toString());
    }
}

void ProjectEditorWidget::slotRemoveAttachment()
{
    QModelIndex index = ui->attachmentsTableView->currentIndex();
    if(index.isValid()){
        qDebug()<<"Removing row";
        bool res = _attachmentModel->removeRow(index.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
    }
}

void ProjectEditorWidget::slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    ui->deleteAttachmentButton->setEnabled(current.isValid());
}

void ProjectEditorWidget::slotAttachmentDoubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    if(index.column()==0){
        const QModelIndex & urlColIndex = _attachmentModel->index(index.row(), AttachmentTableModel3::ColumnURL);
        QString url = urlColIndex.data(Qt::EditRole).toString();
        QDesktopServices::openUrl(QUrl::fromUserInput(url));
    }
}

