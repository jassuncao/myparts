#ifndef PACKAGEEDITORWIDGET_H
#define PACKAGEEDITORWIDGET_H

#include "core/ui/abstracteditor.h"
#include <QWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QDir>

class QLineEdit;
class QLabel;
class AttachmentsTableView;
class QDialogButtonBox;
class QPushButton;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapper;
class QToolButton;
class AttachmentTableModel;
class ImageViewer;

class PackageEditorWidget : public AbstractEditor
{
    Q_OBJECT
public:
    explicit PackageEditorWidget(QWidget *parent = 0);
    //QModelIndex currentModelIndex() const;
    //void setCurrentModelIndex(const QModelIndex & modelIndex);
    void setModel(QAbstractItemModel * model);
    void setCurrentIndex(int row);
    int currentIndex() const;
    bool validate();
public slots:
    void submit();
    void submitChilds(const QVariant & id);
    void revert();
private slots:
    void slotImageContextMenu(const QPoint &pos);
    void slotImageShow();
    void slotAddImage();
    void slotAddImageRemote();
    void slotRemoveImage();
    void slotAddAttachment();
    void slotRemoveAttachment();
    void slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotAttachmentDoubleClicked(const QModelIndex &index);
private:
    void setImageEditorData(int row);
    void setImageModelData(int row);
    void setPackageImage(const QString & filePath);

    QLineEdit * _nameLineEdit;
    QLineEdit * _descriptionLineEdit;
    QToolButton * _imageButton;
    AttachmentsTableView * _attachmentsTable;
    QPushButton * _removeImageButton;
    QPushButton * _removeAttachmentButton;
    QAbstractItemModel * _model;
    AttachmentTableModel * _attachmentModel;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
    QAction * _actionViewImage;
    QAction * _actionSetImageLocal;
    QAction * _actionSetImageRemote;
    QAction * _actionRemoveImage;
    ImageViewer * _imageViewer;
};

#endif // PACKAGEEDITORWIDGET_H
