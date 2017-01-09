#include "partpicker.h"
#include "qtreebox.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include "widgets/partsfilterwidget.h"
#include "widgets/partstableview.h"
#include "models/modelsrepository.h"
#include "models/partssqltablemodel.h"
#include "models/partsquerybuilder.h"
#include "widgets/filteritemwidget.h"
#include "widgets/treeviewcombobox.h"
#include "models/categorytreemodel.h"

PartPicker::PartPicker(QWidget *parent) :
    QComboBox(parent),
    _skipNextHide(false)
{
    //PartPickerView * pickerView = new PartPickerView(this);
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

PartPickerView::PartPickerView(ModelsRepository * modelsRepo, QWidget *parent) :
    QWidget(parent)
{  
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsFilterWidget = new PartsFilterWidget(modelsRepo, this);
    _partsFilterWidget->setPartsQueryBuilder(_partsQueryBuilder);

    _partsModels = new PartsSqlTableModel(_partsQueryBuilder, this);

    TreeComboFilterItemWidget * categoryFilterItem  = new TreeComboFilterItemWidget(tr("Category:"), PartsQueryBuilder::FilterByCategory, false, this);
    categoryFilterItem->setOptionsModel(modelsRepo->partCategoryModel());

    _partsFilterWidget->insertFixedFilterItemWidget(categoryFilterItem);
    _partsTable = new PartsTableView(this);
    _partsTable->setModel(_partsModels);
    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(_partsFilterWidget);
    layout->addWidget(_partsTable);
    layout->addWidget(buttons);
    setLayout(layout);

    connect(_partsFilterWidget, SIGNAL(filterChanged()), this, SLOT(slotFilterChanged()));
}

PartPickerView::~PartPickerView()
{
    delete _partsQueryBuilder;
}

void PartPickerView::slotFilterChanged()
{
    qDebug()<<"PartPickerView::slotFilterChanged";
    _partsModels->select();
}

