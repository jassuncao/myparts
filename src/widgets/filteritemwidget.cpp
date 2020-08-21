#include "filteritemwidget.h"
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QAbstractItemModel>
#include <QFormLayout>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QPushButton>
#include <QStylePainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QWidgetAction>
#include <QDebug>
#include "widgets/treeviewcombobox.h"
#include "core/treemodel/treeitemmodel.h"
#include "part/model/partsquerybuilder.h"

#if defined(Q_WS_X11)
#include <private/qkde_p.h>
#include <private/qt_x11_p.h>
#endif

class MyQComboBox :public QComboBox
{
public:
    explicit MyQComboBox(QWidget * parent);
    void showPopup();
    void hidePopup();
private:
    const QRect popupGeometry(int screen) const;
    QMenu * _menu;
};

MyQComboBox::MyQComboBox(QWidget *parent)
    : QComboBox(parent)
{
    setEditable(false);
    setMaxVisibleItems(1);
    setInsertPolicy(QComboBox::NoInsert);
    _menu = new QMenu(this);

    QWidget * basicEditor = new QFrame(this);
    QFormLayout * formLayout = new QFormLayout(basicEditor);
    basicEditor->setLayout(formLayout);
    QLineEdit * input = new QLineEdit(basicEditor);
    formLayout->addRow("Text", input);

    QWidgetAction * pickerAction = new QWidgetAction(this);
    pickerAction->setDefaultWidget(basicEditor);
    _menu->addAction(pickerAction);
}

//Windows and KDE allows menus to cover the taskbar, while GNOME and Mac don't
const QRect MyQComboBox::popupGeometry(int screen) const
{
#ifdef Q_WS_WIN
    return QApplication::desktop()->screenGeometry(screen);
#elif defined Q_WS_X11
    if (X11->desktopEnvironment == DE_KDE)
        return QApplication::desktop()->screenGeometry(screen);
    else
        return QApplication::desktop()->availableGeometry(screen);
#else
    return QApplication::desktop()->availableGeometry(screen);
#endif
}

void MyQComboBox::showPopup()
{
    QRect desk = popupGeometry(QApplication::desktop()->screenNumber(this));
    QPoint popupPoint = mapToGlobal(QPoint(0, 0));
    const int dateFrameHeight = _menu->sizeHint().height();
    if (popupPoint.y() + height() + dateFrameHeight > desk.bottom()) {
        popupPoint.setY(popupPoint.y() - dateFrameHeight);
     } else {
      popupPoint.setY(popupPoint.y() + height());
      }

     const int dateFrameWidth = _menu->sizeHint().width();
      if (popupPoint.x() + dateFrameWidth > desk.right()) {
        popupPoint.setX(desk.right() - dateFrameWidth);
      }

      if (popupPoint.x() < desk.left()) {
        popupPoint.setX(desk.left());
        }
      if (popupPoint.y() < desk.top()) {
        popupPoint.setY(desk.top());
        }
    _menu->popup(popupPoint);
}

void MyQComboBox::hidePopup()
{
    QComboBox::hidePopup();
}

FilterItemWidget::FilterItemWidget(const int filterTag, bool removableItem, QWidget *parent) :
    QWidget(parent), _filterTag(filterTag), _removableItem(removableItem)
{
}

FilterItemWidget::~FilterItemWidget()
{
}

void FilterItemWidget::reset()
{
    doReset();
}

void FilterItemWidget::doReset()
{
}

void FilterItemWidget::deleteItem()
{
    qDebug("Delete item");
    emit deleteItem(_filterTag);
}

QToolButton * FilterItemWidget::createDeleteButton()
{
    QToolButton * deleteBtn = new QToolButton;
    deleteBtn->setFixedSize(16,16);
    deleteBtn->setStyleSheet("QToolButton { "
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
    deleteBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    deleteBtn->setToolTip(tr("Remove criterion"));
    return deleteBtn;
}

ComboFilterItemWidget::ComboFilterItemWidget(const QString & labelText, const int filterTag, bool removableItem, QWidget *parent)
    : FilterItemWidget(filterTag, removableItem, parent),
       _deleteBtn(0), _valueColumn(-1), _valueRole(Qt::EditRole)
{
    QLabel * label = new QLabel(labelText);
    _comboBox = new QComboBox;
    _comboBox->setMaxVisibleItems(20);
    _comboBox->setStyleSheet("combobox-popup: 0;");
    _comboBox->installEventFilter(this);
    label->setBuddy(_comboBox);



    QHBoxLayout * layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignVCenter);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(label);
    layout->addSpacing(2);
    layout->addWidget(_comboBox);
    if(removableItem){
        _deleteBtn = createDeleteButton();
        _deleteBtn->installEventFilter(this);
        connect(_deleteBtn, SIGNAL(clicked()), this, SLOT(deleteItem()));
        layout->addWidget(_deleteBtn);
    }
    setLayout(layout);
    connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged(int)));
}

