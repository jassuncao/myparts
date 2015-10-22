#include "itemstreeview.h"
#include <QHeaderView>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QKeyEvent>


OpenDocumentsDelegate::OpenDocumentsDelegate(QObject *parent)
 : QStyledItemDelegate(parent),
   closeButtonVisible(true)
{
}

void OpenDocumentsDelegate::setCloseButtonVisible(bool visible)
{
    closeButtonVisible = visible;
}

void OpenDocumentsDelegate::handlePressed(const QModelIndex &index)
{
    if (index.column() == 1)
        pressedIndex = index;
}

void OpenDocumentsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_MouseOver) {
        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
            pressedIndex = QModelIndex();
        QBrush brush = option.palette.alternateBase();
        if (index == pressedIndex)
            brush = option.palette.dark();
        painter->fillRect(option.rect, brush);
    }


    QStyledItemDelegate::paint(painter, option, index);

    if (closeButtonVisible && index.column() == 1 && option.state & QStyle::State_MouseOver) {
        const QIcon icon(QLatin1String((option.state & QStyle::State_Selected) ?
                                       ":/icons/delete" : ":/icons/delete"));

        QRect iconRect(option.rect.right() - option.rect.height(),
                       option.rect.top(),
                       option.rect.height(),
                       option.rect.height());

        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
    }

}


ItemsTreeView::ItemsTreeView(QWidget *parent) : QTreeView(parent)
{

    m_delegate = new OpenDocumentsDelegate(this);
    setItemDelegate(m_delegate);
    setIndentation(0);
    setUniformRowHeights(true);
    setTextElideMode(Qt::ElideMiddle);
    setFrameStyle(QFrame::NoFrame);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    viewport()->setAttribute(Qt::WA_Hover);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    //setActivationMode(Utils::SingleClickActivation);

    installEventFilter(this);
    viewport()->installEventFilter(this);

    connect(this, SIGNAL(pressed(QModelIndex)), m_delegate, SLOT(handlePressed(QModelIndex)));
}

ItemsTreeView::~ItemsTreeView()
{
}

void ItemsTreeView::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);
    header()->hide();
    header()->setStretchLastSection(false);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setSectionResizeMode(1, QHeaderView::Fixed);    
#else
    header()->setResizeMode(0,QHeaderView::Stretch);
    header()->setResizeMode(1,QHeaderView::Fixed);
#endif
    header()->resizeSection(1, 16);
}

bool ItemsTreeView::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this && event->type() == QEvent::KeyPress
            && currentIndex().isValid()) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if ((ke->key() == Qt::Key_Delete
                   || ke->key() == Qt::Key_Backspace)
                && ke->modifiers() == 0) {
            emit closeActivated(currentIndex());
        }
    } else if (obj == viewport()
             && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::MiddleButton
                && me->modifiers() == Qt::NoModifier) {
            QModelIndex index = indexAt(me->pos());
            if (index.isValid()) {
                emit closeActivated(index);
                return true;
            }
        }
    }
    return false;
}


