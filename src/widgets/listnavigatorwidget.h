#ifndef LISTNAVIGATORWIDGET_H
#define LISTNAVIGATORWIDGET_H

#include <QWidget>
#include <QModelIndex>

class QListView;
class QSearchLineEdit;
class QAbstractItemModel;

class ListNavigatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ListNavigatorWidget(const QString &title, QWidget *parent = 0);
    ~ListNavigatorWidget();
    QString filterText() const;
    void setModel(QAbstractItemModel *model);
    void setModelColumn(int column);
    void setCurrentIndex(const QModelIndex &index);
    void setCurrentRow(int row);
    QModelIndex currentIndex() const;
signals:
    void filterChanged(const QString & filterText);
    void addNewItem();
    void deleteItem(const QModelIndex & index);
    void itemSelected(const QModelIndex & index);
protected slots:
    void slotFilterChanged();
    void slotRowChanged(const QModelIndex &current, const QModelIndex &);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    QListView * _listView;
    QSearchLineEdit * _searchLineEdit;
};

#endif // LISTNAVIGATORWIDGET_H
