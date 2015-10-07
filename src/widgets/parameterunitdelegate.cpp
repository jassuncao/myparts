#include "parameterunitdelegate.h"
#include "models/partparametertablemodel2.h"
#include <QComboBox>

ParameterUnitDelegate2::ParameterUnitDelegate2(QObject *parent) : QStyledItemDelegate(parent)
{
}

ParameterUnitDelegate2::~ParameterUnitDelegate2()
{
}

QWidget * ParameterUnitDelegate2::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const PartParameterTableModel2 *model = qobject_cast<const PartParameterTableModel2 *>(index.model());
    ComboBoxLookupModel *childModel = model ? model->unitsModel() : 0;
    if (!childModel)
        return QStyledItemDelegate::createEditor(aParent, option, index);

    QComboBox *combo = new QComboBox(aParent);
    combo->setModel(childModel);
    combo->installEventFilter(const_cast<ParameterUnitDelegate2 *>(this));
    return combo;
}

void ParameterUnitDelegate2::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const PartParameterTableModel2 *model = qobject_cast<const PartParameterTableModel2 *>(index.model());
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!model || !combo) {
        QStyledItemDelegate::setEditorData(editor, index);
        return;
    }
    QVariant var = model->data(index, Qt::EditRole);
    int currentIndex = -1;
    if(var.isValid()){
        const ComboBoxLookupModel * childModel = qobject_cast<const ComboBoxLookupModel *>(combo->model());
        currentIndex = childModel->findIdIndex(var.toInt());
    }
    combo->setCurrentIndex(currentIndex);
}

void ParameterUnitDelegate2::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    PartParameterTableModel2 * partParameterModel = qobject_cast<PartParameterTableModel2 *>(model);
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!model || !combo) {
        QStyledItemDelegate::setModelData(editor, model, index);
        return;
    }

    int currentItem = combo->currentIndex();
    if(currentItem>=0){
        const ComboBoxLookupModel * childModel = qobject_cast<const ComboBoxLookupModel *>(combo->model());
        QVariant id = childModel->data(childModel->index(currentItem), Qt::EditRole);
        partParameterModel->setData(index, id, Qt::EditRole);
    }
    else{
        partParameterModel->setData(index, QVariant(), Qt::EditRole);
    }
}

