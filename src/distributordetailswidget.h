#ifndef DISTRIBUTORDETAILSWIDGET_H
#define DISTRIBUTORDETAILSWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QDir>

class QLineEdit;
class QTextEdit;
class QDialogButtonBox;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapper;
class QPushButton;

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
    inline bool isNew(){ return _newRecord;}
signals:
    void accepted();
    void rejected();
    void deleted(const QModelIndex&);
public slots:
    void slotAccept();
    void slotReject();
protected slots:
    void slotSetDirty();
    /*
    void slotResetDirty();
    */
    void slotDelete();
private:
    bool _newRecord;
    QLineEdit * _nameLineEdit;
    QLineEdit * _websiteLineEdit;
    QTextEdit * _commentTextEdit;
    QDialogButtonBox* _buttonBox;
    QAbstractItemModel * _model;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
    QPushButton * _saveButton;
    QPushButton * _deleteButton;
    QPushButton * _cancelButton;
};

#endif // DISTRIBUTORDETAILSWIDGET_H
