#include "editormanagerview.h"
#include "editors/companyeditorwidget.h"
#include "editors/packageeditorwidget.h"
#include "editors/projecteditorwidget.h"
#include "widgets/itemstreeview.h"
#include "widgets/listnavigatorwidget.h"
#include "widgets/qsearchlineedit.h"
#include "styledbar.h"
#include "minisplitter.h"
#include <QHBoxLayout>
#include <QListView>
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QSqlTableModel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QStackedLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSortFilterProxyModel>

static const int NO_DATA_WIDGET = 0;
static const int EDITOR_WIDGET = 1;

EditorManagerHelper::~EditorManagerHelper()
{

}

QWidget* DistributorManagerHelper::createNoDataWidget() const
{
    QLabel * l = new QLabel(QCoreApplication::translate("DistributorManagerHelper", "Select a distributor"));
    l->setAlignment(Qt::AlignCenter);
    return l;
}

AbstractEditor* DistributorManagerHelper::createEditor() const {
    return new CompanyEditorWidget;
}

QWidget* ManufacturerManagerHelper::createNoDataWidget() const
{
    QLabel * l = new QLabel(QCoreApplication::translate("ManufacturerManagerHelper", "Select a manufacturer"));
    l->setAlignment(Qt::AlignCenter);
    return l;
}

AbstractEditor* ManufacturerManagerHelper::createEditor() const {
    return new CompanyEditorWidget;
}

QWidget* PackageManagerHelper::createNoDataWidget() const
{
    QLabel * l = new QLabel(QCoreApplication::translate("PackageManagerHelper", "Select a package"));
    l->setAlignment(Qt::AlignCenter);
    return l;
}

AbstractEditor* PackageManagerHelper::createEditor() const {
    return new PackageEditorWidget;
}

ProjectManagerHelper::ProjectManagerHelper(ModelsRepository * modelsRepo) :
    _modelsRepo(modelsRepo)
{}

QWidget* ProjectManagerHelper::createNoDataWidget() const
{
    QLabel * l = new QLabel(QCoreApplication::translate("ProjectManagerHelper", "Select a project"));
    l->setAlignment(Qt::AlignCenter);
    return l;
}

AbstractEditor* ProjectManagerHelper::createEditor() const {
    return new ProjectEditorWidget(_modelsRepo);
}


