#ifndef STORAGEMANAGERWIDGET_H
#define STORAGEMANAGERWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QDialogButtonBox>
#include <QAction>
#include <QModelIndex>

class StorageTreeModel;
class QDataWidgetMapper;
class QSqlQuery;
class QAbstractButton;
class QAction;
class QMenu;

class StorageManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StorageManagerWidget(QWidget *parent = 0);
    ~StorageManagerWidget();
    void setModel(StorageTreeModel * model);
signals:

public slots:
private slots:
    void slotExpandTreeButtonClicked();
    void slotCollapseTreeButtonClicked();
    void slotTreeCurrentChanged(const QModelIndex &current, const QModelIndex &);
    void slotDataChanged();
    void slotButtonBoxClicked(QAbstractButton*button);
    void slotTreeViewCustomContextMenuRequested(const QPoint &pos);
    void slotAddStorage();
    void slotDeleteStorage();
private:
    void createActions();
    void createMenus();
    StorageTreeModel * _storageTreeModel;
    QTreeView * _treeView;
    QLineEdit * _nameLineEdit;
    QTableView * _partsTableView;
    QDialogButtonBox * _buttonBox;    
    QDataWidgetMapper * _mapper;
    QSqlQuery * _partsQuery;
    QAction * _actionAddStorage;
    QAction * _actionDeleteStorage;
    QModelIndex _ctxMenuTreeIndex;
};

#endif // STORAGEMANAGERWIDGET_H
