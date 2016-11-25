#ifndef PROJECTEDITORWIDGET_H
#define PROJECTEDITORWIDGET_H

#include "abstracteditor.h"
#include <QWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QDir>

/*
class QLineEdit;
class QLabel;
class QTableView;
class QDialogButtonBox;
class QPushButton;
class QAbstractItemModel;
class QToolButton;
*/
class QModelIndex;
class QDataWidgetMapper;
class AttachmentTableModel3;
class ProjectPartTableModel;

namespace Ui {
    class ProjectEditorForm;
}

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
    void slotAddPart();
    void slotRemovePart();
    void slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotCurrentPartRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotAttachmentDoubleClicked(const QModelIndex &index);    
private:    
    Ui::ProjectEditorForm *ui;   
    QAbstractItemModel * _model;
    AttachmentTableModel3 * _attachmentModel;
    ProjectPartTableModel * _partsModel;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
};

#endif // PROJECTEDITORWIDGET_H
