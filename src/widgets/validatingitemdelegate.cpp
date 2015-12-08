#include "validatingitemdelegate.h"
#include <QValidator>
#include <QLineEdit>
#include <QMessageBox>

ValidatingItemDelegate::ValidatingItemDelegate(QValidator * validator, QObject *parent) :
    QStyledItemDelegate(parent), _validator(validator)
{
}


ValidatingItemDelegate::~ValidatingItemDelegate()
{
    if(_validator)
        delete _validator;
}

QWidget * ValidatingItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    if (!index.isValid())
        return 0;
    QLineEdit *le = new QLineEdit(parent);
    le->setFrame(le->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, le));
    le->setValidator(_validator);
    return le;
}

void ValidatingItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit->text().isEmpty()){
        QMessageBox::warning(editor, tr("Error"), tr("Invalid input"),QMessageBox::Ok);
    }
    else{
        model->setData(index,QVariant(lineEdit->text()));
    }

}
