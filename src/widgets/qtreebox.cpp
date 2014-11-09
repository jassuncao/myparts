#include "qtreebox.h"
#include <QTreeView>
#include <QEvent>
#include <QMouseEvent>
#include <QModelIndex>

QTreeBox::QTreeBox(QWidget *parent) :
    QComboBox(parent),
    _skipNextHide(false)
{
    QTreeView * treeView = new QTreeView(this);
    setView(treeView);
    view()->viewport()->installEventFilter(this);
    //QComboBox::resize(200,30);
}

bool QTreeBox::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex index = view()->indexAt(mouseEvent->pos());
        if (!view()->visualRect(index).contains(mouseEvent->pos()))
            _skipNextHide = true;
    }
    return false;
}

void QTreeBox::showPopup()
{

    QTreeView * treeView = static_cast<QTreeView*>(view());
    treeView->expandAll();
    QModelIndex selected = model()->index(currentIndex(),0,rootModelIndex());
    qDebug("Curre idx: %d", currentIndex());
    qDebug("Curre root: %d",rootModelIndex().row());
    treeView->setCurrentIndex(selected);
    //setRootModelIndex(static_cast<QDirModel*>(model())->index(QDir::rootPath()));
    setRootModelIndex(QModelIndex());
    QComboBox::showPopup();
}

void QTreeBox::hidePopup()
{
    setRootModelIndex(view()->currentIndex().parent());
    setCurrentIndex(view()->currentIndex().row());
    if (_skipNextHide) {
            _skipNextHide = false;
    }
    else {
        QComboBox::hidePopup();
    }
}
