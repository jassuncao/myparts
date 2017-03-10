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

#include <QStyledItemDelegate>
#include <QRegExp>
#include <QSpinBox>

class PartQuantityDelegate : public ValidatingItemDelegate
{
public:
    explicit PartQuantityDelegate(QValidator * validator, QObject *parent = 0) :
        ValidatingItemDelegate(validator, parent),
        _sepRegex(",|;|\\s"),
        _rangeRegex("\\D+(\\d+)\\s*-\\s*\\D(\\d+)")
    {}
    virtual ~PartQuantityDelegate() {}

    void setEditorData(QWidget *editor, const QModelIndex &index) const {
        QVariant var = index.data(Qt::EditRole);
        if(var.isNull()){//Only attempt to guess the quantity if the record is empty
            QString s = index.model()->index(index.row(), ProjectPartTableModel::RefDes).data(Qt::EditRole).toString();
            int quantity = guessQuantityFromRefDes(s);

            QSpinBox * spinbox = dynamic_cast<QSpinBox*>(editor);
            if(spinbox){
                spinbox->setValue(quantity);
                return;
            }
            else{
                QLineEdit * lineEdit = dynamic_cast<QLineEdit*>(editor);
                if(lineEdit){
                    lineEdit->setText(QString::number(quantity));
                    return;
                }
            }
        }
        ValidatingItemDelegate::setEditorData(editor, index);
    }
private:

    int guessQuantityFromRefDes(const QString & s) const {
        int count = 0;
        QStringList tokens = s.split(_sepRegex, QString::SkipEmptyParts);
        foreach (QString token, tokens) {
            int incFactor = 1;
            //Check if the token is something like "C1-C9"
            if(token.size() >= 5 && _rangeRegex.exactMatch(token)){
                //The regex captures the numeric portions
                QStringList groups = _rangeRegex.capturedTexts();
                if(groups.size()==3){//The first group matches the whole string
                    bool ok1, ok2;

                    int v1 = groups.at(1).toInt(&ok1);
                    int v2 = groups.at(2).toInt(&ok2);

                    if(ok1 && ok2){
                        int diff = v2-v1;
                        if(diff >= 0){
                            incFactor = diff + 1;
                        }
                    }
                }
            }
            count+=incFactor;
        }
        return count;
    }

    QRegExp _sepRegex;
    QRegExp _rangeRegex;
};


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
    //ui->partsTableView->setItemDelegateForColumn(ProjectPartTableModel::Quantity, new PartQuantityDelegate(this));
    ui->partsTableView->setItemDelegateForColumn(ProjectPartTableModel::Quantity, new PartQuantityDelegate(new QIntValidator(), this));
    ui->partsTableView->setSortingEnabled(true);

    connect(ui->nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(ui->descriptionTextEdit, SIGNAL(textChanged()), this, SLOT(slotContentChanged()));

    connect(ui->attachmentsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentAttachmentRowChanged(QModelIndex,QModelIndex)));
    connect(ui->attachmentsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));
    connect(_attachmentModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotContentChanged()));
    connect(ui->addAttachmentButton, SIGNAL(clicked()), this, SLOT(slotAddAttachment()));
    connect(ui->deleteAttachmentButton, SIGNAL(clicked()), this, SLOT(slotRemoveAttachment()));

    connect(ui->partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentPartRowChanged(QModelIndex,QModelIndex)));
    connect(ui->partsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotPartsDoubleCkicked(QModelIndex)));
    connect(_partsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotContentChanged()));
    connect(ui->addPartButton, SIGNAL(clicked()), this, SLOT(slotAddPart()));
    connect(ui->deletePartButton, SIGNAL(clicked()), this, SLOT(slotRemovePart()));
    connect(ui->assignPartButton, SIGNAL(clicked(bool)), this, SLOT(slotAssignPart()));

    //connect(_partsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotPartDataChanged(QModelIndex,QModelIndex)));
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
        QModelIndex index = _partsAlignmentProxyModel->index(rowCount, ProjectPartTableModel::RefDes);
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
    if(false == index.isValid()){
        return;
    }
    qDebug()<<"Assign part row";
    const QModelIndex sourceIndex = _partsAlignmentProxyModel->mapToSource(index);
    PartPickerDialog pickerDlg(_modelsRepo, this);
    int res = pickerDlg.exec();
    if(QDialog::Accepted == res){
        QVariant partId = pickerDlg.selectedPartId();
        QVariant partName = pickerDlg.selectedPartName();
        QModelIndex partIndex = _partsModel->index(sourceIndex.row(), ProjectPartTableModel::AssignedPart);
        _partsModel->setData(partIndex, partId, Qt::EditRole);
        _partsModel->setData(partIndex, partName, Qt::DisplayRole);
    }
}

void ProjectEditorWidget::slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    ui->deleteAttachmentButton->setEnabled(current.isValid());
}

void ProjectEditorWidget::slotCurrentPartRowChanged(const QModelIndex &current, const QModelIndex &)
{
    ui->deletePartButton->setEnabled(current.isValid());
    ui->assignPartButton->setEnabled(current.isValid());
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

void ProjectEditorWidget::slotPartsDoubleCkicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    if(index.column() == ProjectPartTableModel::AssignedPart){
        slotAssignPart();
    }
}