EditorManagerView::EditorManagerView(const EditorManagerHelper *helper, BasicEntityTableModel *model, QWidget *parent)
    : Manhattan::MiniSplitter(parent), _helper(helper), _model(model), _dirty(false), _newRow(false)
{
    _navigatorWidget = new ListNavigatorWidget(_helper->mainTitle());
    _editorWidget = _helper->createEditor();    

    /*
    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    _cancelButton = _buttonBox->button(QDialogButtonBox::Cancel);
    _cancelButton->setEnabled(false);
    _saveButton = _buttonBox->button(QDialogButtonBox::Save);
    _saveButton->setEnabled(false);
    _saveButton->setText(_helper->saveChangesButtonText());
    _deleteButton = new QPushButton(_helper->deleteButtonText());
    _buttonBox->addButton(_deleteButton, QDialogButtonBox::ResetRole);
    */

    _deleteButton = new QPushButton(tr("Delete"));
    _deleteButton->setEnabled(false);
    _saveButton = new QPushButton(_helper->saveChangesButtonText());
    //_saveButton->setIcon(style()->standardIcon(QStyle::StandardPixmap(QStyle::SP_DialogSaveButton)));
    _saveButton->setEnabled(false);
    _cancelButton = new QPushButton(tr("Cancel"));
    //_cancelButton->setIcon(style()->standardIcon(QStyle::StandardPixmap(QStyle::SP_DialogCancelButton)));
    _cancelButton->setEnabled(false);

    QHBoxLayout * actionsLayout = new QHBoxLayout;
    //actionsLayout->setMargin(6);
    actionsLayout->setSpacing(8);
    actionsLayout->addWidget(_deleteButton);
    actionsLayout->addStretch();
    actionsLayout->addWidget(_saveButton);
    actionsLayout->addWidget(_cancelButton);

    QVBoxLayout * editorLayout = new QVBoxLayout;
    editorLayout->setMargin(9);

    editorLayout->addWidget(_editorWidget);
    editorLayout->addLayout(actionsLayout);
    QWidget * editorWapper = new QWidget;
    editorWapper->setLayout(editorLayout);

    _stackedLayout = new QStackedLayout;
    _stackedLayout->addWidget(helper->createNoDataWidget());
    _stackedLayout->addWidget(editorWapper);

    QVBoxLayout * rightSideLayout = new QVBoxLayout;
    rightSideLayout->setMargin(0);
    rightSideLayout->setSpacing(0);
    rightSideLayout->addWidget(new Manhattan::StyledBar);
    rightSideLayout->addLayout(_stackedLayout);

    QWidget * rightSplitWidget = new QWidget;
    rightSplitWidget->setLayout(rightSideLayout);

    insertWidget(0, _navigatorWidget);
    insertWidget(1, rightSplitWidget);
    setStretchFactor(0, 0);
    setStretchFactor(1, 1);

    _model->select();

    _filterProxyModel = new QSortFilterProxyModel(this);
    _filterProxyModel->setSourceModel(_model);
    _filterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _filterProxyModel->setFilterKeyColumn(model->getNameColumn());
    _navigatorWidget->setModel(_filterProxyModel);

    _navigatorWidget->setModelColumn(model->getNameColumn());
    _editorWidget->setModel(_model);
    _editorWidget->setCurrentIndex(-1);

    connect(_navigatorWidget, SIGNAL(filterChanged(QString)), this, SLOT(slotFilterChanged(QString)));
    connect(_navigatorWidget, SIGNAL(itemSelected(QModelIndex)), this, SLOT(slotItemSelected(QModelIndex)), Qt::QueuedConnection);
    connect(_navigatorWidget, SIGNAL(addNewItem()), this, SLOT(slotAddItem()));
    connect(_navigatorWidget, SIGNAL(deleteItem(QModelIndex)), this, SLOT(slotDeleteItem(QModelIndex)));
    connect(_editorWidget, SIGNAL(contentChanged()), this, SLOT(slotContentChanged()));
    //connect(_model, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPrimeInsert(int,QSqlRecord&)));

    //connect(_buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    //connect(_buttonBox, SIGNAL(rejected()), this, SLOT(slotReject()));
    connect(_saveButton, SIGNAL(clicked(bool)), this, SLOT(slotAccept()));
    connect(_cancelButton, SIGNAL(clicked(bool)), this, SLOT(slotReject()));
    connect(_deleteButton, SIGNAL(clicked()), this, SLOT(slotDelete()));  
}

EditorManagerView::~EditorManagerView()
{
    delete _helper;
}
/*
void EditorManagerView::setModel(QAbstractTableModel * model)
{
    _model = model;
    _navigatorWidget->setModel(_model);
    _navigatorWidget->setModelColumn(helper->itemLabelColumn());
    _editorWidget->setModel(_model);
    _editorWidget->setCurrentModelIndex(QModelIndex());
}
*/
void EditorManagerView::slotAddItem()
{
    if(_dirty){
        if(discardChangesConfirmation()){
            slotReject();
        }
        else{
            return;
        }
    }
    int row = _model->rowCount();
    if(_model->insertRow(row)){
        _newRow = true;
        _navigatorWidget->setCurrentRow(row);
        _editorWidget->setFocus();
    }
    else{
        qWarning("Failed to insert row. Reason is %s", qPrintable(_model->lastError().text()));
        //TODO: Show error message
    }
}

void EditorManagerView::restoreCurrentIndex(int row)
{
    bool oldState =_navigatorWidget->blockSignals(true);
    _navigatorWidget->setCurrentRow(row);
    _navigatorWidget->blockSignals(oldState);
}

bool EditorManagerView::discardChangesConfirmation()
{
    QMessageBox msgBox;
    msgBox.setText(tr("The current item has unsaved changes."));
    msgBox.setInformativeText(tr("Are you sure you want to discard your changes?"));
    msgBox.setStandardButtons(QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Discard);
    return msgBox.exec()==QMessageBox::Discard;
}

void EditorManagerView::setDirty(bool dirty)
{
    if(_dirty == dirty){ return; }
    _dirty = dirty;
    _saveButton->setEnabled(dirty);
    _cancelButton->setEnabled(dirty);
}

