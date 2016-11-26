#include "listnavigatorwidget.h"
#include "qsearchlineedit.h"
#include "styledbar.h"
#include <QHBoxLayout>
#include <QListView>
#include <QLabel>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QKeyEvent>

ListNavigatorWidget::ListNavigatorWidget(const QString & title, QWidget *parent) : QWidget(parent)
{
    QLabel * titleLabel = new QLabel(title);
    Manhattan::StyledBar * titleBar = new Manhattan::StyledBar;

    QHBoxLayout *titleBarLayout = new QHBoxLayout;
    titleBarLayout->addSpacing(4);
    titleBarLayout->setMargin(0);
    titleBarLayout->setSpacing(0);
    titleBarLayout->addWidget(titleLabel);
    titleBar->setLayout(titleBarLayout);

    _searchLineEdit = new QSearchLineEdit;

    _listView = new QListView;
    _listView->setFrameStyle(QFrame::NoFrame);
    _listView->setUniformItemSizes(true);
    _listView->setTextElideMode(Qt::ElideNone);
    _listView->setAttribute(Qt::WA_MacShowFocusRect, false);
    _listView->setEditTriggers(QListView::NoEditTriggers);
    _listView->setSelectionBehavior(QListView::SelectRows);
    _listView->setSelectionMode(QListView::SingleSelection);

    QPushButton * addButton = new QPushButton(tr("Add"));

    QHBoxLayout * actionsLayout = new QHBoxLayout;
    actionsLayout->setMargin(9);
    actionsLayout->setSpacing(8);
    actionsLayout->addStretch();
    actionsLayout->addWidget(addButton);
    actionsLayout->addStretch();


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(_searchLineEdit);
    mainLayout->addWidget(_listView);
    mainLayout->addLayout(actionsLayout);
    setLayout(mainLayout);

    connect(_searchLineEdit, SIGNAL(textClear()), this, SLOT(slotFilterChanged()));
    connect(_searchLineEdit, SIGNAL(returnPressed()), this, SLOT(slotFilterChanged()));
    connect(addButton, SIGNAL(clicked()), this, SIGNAL(addNewItem()));
    _listView->installEventFilter(this);
    _listView->viewport()->installEventFilter(this);
}

ListNavigatorWidget::~ListNavigatorWidget()
{
}

void ListNavigatorWidget::setModel(QAbstractItemModel *model)
{
    _listView->setModel(model);
    connect(_listView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotRowChanged(QModelIndex,QModelIndex)));
}

void ListNavigatorWidget::setModelColumn(int column)
{
   _listView->setModelColumn(column);   
}


void ListNavigatorWidget::setCurrentIndex(const QModelIndex &index)
{
    if(index.isValid()){
        QModelIndex columnIndex = _listView->model()->index(index.row(), _listView->modelColumn());
        _listView->scrollTo(columnIndex);
    }
    _listView->setCurrentIndex(index);
}


void ListNavigatorWidget::setCurrentRow(int row)
{
    if(row<0){
        _listView->setCurrentIndex(QModelIndex());
    }
    /*QSqlTableModel.index returns an invalid QModelIndex if we attempt to create an index for a
     *row outside of the prefetched items. To avoid this we fetch until the desired row is in range
     */
    QAbstractItemModel * model = _listView->model();
    while(row>=model->rowCount() && model->canFetchMore(QModelIndex()))
        model->fetchMore(QModelIndex());

    if(row>=model->rowCount()){
        row = model->rowCount() - 1;
    }
    QModelIndex columnIndex = model->index(row, _listView->modelColumn());
    if(columnIndex.isValid()){
        _listView->scrollTo(columnIndex);
    }
    _listView->setCurrentIndex(columnIndex);
}

QModelIndex ListNavigatorWidget::currentIndex() const
{
    return _listView->currentIndex();
}

void ListNavigatorWidget::slotFilterChanged()
{
    emit filterChanged(_searchLineEdit->text());
}

QString ListNavigatorWidget::filterText() const
{
    return _searchLineEdit->text();
}

void ListNavigatorWidget::slotRowChanged(const QModelIndex &current, const QModelIndex &)
{
    emit itemSelected(current);
}

bool ListNavigatorWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == _listView
        && event->type() == QEvent::KeyPress
        && currentIndex().isValid()) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if ((ke->key() == Qt::Key_Delete
                   || ke->key() == Qt::Key_Backspace)
                && ke->modifiers() == 0) {
            emit deleteItem(currentIndex());
        }
    }
    return QWidget::eventFilter(obj, event);
}

