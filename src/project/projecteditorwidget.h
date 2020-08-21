#ifndef PROJECTEDITORWIDGET_H
#define PROJECTEDITORWIDGET_H

#include "core/ui/abstracteditor.h"
#include <QWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QDir>

class QModelIndex;
class QDataWidgetMapper;
class AttachmentTableModel;
class ProjectPartTableModel;
class AlignmentProxyModel;
class ModelsRepository;

namespace Ui {
    class ProjectEditorForm;
}

class ProjectEditorWidget : public AbstractEditor
{
    Q_OBJECT
public:
    explicit ProjectEditorWidget(ModelsRepository *modelsRepo, QWidget *parent = 0);
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
    void slotAssignPart();
    void slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotCurrentPartRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotAttachmentDoubleClicked(const QModelIndex &index);    
    void slotPartsDoubleCkicked(const QModelIndex &index);
    //void slotPartDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
private:    
    Ui::ProjectEditorForm *ui;   
    ModelsRepository * _modelsRepo;
    QAbstractItemModel * _model;
    AttachmentTableModel * _attachmentModel;
    ProjectPartTableModel * _partsModel;
    AlignmentProxyModel * _partsAlignmentProxyModel;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentIndex;
};

#endif // PROJECTEDITORWIDGET_H
