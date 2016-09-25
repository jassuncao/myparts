#include "editormanagerview.h"
#include "editors/companyeditorwidget.h"
#include "editors/packageeditorwidget.h"
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


EditorManagerView::EditorManagerView(const EditorManagerHelper *helper, BasicEntityTableModel *model, QWidget *parent)
    : Manhattan::MiniSplitter(parent), _helper(helper), _model(model), _dirty(false)
{
    _navigatorWidget = new ListNavigatorWidget(_helper->mainTitle());
    _editorWidget = _helper->createEditor();

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    _cancelButton = _buttonBox->button(QDialogButtonBox::Cancel);
    _saveButton = _buttonBox->button(QDialogButtonBox::Ok);
    _deleteButton = new QPushButton(_helper->deleteButtonText());
    _buttonBox->addButton(_deleteButton, QDialogButtonBox::ResetRole);

    QVBoxLayout * editorLayout = new QVBoxLayout;
    editorLayout->setMargin(6);

    editorLayout->addWidget(_editorWidget);
    editorLayout->addWidget(_buttonBox);
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

    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(slotReject()));
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
        _navigatorWidget->setCurrentRow(row);
    }
    else{
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
    if(sourceIndex.isValid()==false){
        _stackedLayout->setCurrentIndex(0);
        return;
    }
    _stackedLayout->setCurrentIndex(1);
    //A brand new row is set with an invalid ID
    if(sourceIndex.data(Qt::EditRole).isValid()){
        //Editing an existing element
        _saveButton->setText(_helper->saveChangesButtonText());
        _saveButton->setEnabled(false);
        _cancelButton->setEnabled(false);
        _deleteButton->setEnabled(true);
    }
    else{
        //Editing a brand new
        _saveButton->setText(_helper->saveNewButtonText());
        _saveButton->setEnabled(true);
        _cancelButton->setEnabled(true);
        _deleteButton->setEnabled(false);
        _editorWidget->setFocus();
        _dirty = true;
    }
}

void EditorManagerView::slotDelete()
{
    deleteRow(_editorWidget->currentIndex());
}


void EditorManagerView::deleteRow(int row)
{
    _dirty = false;
    qDebug()<<"Deleting row "<<row;
    if(row < 0){
        return;
    }
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setText("Do you really wish to delete the selected item?");
    if(msgBox.exec()!=QMessageBox::Yes)
        return;
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
    if(_editorWidget->validate()==false){
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

    _editorWidget->submit();
    //We assume the editor widget is set with a QModelIndex made of (row,Id Column)
    id = _model->keyValue(_editorWidget->currentIndex());
    if(id.isValid()){
        success=_model->submitAll();
    }
    else{
        //If the id is not valid it must be a brand new item. We need to submit to get the generated ID
        success=_model->submitAll();
        id = _model->query().lastInsertId();        
    }
    _editorWidget->submitChilds(id);
    if(success){
        _dirty = false;
    }
    else{
        qWarning()<<"Failed to submit changes ";
        qWarning("Reason is %s", qPrintable(_model->lastError().text()));
    }
    return id;
}

void EditorManagerView::slotReject()
{    
    int row = _editorWidget->currentIndex();
    _model->revertRow(row);
    _editorWidget->revert();
    _dirty = false;
    _cancelButton->setEnabled(false);
    _saveButton->setEnabled(false);
}

void EditorManagerView::slotContentChanged()
{
    if(!_dirty){
        _cancelButton->setEnabled(true);
        _saveButton->setEnabled(true);
        _dirty = true;
    }
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

