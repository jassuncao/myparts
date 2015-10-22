#include "distributorsmanagerview.h"
#include "fancyactionbar.h"

#include <QHBoxLayout>
#include <QListView>
#include "widgets/qsearchlineedit.h"
#include "styledbar.h"
#include "minisplitter.h"
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include "distributordetailswidget.h"
#include "widgets/itemstreeview.h"
#include <QStringListModel>
#include <QStandardItemModel>

DistributorsManagerView::DistributorsManagerView(QWidget *parent) : Manhattan::MiniSplitter(parent)
{

    _toolBar = new Manhattan::StyledBar(this);

    QHBoxLayout *toolBarLayout = new QHBoxLayout;
    toolBarLayout->addSpacing(4);
    toolBarLayout->setMargin(0);
    toolBarLayout->setSpacing(0);
    _toolBar->setLayout(toolBarLayout);
    QLabel * title = new QLabel("Distributors");
    toolBarLayout->addWidget(title);
    /*
    toolBarLayout->addStretch();
    QToolButton * button3 = new QToolButton();
    button3->setIcon(QIcon(QLatin1String("/hd/Work/IconCollections/famfamfam_silk_icons_v013/icons/add.png")));
    toolBarLayout->addWidget(button3);

    QToolButton * button4 = new QToolButton();
    button4->setIcon(QIcon(QLatin1String("/hd/Work/IconCollections/famfamfam_silk_icons_v013/icons/delete.png")));
    toolBarLayout->addWidget(button4);
*/

    QVBoxLayout *lay = new QVBoxLayout();
    lay->setMargin(0);
    lay->setSpacing(0);
    lay->addWidget(_toolBar);

    QListView * listView = new QListView;
    //ItemsTreeView * listVew = new ItemsTreeView;

    listView->setFrameStyle(QFrame::NoFrame);
    listView->setUniformItemSizes(true);
    listView->setTextElideMode(Qt::ElideNone);
    listView->setAttribute(Qt::WA_MacShowFocusRect, false);
    QStandardItemModel * model = new QStandardItemModel(4, 2);
     for (int row = 0; row < 4; ++row) {
         QStandardItem *item = new QStandardItem(QString("row %0").arg(row));
         model->setItem(row, 0, item);
     }
    listView->setModel(model);

    /*
    QToolBar * toolbar = new QToolBar;
    toolbar->setIconSize(QSize(16,16));

    QSearchLineEdit * _searchLineEdit = new QSearchLineEdit;
    toolbar->addWidget(_searchLineEdit);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/distributor_add"),tr("Add Distributor"), this, SLOT(slotAddDistributor()));
    toolbar->addAction(QIcon(":/icons/distributor_delete"),tr("Delete Distributor"), this, SLOT(slotDeleteDistributor()));
*/


    /*
    QToolButton * button1 = new QToolButton();
    button1->setIcon(QIcon(QLatin1String(":icons/distributor_add")));

    QToolButton * button2 = new QToolButton();
    button2->setIcon(QIcon(QLatin1String(":icons/distributor_delete")));

    QHBoxLayout * actionsLayout = new QHBoxLayout;
    QSearchLineEdit * search = new QSearchLineEdit;
    actionsLayout->addWidget(search);
    actionsLayout->addSpacing(16);
    actionsLayout->addWidget(button1);
    actionsLayout->addWidget(button2);
    actionsLayout->addSpacing(4);
    lay->addLayout(actionsLayout);
*/
    //lay->addWidget(toolbar);



    QHBoxLayout * actionsLayout = new QHBoxLayout;
    actionsLayout->setMargin(6);
    actionsLayout->setSpacing(8);
    actionsLayout->addStretch();
    actionsLayout->addWidget(new QPushButton("Add"));
    //actionsLayout->addWidget(new QPushButton("Delete"));
    actionsLayout->addStretch();

    QSearchLineEdit * search = new QSearchLineEdit;
    lay->addWidget(search);
    lay->addWidget(listView);
    lay->addLayout(actionsLayout);

    DistributorDetailsWidget * contentWidget = new  DistributorDetailsWidget();

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setMargin(0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(new Manhattan::StyledBar);
    /*
    contentLayout->addLayout(actionsLayout);
    QFrame * hLine = new QFrame;
    hLine->setFrameShape(QFrame::HLine);
    contentLayout->addWidget(hLine);
    */
    contentLayout->addWidget(contentWidget);

    QWidget *leftSplitWidget = new QWidget;
    leftSplitWidget->setLayout(lay);

    QWidget * rightSplitWidget = new QWidget;
    rightSplitWidget->setLayout(contentLayout);

    insertWidget(0,  leftSplitWidget);
    insertWidget(1,  rightSplitWidget);
    setStretchFactor(0, 0);
    setStretchFactor(1, 1);

}

DistributorsManagerView::~DistributorsManagerView()
{

}

void DistributorsManagerView::slotAddDistributor() {}
void DistributorsManagerView::slotDeleteDistributor() {}

