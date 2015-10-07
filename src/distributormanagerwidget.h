#ifndef DISTRIBUTORMANAGERWIDGET_H
#define DISTRIBUTORMANAGERWIDGET_H

#include <QWidget>
#include <QModelIndex>

class QListView;
class QSearchLineEdit;
class QLineEdit;
class QTextEdit;
class QTableView;
class QAction;
class QDialogButtonBox;
class QSqlTableModel;
class DistributorDetailsWidget;

class DistributorManagerWidget : public QWidget
{
    Q_OBJECT
public:    
    explicit DistributorManagerWidget(QWidget *parent = 0);
    ~DistributorManagerWidget();

signals:
private slots:
    void slotAddDistributor();
    void slotDeleteDistributor();
    void slotFilterChanged();
    void slotAccept();
    void slotReject();
    void slotItemActivated(const QModelIndex &index);
private:
    QListView * _distributorsListView;
    QSearchLineEdit *_searchLineEdit;
    QSqlTableModel * _distributorsTableModel;
    DistributorDetailsWidget * _detailsWidget;
};

#endif // DISTRIBUTORMANAGERWIDGET_H
