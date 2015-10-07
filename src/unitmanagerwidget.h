#ifndef UNITMANAGERWIDGET_H
#define UNITMANAGERWIDGET_H

#include <QWidget>
#include <QModelIndex>

class QListView;
class QSearchLineEdit;
class QLineEdit;
class QTextEdit;
class QTableView;
class QAction;
class QGraphicsView;
class QDialogButtonBox;
class QTabBar;
class QSqlTableModel;
class UnitDetailsWidget;
class UnitTableModel;


class UnitManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UnitManagerWidget(QWidget *parent = 0);
    ~UnitManagerWidget();

signals:

public slots:

private slots:
    void slotAddItem();
    void slotDeleteItem();
    void slotFilterChanged();
    void slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void slotEditorAccepted();
    void slotEditorRejected();
private:
    QListView * _listView;
    QSearchLineEdit *_searchLineEdit;
    UnitTableModel * _tableModel;
    UnitDetailsWidget * _detailsWidget;
};

#endif // UNITMANAGERWIDGET_H
