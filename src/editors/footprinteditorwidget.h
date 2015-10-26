#ifndef FOOTPRINTEDITORWIDGET_H
#define FOOTPRINTEDITORWIDGET_H

#include <QWidget>
#include <QWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QDir>

class QLineEdit;
class QLabel;
class QTableView;
class QDialogButtonBox;
class QPushButton;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapper;
class QToolButton;
class AttachmentTableModel3;

class FootprintEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FootprintEditorWidget(QWidget *parent = 0);
    QModelIndex currentModelIndex() const;
    void setCurrentModelIndex(const QModelIndex & modelIndex);
    void setModel(QAbstractItemModel * model);
    bool validate();
signals:
    void contentChanged();
public slots:
    void submit();
    void submitChilds(const QVariant & id);
    void revert();
private slots:
    void slotContentChanged();
    void slotImageContextMenu(const QPoint &pos);
private:
    //bool setFootprintImage(const QString & fileName);
    void setImageEditorData(int row);
    void setImageModelData(int row);

    QLineEdit * _nameLineEdit;
    QLineEdit * _descriptionLineEdit;
    QToolButton * _imageButton;
    QTableView * _attachmentsTable;
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
};

#endif // FOOTPRINTEDITORWIDGET_H