void EditorManagerView::slotItemSelected(const QModelIndex &index)
{
    if(_dirty){
        qDebug()<<"Is dirty ";
        if(discardChangesConfirmation()){
            slotReject();
        }
        else{
            restoreCurrentIndex(_editorWidget->currentIndex());
            return;
        }
    }
    QModelIndex sourceIndex = _filterProxyModel->mapToSource(index);
    _editorWidget->setCurrentIndex(sourceIndex.row());
    if(sourceIndex.isValid() == false){
        _stackedLayout->setCurrentIndex(NO_DATA_WIDGET);
        return;
    }

    _stackedLayout->setCurrentIndex(EDITOR_WIDGET);
    if(_newRow){
        //Editing a brand new
        _saveButton->setText(_helper->saveNewButtonText());        
        _deleteButton->setEnabled(false);        
        _editorWidget->setFocus();
        setDirty(true);
    }
    else{
        //Editing an existing element
        _saveButton->setText(_helper->saveChangesButtonText());
        _deleteButton->setEnabled(true);
        setDirty(false);
    }
}

void EditorManagerView::slotDelete()
{
    deleteRow(_editorWidget->currentIndex());
}


void EditorManagerView::deleteRow(int row)
{
    _dirty = false;
    qDebug()<<"Deleting row " << row;
    if(row < 0){
        return;
    }
    int res = QMessageBox::question(this, tr("Confirmation"), tr("Do you really wish to delete the selected item?"), QMessageBox::Yes | QMessageBox::No);
    if( res != QMessageBox::Yes) {
        return;
    }

    //Resets the editor state
    _editorWidget->setCurrentIndex(-1);

    _model->removeRow(row);
    _model->database().transaction();
    if(_model->submitAll()){
        _navigatorWidget->setCurrentRow(row - 1);
        _model->database().commit();
    }
    else{
        qWarning()<<"Failed to delete items. Reason:"<<_model->database().lastError();
        _model->database().rollback();
    }
}

void EditorManagerView::slotDeleteItem(const QModelIndex & index)
{
    if(index.isValid()){
        deleteRow(index.row());
    }
}

void EditorManagerView::slotFilterChanged(const QString &filterText)
{
    _filterProxyModel->setFilterFixedString(filterText);
}

void EditorManagerView::slotAccept()
{    
    if(_editorWidget->validate() == false){
        return;
    }    
    QVariant elementId = commitChanges();
    /* submitAll resets the model making the indexes invalid.
     * We iterate over the records to find the row matching the current id
     */
    const QModelIndex newIndex = _model->findIndexOfKeyValue(elementId);
    //int row = findRowNumber(elementId);
    //_navigatorWidget->setCurrentRow(row);
    const QModelIndex proxyIndex = _filterProxyModel->mapFromSource(newIndex);
    _navigatorWidget->setCurrentIndex(proxyIndex);
}

/**
 * @brief CompanyManagerView::commitChanges
 * @return the ID of the commited element
 */
QVariant EditorManagerView::commitChanges()
{
    //We keep the id of element being edited to use later for locating the row associated to the element
    QVariant id;
    bool success;
    qDebug() << "Commiting changes";
    _editorWidget->submit();    
    if(_newRow){
        //We need to submit to get the generated ID
        success=_model->submitAll();
        id = _model->lastInsertedId();
        qDebug() << "New item has id " << id;
    }
    else {
        //Get the ID before the submit or we will loose it
        id = _model->keyValue(_editorWidget->currentIndex());
        success = _model->submitAll();
    }
    //For a brand new item we need to pass the last inserted ID. It will be needed to insert child entities
    qDebug() << "Saving item with id "<<id<< " childs (if any) ";
    _editorWidget->submitChilds(id);
    if(success){
        setDirty(false);
        _newRow = false;
        qDebug() << "Item saved.";
    }
    else{
        qWarning()<<"Failed to submit changes ";
        qWarning("Reason is %s", qPrintable(_model->lastError().text()));
    }
    return id;
}

void EditorManagerView::slotReject()
{    
    //int row = _editorWidget->currentIndex();
    //_model->revertRow(row);
    _editorWidget->revert();
    setDirty(false);
}

void EditorManagerView::slotContentChanged()
{
    setDirty(true);
}

//int EditorManagerView::findRowNumber(QVariant idValue)
//{
//    int row = 0;
//    bool dataAvailable = true;
//    while(dataAvailable){
//        for(; row<_model->rowCount(); ++row){
//            const QModelIndex & idx = _model->rootIndex(row);
//            QVariant id = _model->data(idx);
//            if(id==idValue)
//                return row;
//        }
//        dataAvailable = _model->canFetchMore();
//        if(dataAvailable){
//            _model->fetchMore();
//        }
//    }
//    return -1;
//}

