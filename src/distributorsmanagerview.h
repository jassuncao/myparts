#ifndef DISTRIBUTORSMANAGERVIEW_H
#define DISTRIBUTORSMANAGERVIEW_H

#include "minisplitter.h"
#include <QModelIndex>

namespace Manhattan {
class StyledBar;
class MiniSplitter;
}

class ListNavigatorWidget;
class DistributorDetailsWidget;
class QSqlTableModel;
class QStackedLayout;

class DistributorsManagerView : public Manhattan::MiniSplitter
{
    Q_OBJECT
public:
    explicit DistributorsManagerView(QWidget *parent = 0);
    ~DistributorsManagerView();

private slots:
    void slotAddDistributor();
    void slotDeleteDistributor(const QModelIndex &index);
    void slotFilterChanged(const QString & filterText);
    void slotAccept();
    void slotReject();
    void slotItemSelected(const QModelIndex &index);    
private:
    int findRowNumber(QVariant idValue);
    ListNavigatorWidget * _navigatorWidget;
    DistributorDetailsWidget * _detailsWidget;
    QSqlTableModel * _distributorsTableModel;
    QStackedLayout * _contentLayout;
};

#endif // DISTRIBUTORSMANAGERVIEW_H
