#ifndef PARTSMANAGERVIEW_H
#define PARTSMANAGERVIEW_H

#include "minisplitter.h"
#include <QModelIndex>
#include <QSqlRecord>
#include "widgets/navigationsubwidget.h"

namespace Manhattan {
    class StyledBar;
}

class PartsTableView;
class PartDetailsView;
class PartsSqlQueryModel2;
class CategoryTreeModel;
class StorageTreeModel;
class NavigationSubWidget;
class PartsFilterWidget;
class QActionPushButton;
class QPushButton;
class QToolButton;
class FilterBuilder;
class PartsSqlTableModel;
class PartsQueryBuilder;

class PartsManagerView : public Manhattan::MiniSplitter
{
    Q_OBJECT
public:
    explicit PartsManagerView(QWidget *parent = 0);
    ~PartsManagerView();

signals:

public slots:
protected slots:
    void slotPartTableCurrentRowChanged(const QModelIndex &current, const QModelIndex &);    
    void slotAddPart();
    void slotEditPart();
    void slotDeletePart();
    void slotDuplicatePart();
    void slotPartsModelPrimeInsert(int, QSqlRecord &record);
    void slotNavModeChanged(int mode);
    void slotFilterChanged();    
private:
    PartsTableView * createPartsTableView(QAbstractTableModel * tableModel);
    NavigationSubWidget * _navWidget;
    PartsTableView * _partsTableView;
    PartDetailsView * _partDetailsView;
    PartsSqlTableModel * _partsModel;  
    CategoryTreeModel * _categoriesTreeModel;
    StorageTreeModel * _storageTreeModel;
    PartsFilterWidget * _partsFilterWidget;
    QPushButton * _deletePartButton;
    QPushButton * _duplicatePartButton;
    PartsQueryBuilder * _partsQueryBuilder;
};

#endif // PARTSMANAGERVIEW_H
