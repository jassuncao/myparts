#ifndef PARTSTORAGEWIDGET_H
#define PARTSTORAGEWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QItemSelection>
class QPushButton;
class QSqlTableModel;
class QDialogButtonBox;
class QTableView;
class QSqlRecord;

class PartStorageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartStorageWidget(QWidget *parent = 0);
    
signals:
    void dataChanged();
    void dataCommited();
public slots:
    bool submit();
    void revert();
private slots:
    //void dataChanged(const QModelIndex, const QModelIndex);
    void addStorage();
    void deleteStorage();
private:
    QTableView * _view;
    QSqlTableModel * _model;
    QDialogButtonBox *_buttonBox;
    QPushButton * _addButton;
    QPushButton * _removeButton;
    
};

#endif // PARTSTORAGEWIDGET_H
