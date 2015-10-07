#include "storagemanagerwidget.h"
#include "models/storagetreemodel.h"
#include "entities/storagedao.h"
#include <QToolBar>
#include <QBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QDataWidgetMapper>
#include <QPushButton>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QHeaderView>
#include <QMenu>


static QWidget * createHeader(const QString & title){
    QString headerStyle("QWidget {background-color:qlineargradient(spread:pad, x1:0.5, y1:0.8, x2:0.5, y2:0, stop:0.248619 rgba(67, 67, 67, 255), stop:0.707182 rgba(114, 114, 114, 255));}");
    QWidget * header1 = new QWidget;
    header1->setStyleSheet(headerStyle);

    QHBoxLayout * hBoxLayout = new QHBoxLayout;
    QLabel * label = new QLabel(title);
    label->setMinimumHeight(20);
    label->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
    hBoxLayout->addWidget(label);
    header1->setLayout(hBoxLayout);
    return header1;
}

StorageManagerWidget::StorageManagerWidget(QWidget *parent)
    : QWidget(parent)
{
    QToolBar * toolbar = new QToolBar;
    toolbar->setIconSize(QSize(16,16));
    toolbar->addAction(QIcon(":/icons/toggle_expand"), tr("Expand"), this, SLOT(slotExpandTreeButtonClicked()));
    toolbar->addAction(QIcon(":/icons/toggle_collapse"), tr("Collapse"), this, SLOT(slotCollapseTreeButtonClicked()));   

    _treeView = new QTreeView;
    _treeView->header()->setVisible(false);
    _treeView->setDragDropMode(QAbstractItemView::DragDrop);
    _treeView->setDefaultDropAction(Qt::MoveAction);    
    _treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView->setEditTriggers(QTreeView::NoEditTriggers);


    QVBoxLayout *westBoxLayout = new QVBoxLayout;
    westBoxLayout->setMargin(0);
    westBoxLayout->addWidget(createHeader(tr("Storage")));
    westBoxLayout->addWidget(toolbar);
    westBoxLayout->addWidget(_treeView);

    _nameLineEdit = new QLineEdit;
    _partsTableView = new QTableView;
    _partsTableView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    _partsTableView->setSelectionBehavior(QTableView::SelectRows);
    _partsTableView->setSelectionMode(QTableView::ExtendedSelection);
    _partsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _partsTableView->verticalHeader()->setVisible(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    _partsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    _partsTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    _partsTableView->setDragEnabled(true);
    _partsTableView->setDragDropMode(QAbstractItemView::DragOnly);
    _partsTableView->setDefaultDropAction(Qt::LinkAction);

    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), _nameLineEdit);
    formLayout->addRow(tr("Contained Parts:"), _partsTableView);

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
    //connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    //connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


    QWidget * detailsContainer = new QWidget;
    QVBoxLayout *detailsLayout = new QVBoxLayout;
    detailsLayout->addLayout(formLayout);
    detailsLayout->addWidget(_buttonBox);
    detailsContainer->setLayout(detailsLayout);

    QVBoxLayout *eastBoxLayout = new QVBoxLayout;
    eastBoxLayout->setMargin(0);
    eastBoxLayout->addWidget(createHeader(tr("Details")));
    eastBoxLayout->addWidget(detailsContainer);


    QFrame * eastWidget = new QFrame;
    eastWidget->setFrameShape(QFrame::Box);
    eastWidget->setFrameShadow(QFrame::Plain);
    eastWidget->setLayout(eastBoxLayout);

    QFrame * westWidget = new QFrame;
    westWidget->setFrameShape(QFrame::Box);
    westWidget->setFrameShadow(QFrame::Plain);
    westWidget->setLayout(westBoxLayout);


    QSplitter * hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(westWidget);
    hSplitter->addWidget(eastWidget);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(hSplitter);
    setLayout(layout);

    _mapper = new QDataWidgetMapper(this);
    connect(_nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotDataChanged()));
    connect(_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slotButtonBoxClicked(QAbstractButton*)));
    connect(_treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotTreeViewCustomContextMenuRequested(QPoint)));    

    createActions();
    createMenus();
}

StorageManagerWidget::~StorageManagerWidget()
{

}

