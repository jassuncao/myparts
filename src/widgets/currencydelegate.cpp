#include "currencydelegate.h"
#include "constants.h"
#include <QSettings>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QBoxLayout>
#include "price.h"
#include "currencyhelper.h"

CurrencyDelegate::CurrencyDelegate(QObject *parent) :
    QStyledItemDelegate(parent), _horizontalAlignment(Qt::AlignRight)
{
    QSettings settings;
    _defaultCurrency = settings.value(CURRENCY_CODE_KEY).toString();    
    if(_defaultCurrency.isEmpty()){
        _defaultCurrency = CurrencyHelper::currencies(CurrencyHelper::MostCommon).first();
    }    
}

CurrencyDelegate::~CurrencyDelegate()
{
}

QWidget * CurrencyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
    if (!index.isValid()) return 0;

    QStringList isoCodes = CurrencyHelper::currencies(CurrencyHelper::MostCommon);
    QComboBox* combo = new QComboBox(parent);
    combo->addItems(isoCodes);
    combo->setEditable(true);
    combo->setInsertPolicy(QComboBox::InsertAtTop);
    combo->setDuplicatesEnabled(false);
    return combo;
}

void CurrencyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (QComboBox* combo = qobject_cast<QComboBox*>(editor)) {
        int comboIndex = -1;
        QVariant var = index.data(Qt::EditRole);
        if(var.isValid() && var.canConvert<QString>()){
            QString value = var.toString();
            comboIndex = combo->findText(value);
            if( comboIndex < 0) {
                comboIndex = 0;
                combo->insertItem(comboIndex, value);
            }
        }
        combo->setCurrentIndex(comboIndex);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void CurrencyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (QComboBox* combo = qobject_cast<QComboBox*>(editor)){
        model->setData(index, combo->currentText(), Qt::EditRole);
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void CurrencyDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = option->displayAlignment | _horizontalAlignment;
}


