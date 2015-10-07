#ifndef DISTRIBUTORDETAILSWIDGET_H
#define DISTRIBUTORDETAILSWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QDir>

class QLineEdit;
class QTextEdit;
class QLabel;
class QTableView;
class QDialogButtonBox;
class QPushButton;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapper;
class QToolButton;

class DistributorDetailsWidget : public QWidget
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnWebsite,
        ColumnComment
    };

    explicit DistributorDetailsWidget(QWidget *parent = 0);
    ~DistributorDetailsWidget();
    QModelIndex currentModelIndex() const;
    void setCurrentModelIndex(const QModelIndex & modelIndex, bool newRecord=false);
    void setModel(QAbstractItemModel * model);
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
private:
    bool _dirty;
    bool _newRecord;
    QLineEdit * _nameLineEdit;
    QLineEdit * _websiteLineEdit;
    QTextEdit * _commentTextEdit;
    QDialogButtonBox* _buttonBox;
    QAbstractItemModel * _model;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
};

#endif // DISTRIBUTORDETAILSWIDGET_H
