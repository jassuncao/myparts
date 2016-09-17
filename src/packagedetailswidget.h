#ifndef PACKAGEDETAILSWIDGET_H
#define PACKAGEDETAILSWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QDir>

class QLineEdit;
class QTableView;
class QDialogButtonBox;
class QPushButton;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapper;
class QToolButton;
class AttachmentTableModel3;

class PackageDetailsWidget : public QWidget
{
    Q_OBJECT
public:   
    explicit PackageDetailsWidget(QWidget *parent = 0);
    ~PackageDetailsWidget();
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
    void slotImageShow();
    void slotAddImage();
    void slotAddImageRemote();
    void slotRemoveImage();
    void slotAddAttachment();
    void slotRemoveAttachment();
    void slotImageContextMenu(const QPoint &pos);
    void slotSetDirty();
    void slotResetDirty();
    void slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotAttachmentDoubleClicked(const QModelIndex &index);
private:
    bool _dirty;
    bool _newRecord;
    QLineEdit * _nameLineEdit;
    QLineEdit * _descriptionLineEdit;
    QToolButton * _imageButton;
    QTableView * _attachmentsTable;
    QDialogButtonBox* _buttonBox;
    QPushButton * _removeImageButton;
    QPushButton * _removeAttachmentButton;
    QAbstractItemModel * _model;
    AttachmentTableModel3 * _attachmentModel;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
    QAction * _actionViewImage;
    QAction * _actionSetImageLocal;
    QAction * _actionSetImageRemote;
    QAction * _actionRemoveImage;   

    bool setPackageImage(const QString & fileName);
    //QString copyFileToDir(const QString &file, const QDir & dir);
    //bool doAddAttachment(QString filePath);
};

class PackageDetailsDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
     PackageDetailsDelegate(QObject * parent = 0);
     virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
     virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};



#endif // PACKAGEDETAILSWIDGET_H
