#ifndef FOOTPRINTMANAGERWIDGET_H
#define FOOTPRINTMANAGERWIDGET_H

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
class QSqlTableModel;
class FootprintDetailsWidget;
class FootprintAttachmentTableModel;


class FootprintManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FootprintManagerWidget(QWidget *parent = 0);
    ~FootprintManagerWidget();

signals:    

public slots:
private slots:    
    void slotAddFootprint();
    void slotDeleteFootprint();
    void slotFilterChanged();
    void slotAddImage();
    void slotDeleteImage();   
    void slotAccept();
    void slotReject();    
    void slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &);
private:
    QListView * _footprintsListView;
    QSearchLineEdit *_searchLineEdit;
    QSqlTableModel * _footprintsTableModel;
    FootprintDetailsWidget * _detailsWidget;
};

#endif // FOOTPRINTMANAGERWIDGET_H
