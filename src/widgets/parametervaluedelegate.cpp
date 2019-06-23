#include "parametervaluedelegate.h"
#include "parametervaluevalidator.h"
#include <QLineEdit>
#include "unitparser.h"
#include <QDebug>
#include <QItemEditorFactory>
#include "valuewithunit.h"
#include "parametervalue.h"
#include <QRegExpValidator>

ParameterValueDelegate::ParameterValueDelegate(QObject * parent) :
    QStyledItemDelegate(parent)
{
}

ParameterValueDelegate::~ParameterValueDelegate()
{
}

QString ParameterValueDelegate::displayText(const QVariant &value, const QLocale &locale) const
{   
    if(value.canConvert<ParameterValue>()){
        ParameterValue aux =  value.value<ParameterValue>();
        qDebug()<<"Display locale "<<locale.name();
        if(!aux.value.isNull()){
            return UnitParser::formatUnit(aux.value.toReal(), aux.symbol, locale);
        }
        else{
            return QString();
        }
    }    
    return QStyledItemDelegate::displayText(value, locale);
}


QWidget * ParameterValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;    
    QVariant v = index.data(Qt::EditRole);
    if(v.canConvert<ParameterValue>()){                
        ParameterValue paramValue = v.value<ParameterValue>();
        QLineEdit *le = new QLineEdit(parent);                
        le->setFrame(le->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, le));
        le->setProperty("unitSymbol", paramValue.symbol);
        const bool emptyAllowed = true;
        le->setValidator(new ParameterValueValidator(paramValue.symbol, emptyAllowed, parent));
        return le;
    }
   return QStyledItemDelegate::createEditor(parent, option, index);
}

void ParameterValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit){
        QVariant unitSymbol = lineEdit->property("unitSymbol");
        if(unitSymbol.isValid()){
            const QString& text = lineEdit->text();
            if(text.isEmpty()){
                model->setData(index,QVariant());
            }
            else{
                bool ok = false;
                double value = UnitParser::parseUnit(text, unitSymbol.toString(), &ok);
                if(ok){
                    model->setData(index,QVariant(value));
                }
                else{
                    //DO something, for example draw a red background
                }
            }
            return;
        }
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}

void ParameterValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant v = index.data(Qt::EditRole);    
    if(v.canConvert<ParameterValue>()){
        QLocale locale;
        ParameterValue paramValue = v.value<ParameterValue>();
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);        
        if(!paramValue.value.isNull()){
            lineEdit->setText(UnitParser::formatUnit(paramValue.value.toReal(), paramValue.symbol, locale));
        }
    }
    else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
