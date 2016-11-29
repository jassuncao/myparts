#include "partpicker.h"
#include "qtreebox.h"
#include <QTableView>
#include <QVBoxLayout>

PartPicker::PartPicker(QWidget *parent) :
    QComboBox(parent),
    _skipNextHide(false)
{
    PartPickerView * pickerView = new PartPickerView(this);
    //setView(pickerView);
    //view()->viewport()->installEventFilter(this);
}

bool PartPicker::eventFilter(QObject* object, QEvent* event)
{
    /*
    if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex index = view()->indexAt(mouseEvent->pos());
        if (!view()->visualRect(index).contains(mouseEvent->pos()))
            _skipNextHide = true;
    }
    */
    return false;
}

void PartPicker::showPopup()
{

    //PartPickerView * pickerView = static_cast<PartPickerView*>(view());
    QComboBox::showPopup();
}

void PartPicker::hidePopup()
{
    if (_skipNextHide) {
            _skipNextHide = false;
    }
    else {
        QComboBox::hidePopup();
    }
}

PartPickerView::PartPickerView(QWidget *parent) :
    QWidget(parent)
{
    _categoryCombo = new QTreeBox(this);
    _partsView = new QTableView(this);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(_categoryCombo);
    layout->addWidget(_partsView);
    setLayout(layout);
}

