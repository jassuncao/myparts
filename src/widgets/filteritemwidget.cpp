#include "filteritemwidget.h"
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QAbstractItemModel>

FilterItemWidget::FilterItemWidget(const QString & labelText, bool removableItem, QWidget *parent)
    : QWidget(parent), _deleteBtn(0)
{    
    QLabel * label = new QLabel(labelText);
    _comboBox = new QComboBox;    
    _comboBox->addItem("All");
    _comboBox->addItem("Item 1");
    _comboBox->addItem("Item 2");
    label->setBuddy(_comboBox);

    if(removableItem){
        _deleteBtn = new QToolButton;
        _deleteBtn->setFixedSize(16,16);
        _deleteBtn->setStyleSheet("QToolButton { "
                                   "border: none; "
                                   "padding: 0px; "
                                   "background: url(:/icons/close) center center no-repeat;"
                                   "}"
                                   "QToolButton:hover {"
                                   "background: url(:/icons/close_hover) center center no-repeat;"
                                   "}"
                                   "QToolButton:pressed {"
                                   "background: url(:/icons/close_pressed) center center no-repeat;"
                                   "}");
        _deleteBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        _deleteBtn->setToolTip(tr("Remove criterion"));
        connect(_deleteBtn, SIGNAL(clicked()), this, SLOT(slotDeleteItem()));

        _deleteBtn->installEventFilter(this);
        _comboBox->installEventFilter(this);
    }

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignVCenter);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(label);
    layout->addSpacing(2);
    layout->addWidget(_comboBox);
    if(_deleteBtn)
        layout->addWidget(_deleteBtn);
    setLayout(layout);
}

FilterItemWidget::~FilterItemWidget()
{
}

void FilterItemWidget::setOptionsModel(QAbstractItemModel * model, int visibleColumn)
{
    _comboBox->setModel(model);
    _comboBox->setModelColumn(visibleColumn);
}

bool FilterItemWidget::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == _comboBox || obj==_deleteBtn) && event->type() == QEvent::KeyPress ) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Delete && ke->modifiers() == 0) {
            slotDeleteItem();
        }
    }
    return QWidget::eventFilter(obj, event);
}

void FilterItemWidget::slotDeleteItem()
{
    qDebug("Delete item");
    emit deleteItem();
}

