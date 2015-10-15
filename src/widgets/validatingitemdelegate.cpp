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
