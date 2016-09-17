#ifndef PACKAGEMANAGERWIDGET_H
#define PACKAGEMANAGERWIDGET_H

#include <QWidget>
#include <QModelIndex>

class QListView;
class QSearchLineEdit;
class QDialogButtonBox;
class QSqlTableModel;
class PackageDetailsWidget;


class PackageManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PackageManagerWidget(QWidget *parent = 0);
    ~PackageManagerWidget();

signals:    

public slots:
private slots:    
    void slotAddPackage();
    void slotDeletePackage();
    void slotFilterChanged();
    void slotAddImage();
    void slotDeleteImage();   
    void slotAccept();
    void slotReject();    
    void slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &);
private:
    QListView * _packagesListView;
    QSearchLineEdit *_searchLineEdit;
    QSqlTableModel * _packagesTableModel;
    PackageDetailsWidget * _detailsWidget;
};

#endif // PACKAGEMANAGERWIDGET_H
