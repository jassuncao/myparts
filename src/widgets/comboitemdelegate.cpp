#include "comboitemdelegate.h"
#include "models/modelwithforeignkey.h"
#include <QComboBox>
#include <QDebug>
#include <QLineEdit>

ComboItemDelegate::ComboItemDelegate(QObject *parent) : QStyledItemDelegate(parent),
    _comboModel(0)
{
}

ComboItemDelegate::~ComboItemDelegate()
{
}

void ComboItemDelegate::setComboModel(QAbstractItemModel *model)
{
    if(_comboModel == model)
        return;
    if(_comboModel && _comboModel->QObject::parent()==this)
        delete _comboModel;
    _comboModel = model;
    if(!_comboModel->QObject::parent()){
        _comboModel->setParent(this);
    }
}

QWidget * ComboItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QAbstractItemModel * lookupModel = 0;
    if(_comboModel){
        lookupModel = _comboModel;
    }
    else{
        const IModelWithForeignKey * modelWithForeignKey = dynamic_cast<const IModelWithForeignKey*>(index.model());
        if(modelWithForeignKey){
            lookupModel = modelWithForeignKey->relationModel(index.column());
        }
    }
    if(lookupModel){
        QComboBox *combo = new QComboBox(parent);       
        combo->setEditable(true);
        combo->setInsertPolicy(QComboBox::NoInsert);
        combo->setAutoCompletion(true);
        combo->setModel(lookupModel);        
        combo->installEventFilter(const_cast<ComboItemDelegate *>(this));
        return combo;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void ComboItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (combo) {
        combo->setCurrentIndex(combo->findText(index.data(Qt::DisplayRole).toString()));
    }
    else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ComboItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (combo) {
        QAbstractItemModel * lookupModel = combo->model();
        int currentIndex = combo->findText(combo->lineEdit()->text(), Qt::MatchFixedString);
        QVariant value = lookupModel->index(currentIndex,0).data(IModelWithForeignKey::ForeignKeyRole);
        model->setData(index, value, Qt::EditRole);
    }
    else{
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

