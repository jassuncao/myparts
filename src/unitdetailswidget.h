#ifndef UNITDETAILSWIDGET_H
#define UNITDETAILSWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QSqlRecord>
#include <QStyledItemDelegate>

class QLineEdit;
class QLabel;
class QTableView;
class QDialogButtonBox;
class QPushButton;
class QSqlTableModel;
class QModelIndex;
class QDataWidgetMapper;
class QToolButton;

class UnitDetailsWidget : public QWidget
{    
    Q_OBJECT
public:
    explicit UnitDetailsWidget(QWidget *parent = 0);
    ~UnitDetailsWidget();
    QModelIndex currentModelIndex() const;
    void setCurrentModelIndex(const QModelIndex & modelIndex, bool newRecord=false);
    void setModel(QAbstractItemModel *model);
    inline bool isDirty(){ return _dirty;}
    inline bool isNew(){ return _newRecord;}
signals:
    void accepted();
    void rejected();
public slots:
    void slotAccept();
    void slotReject();
private slots:
    void slotSetDirty();
    void slotResetDirty();
    void slotToggleCheckboxes(bool checked);
private:    
    bool _dirty;
    bool _newRecord;
    QLineEdit * _nameLineEdit;
    QLineEdit * _symbolLineEdit;
    QTableView * _allowedPrefixesTable;
    QDialogButtonBox* _buttonBox;
    QAbstractItemModel * _model;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
    QStandardItemModel * _allowedPrefixesTableModel;
    void initPrefixesTableModel();  
    bool eventFilter(QObject *obj, QEvent *event);
    void togglePrefix();
};

class UnitDetailsDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
     UnitDetailsDelegate(QObject * parent = 0);
     virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
     virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

#endif // UNITDETAILSWIDGET_H