ComboFilterItemWidget::~ComboFilterItemWidget()
{
}

void ComboFilterItemWidget::setOptionsModel(QAbstractItemModel * model)
{
    _comboBox->setModel(model);
}

void ComboFilterItemWidget::setDisplayColumn(int displayColumn)
{
    _comboBox->setModelColumn(displayColumn);
}

void ComboFilterItemWidget::setValueColumn(int valueColumn, int role)
{
    _valueColumn = valueColumn;
    _valueRole = role;
}

bool ComboFilterItemWidget::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == _comboBox || obj==_deleteBtn) && event->type() == QEvent::KeyPress ) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Delete && ke->modifiers() == 0) {
            if(_deleteBtn)
                deleteItem();
        }
        else if(ke->key() == Qt::Key_Backspace && ke->modifiers() == 0){
            doReset();
        }
    }
    return FilterItemWidget::eventFilter(obj, event);
}

void ComboFilterItemWidget::doReset()
{
    qDebug("Clear item");
    _comboBox->setCurrentIndex(0);
}

void ComboFilterItemWidget::slotCurrentIndexChanged(int row)
{
    if(_valueColumn<0)
        return;
    QAbstractItemModel * model = _comboBox->model();   
    QModelIndex index = model->index(row, _valueColumn);    
    const QVariant value = model->data(index, _valueRole);
    emit valueChanged(_filterTag, value);
}

TreeComboFilterItemWidget::TreeComboFilterItemWidget(const QString & labelText, const int filterTag, bool removableItem, QWidget *parent)
    : FilterItemWidget(filterTag, removableItem, parent), _deleteBtn(0)
{
    QLabel * label = new QLabel(labelText);
    _comboBox = new TreeViewComboBox;
    _comboBox->setMinimumContentsLength(22);
    _comboBox->setMaxVisibleItems(40);

    _comboBox->installEventFilter(this);
    label->setBuddy(_comboBox);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignVCenter);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(label);
    layout->addSpacing(2);
    layout->addWidget(_comboBox);
    if(removableItem){
        _deleteBtn = createDeleteButton();
        _deleteBtn->installEventFilter(this);
        connect(_deleteBtn, SIGNAL(clicked()), this, SLOT(slotDeleteItem()));
        layout->addWidget(_deleteBtn);
    }
    setLayout(layout);

    connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged(int)));
}

TreeComboFilterItemWidget::~TreeComboFilterItemWidget()
{
}

void TreeComboFilterItemWidget::setOptionsModel(TreeItemModel * model)
{
    _comboBox->setModel(model);
}

void TreeComboFilterItemWidget::slotCurrentIndexChanged(int)
{
    NodeCriterionValue nodeCriterion;//Default is NodeCriterionValue::All
    QModelIndex currIndex = _comboBox->view()->currentIndex();
    //The root node has a valid index but an invalid parent
    //We only care for it's children.
    if(currIndex.isValid() && currIndex.parent().isValid()){
        TreeItemModel * model = static_cast<TreeItemModel*>(_comboBox->model());
        QList<int> nodes = model->getSubTreeIds(currIndex);
        nodeCriterion = NodeCriterionValue(NodeCriterionValue::IncludeNodeChilds, nodes);
    }
    QVariant value = QVariant::fromValue(nodeCriterion);
    emit valueChanged(_filterTag, value);
}

bool TreeComboFilterItemWidget::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == _comboBox || obj==_deleteBtn) && event->type() == QEvent::KeyPress ) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Delete && ke->modifiers() == 0) {
            if(_deleteBtn)
                deleteItem();
        }
        else if(ke->key() == Qt::Key_Backspace && ke->modifiers() == 0){
            doReset();
        }
    }
    return FilterItemWidget::eventFilter(obj, event);
}

void TreeComboFilterItemWidget::doReset()
{
    _comboBox->setCurrentIndex(QModelIndex());
}
