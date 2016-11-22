#include "projecteditorwidget.h"
#include "dialogs/attachmentselectiondialog.h"
#include "models/customtablemodel.h"
#include "models/basicentitytablemodel.h"
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
    AbstractEditor(parent), _model(0)
{
    //Info groupbox START
    _nameLineEdit = new QLineEdit;
    _descriptionLineEdit = new QLineEdit;
    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), _nameLineEdit);
    formLayout->addRow(tr("Description:"), _descriptionLineEdit);
    setFocusProxy(_nameLineEdit);
    //Info groupbox END
}

void ProjectEditorWidget::setModel(QAbstractItemModel * model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, PackageTableModel::ColumnName);
    _mapper->addMapping(_descriptionLineEdit, PackageTableModel::ColumnDescription);
}

void ProjectEditorWidget::setCurrentIndex(int row)
{
    _mapper->setCurrentIndex(row);
    setEnabled(row >= 0);
    QVariant packageId = _model->index(row, PackageTableModel::ColumnId).data(Qt::EditRole);
    qDebug()<<"packageId is "<< packageId;
    _attachmentModel->setCurrentForeignKey(packageId);
    _attachmentModel->select();
}

int ProjectEditorWidget::currentIndex() const {
    return _mapper->currentIndex();
}

bool ProjectEditorWidget::validate()
{
    if(_nameLineEdit->text().isEmpty()){
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
    QModelIndex index = _attachmentsTable->currentIndex();
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
    _removeAttachmentButton->setEnabled(current.isValid());
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

