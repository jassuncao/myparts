#include "partpicker.h"
#include "qtreebox.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDebug>
#include "widgets/partsfilterwidget.h"
#include "widgets/partstableview.h"
#include "models/modelsrepository.h"
#include "part/model/partssqltablemodel.h"
#include "part/model/partsquerybuilder.h"
#include "widgets/filteritemwidget.h"
#include "widgets/treeviewcombobox.h"
#include "part/model/categorytreemodel.h"

PartPickerDialog::PartPickerDialog(ModelsRepository *modelsRepo, QWidget *parent) :
    QDialog(parent),
    _partsQueryBuilder(new PartsQueryBuilder)
{
    setWindowIcon(QIcon(":/icons/partLink"));
    setWindowTitle("Select part");
    _partsFilterWidget = new PartsFilterWidget(modelsRepo, this);
    _partsFilterWidget->setPartsQueryBuilder(_partsQueryBuilder);
    _partsModels = new PartsSqlTableModel(_partsQueryBuilder, this);

    TreeComboFilterItemWidget * categoryFilterItem  = new TreeComboFilterItemWidget(tr("Category:"), PartsQueryBuilder::FilterByCategory, false, this);
    categoryFilterItem->setOptionsModel(modelsRepo->partCategoryModel());

    _partsFilterWidget->insertFixedFilterItemWidget(categoryFilterItem);
    _partsTable = new PartsTableView(this);
    _partsTable->setModel(_partsModels);
    _partsTable->setSelectionMode(PartsTableView::SingleSelection);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(_partsFilterWidget);
    layout->addWidget(_partsTable);
    layout->addWidget(buttonBox);
    setLayout(layout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(_partsFilterWidget, SIGNAL(filterChanged()), _partsModels, SLOT(filterChanged()));
}

PartPickerDialog::~PartPickerDialog()
{
    delete _partsQueryBuilder;
}

void PartPickerDialog::accept()
{
    QModelIndex index = _partsTable->currentIndex();
    if(index.isValid()){
        _selectedPartId = _partsModels->index(index.row(), PartsSqlTableModel::ColumnId).data(Qt::EditRole);
        _selectedPartName = _partsModels->index(index.row(), PartsSqlTableModel::ColumnName).data(Qt::EditRole);
    }
    else{
        _selectedPartId = QVariant();
        _selectedPartName = QVariant();
    }
    QDialog::accept();
}
