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
#include "widgets/comboitemdelegate.h"
#include "widgets/validatingitemdelegate.h"
#include "models/projectparttablemodel.h"
#include "models/alignmentproxymodel.h"
#include "widgets/partpicker.h"
#include "models/modelsrepository.h"

ProjectEditorWidget::ProjectEditorWidget(ModelsRepository * modelsRepo, QWidget *parent) :
    AbstractEditor(parent),
    ui(new Ui::ProjectEditorForm),
    _modelsRepo(modelsRepo),
    _model(0)
{
    ui->setupUi(this);
    _attachmentModel = AttachmentTableModel3::createNewPackageAttachmentModel(this);
    //_partsModel = ProjectPartTableModel::createNew(this);
    _partsModel = new ProjectPartTableModel(this);
    _mapper = new QDataWidgetMapper(this);

    _partsAlignmentProxyModel = new AlignmentProxyModel(this);
    _partsAlignmentProxyModel->setSourceModel(_partsModel);
    _partsAlignmentProxyModel->setColumnAlignment(ProjectPartTableModel::Quantity, Qt::AlignTrailing | Qt::AlignVCenter);

    ui->attachmentsTableView->setSelectionMode(QTableView::SingleSelection);
    ui->attachmentsTableView->setSelectionBehavior(QTableView::SelectRows);
    ui->attachmentsTableView->setModel(_attachmentModel);
    ui->attachmentsTableView->verticalHeader()->setVisible(false);
    ui->attachmentsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->attachmentsTableView->setColumnWidth(0, 512);    
    //ui->attachmentsTableView->setDragDropMode(QAbstractItemView::DragDrop);



    ui->partsTableView->setSelectionMode(QTableView::SingleSelection);
    ui->partsTableView->setSelectionBehavior(QTableView::SelectRows);
    ui->partsTableView->setModel(_partsAlignmentProxyModel);
    ui->partsTableView->verticalHeader()->setVisible(false);
    ui->partsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->partsTableView->setItemDelegate(new ComboItemDelegate(this));
    ui->partsTableView->setItemDelegateForColumn(ProjectPartTableModel::Quantity, new ValidatingItemDelegate(new QIntValidator(), this));
    ui->partsTableView->setSortingEnabled(true);

    connect(ui->nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(ui->descriptionTextEdit, SIGNAL(textChanged()), this, SLOT(slotContentChanged()));

    connect(ui->attachmentsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentAttachmentRowChanged(QModelIndex,QModelIndex)));
    connect(ui->attachmentsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));
    connect(_attachmentModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotContentChanged()));
    connect(ui->addAttachmentButton, SIGNAL(clicked()), this, SLOT(slotAddAttachment()));
    connect(ui->deleteAttachmentButton, SIGNAL(clicked()), this, SLOT(slotRemoveAttachment()));

    connect(ui->partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentPartRowChanged(QModelIndex,QModelIndex)));
    //connect(ui->partsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));
    connect(_partsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotContentChanged()));
    connect(ui->addPartButton, SIGNAL(clicked()), this, SLOT(slotAddPart()));
    connect(ui->deletePartButton, SIGNAL(clicked()), this, SLOT(slotRemovePart()));
    connect(ui->assignPartButton, SIGNAL(clicked(bool)), this, SLOT(slotAssignPart()));

    setFocusProxy(ui->nameLineEdit);
}

void ProjectEditorWidget::setModel(QAbstractItemModel * model)
{
    if(_model == model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);        
    _mapper->addMapping(ui->nameLineEdit, ProjectTableModel::ColumnName);
    _mapper->addMapping(ui->descriptionTextEdit, ProjectTableModel::ColumnDescription);
}

void ProjectEditorWidget::setCurrentIndex(int row)
{
    bool previousState = ui->descriptionTextEdit->blockSignals(true);
    _mapper->setCurrentIndex(row);
    ui->descriptionTextEdit->blockSignals(previousState);
    setEnabled(row >= 0);
    QVariant parentId = _model->index(row, ProjectTableModel::ColumnId).data(Qt::EditRole);
    qDebug()<<"project ID is "<< parentId;
    _attachmentModel->setCurrentForeignKey(parentId);
    _attachmentModel->select();

    _partsModel->setProject(parentId);
    _partsModel->select();
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

   //_partsModel->setCurrentForeignKey(id);
   _partsModel->setProject(id);
   _partsModel->submitAll();
}

void ProjectEditorWidget::revert()
{
    _mapper->revert();
    _attachmentModel->select();
    _partsModel->select();
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

void ProjectEditorWidget::slotAddPart()
{
    int rowCount = _partsModel->rowCount();
    if(_partsModel->insertRow(rowCount)){
        QModelIndex index = _partsModel->index(rowCount,0);
        ui->partsTableView->setCurrentIndex(index);
        ui->partsTableView->edit(index);
    }
}

void ProjectEditorWidget::slotRemovePart()
{
    QModelIndex index = ui->partsTableView->currentIndex();
    if(index.isValid()){
        qDebug()<<"Removing row";
        const QModelIndex sourceIndex = _partsAlignmentProxyModel->mapToSource(index);
        bool res = _partsModel->removeRow(sourceIndex.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
    }
}


void ProjectEditorWidget::slotAssignPart()
{
    QModelIndex index = ui->partsTableView->currentIndex();
    if(index.isValid()){
        qDebug()<<"Assign part row";
        const QModelIndex sourceIndex = _partsAlignmentProxyModel->mapToSource(index);
        QDialog pickerDlg(this);
        PartPickerView * pickerView = new PartPickerView(_modelsRepo, &pickerDlg);
        QVBoxLayout * layout = new QVBoxLayout;
        layout->addWidget(pickerView);
        pickerDlg.setLayout(layout);
        pickerDlg.exec();
        /*
        bool res = _partsModel->removeRow(sourceIndex.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
        */
    }
}

void ProjectEditorWidget::slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    ui->deleteAttachmentButton->setEnabled(current.isValid());
}

void ProjectEditorWidget::slotCurrentPartRowChanged(const QModelIndex &current, const QModelIndex &)
{
    ui->deletePartButton->setEnabled(current.isValid());
    ui->viewPartButton->setEnabled(current.isValid());
}

void ProjectEditorWidget::slotAttachmentDoubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    if(index.column() == 0){
        const QModelIndex & urlColIndex = _attachmentModel->index(index.row(), AttachmentTableModel3::ColumnURL);
        QString url = urlColIndex.data(Qt::EditRole).toString();
        QDesktopServices::openUrl(QUrl::fromUserInput(url));
    }
}

