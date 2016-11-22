#ifndef PROJECTEDITORWIDGET_H
#define PROJECTEDITORWIDGET_H

#include "abstracteditor.h"
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

class ProjectEditorWidget : public AbstractEditor
{
    Q_OBJECT
public:
    explicit ProjectEditorWidget(QWidget *parent = 0);
    void setModel(QAbstractItemModel * model);
    void setCurrentIndex(int row);
    int currentIndex() const;
    bool validate();
public slots:
    void submit();
    void submitChilds(const QVariant & id);
    void revert();
private slots:
    void slotAddAttachment();
    void slotRemoveAttachment();
    void slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotAttachmentDoubleClicked(const QModelIndex &index);
private:
    QLineEdit * _nameLineEdit;
    QLineEdit * _descriptionLineEdit;
    QLineEdit * _remarkLineEdit;
    QTableView * _attachmentsTable;
    QPushButton * _removeImageButton;
    QPushButton * _removeAttachmentButton;
    QAbstractItemModel * _model;
    AttachmentTableModel3 * _attachmentModel;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
};

#endif // PROJECTEDITORWIDGET_H
