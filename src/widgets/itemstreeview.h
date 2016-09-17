#ifndef ITEMSTREEVIEW_H
#define ITEMSTREEVIEW_H

#include <QTreeView>
#include <QStyledItemDelegate>

class OpenDocumentsDelegate;

class ItemsTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit ItemsTreeView(QWidget *parent = 0);
    ~ItemsTreeView();

    void setModel(QAbstractItemModel *model);

signals:
    void closeActivated(const QModelIndex &index);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    OpenDocumentsDelegate *m_delegate;
};

class OpenDocumentsDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit OpenDocumentsDelegate(QObject *parent = 0);

    void setCloseButtonVisible(bool visible);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    mutable QModelIndex pressedIndex;
    bool closeButtonVisible;
public slots:
    void handlePressed(const QModelIndex &index);
};

#endif // ITEMSTREEVIEW_H