void StorageManagerWidget::createActions()
{
    _actionAddStorage = new QAction(QIcon(":/icons/box_add"),tr("Add Storage"),this);
    _actionDeleteStorage = new QAction(QIcon(":/icons/box_delete"),tr("Delete Storage"),this);
    connect(_actionAddStorage,SIGNAL(triggered()), this, SLOT(slotAddStorage()));
    connect(_actionDeleteStorage,SIGNAL(triggered()), this, SLOT(slotDeleteStorage()));
}

void StorageManagerWidget::createMenus()
{   
}


void StorageManagerWidget::setModel(StorageTreeModel * model)
{
    _storageTreeModel = model;
    QItemSelectionModel *m = _treeView->selectionModel();
    _treeView->setModel(model);
    if(m) delete m;

    //Setup the mapper
    _mapper->setModel(model);
    _mapper->addMapping(_nameLineEdit, StorageDAO::NAME_COL);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    connect(_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotTreeCurrentChanged(QModelIndex,QModelIndex)));    
}

void StorageManagerWidget::slotExpandTreeButtonClicked()
{
    _treeView->expandAll();
}

void StorageManagerWidget::slotCollapseTreeButtonClicked()
{
    _treeView->expandToDepth(0);
}

void StorageManagerWidget::slotTreeCurrentChanged(const QModelIndex &current, const QModelIndex &)
{
    if(!current.isValid()) return;
    _storageTreeModel->getItemId(current);
    _mapper->setRootIndex(_storageTreeModel->parent(current));
    _mapper->setCurrentModelIndex(current);
    _buttonBox->setEnabled(false);    

    //Show all parts by default
    QString query("SELECT p.name, s.name, p.actualStock FROM part p "
                  "LEFT JOIN part_storage s "
                  "ON p.storage=s.id");
    int selectedStorageId = _storageTreeModel->getItemId(current);
    //If the user selected a node different from the root we filter the results
    if(selectedStorageId != _storageTreeModel->rootItemId()){
        StorageDAO dao;
        QVector<QVariant> childs = dao.listChildStorage(selectedStorageId);
        query+=QLatin1String(" WHERE p.storage IN (");
        query+=childs.at(0).toString();

        for(int i=1; i<childs.size();++i){
            query+=QLatin1String(",");
            query+=childs.at(i).toString();
        }
        query+=QLatin1Char(')');
    }

    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Storage"));
    model->setHeaderData(2, Qt::Horizontal, tr("Quantity"));
    _partsTableView->setModel(model);

}

void StorageManagerWidget::slotDataChanged()
{
    _buttonBox->setEnabled(true);
}

void StorageManagerWidget::slotButtonBoxClicked(QAbstractButton* button)
{
    QDialogButtonBox::ButtonRole role = _buttonBox->buttonRole(button);
    switch(role){
        case QDialogButtonBox::ApplyRole:
        _mapper->submit();
        break;
    case QDialogButtonBox::RejectRole:
        _storageTreeModel->revert();
        _mapper->revert();
        break;
    default:
        break;
    }
    _buttonBox->setEnabled(false);
}

void StorageManagerWidget::slotTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    qDebug()<<"slotTreeViewCustomContextMenuRequested";
    _ctxMenuTreeIndex =_treeView->indexAt(pos);
    bool canDelete = false;
    if(_ctxMenuTreeIndex.isValid()){
        canDelete = !_storageTreeModel->hasChildren(_ctxMenuTreeIndex);
    }
    _actionDeleteStorage->setEnabled(canDelete);
    QMenu menu(this);
    menu.addAction(_actionAddStorage);
    menu.addAction(_actionDeleteStorage);
    menu.exec(_treeView->mapToGlobal(pos));
}

void StorageManagerWidget::slotAddStorage()
{
    const QModelIndex & parentIndex = _ctxMenuTreeIndex;
    if(!_storageTreeModel->insertItem(parentIndex))
        return;
    int row = _storageTreeModel->rowCount(parentIndex) - 1;
    _mapper->setRootIndex(parentIndex);
    _mapper->setCurrentIndex(row);
    _buttonBox->setEnabled(true);
    _nameLineEdit->setFocus();  
}

void StorageManagerWidget::slotDeleteStorage()
{
    if(!_ctxMenuTreeIndex.isValid())
        return;
    _storageTreeModel->removeItem(_ctxMenuTreeIndex);
}



