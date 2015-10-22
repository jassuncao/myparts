#include "partsmanagerview.h"
#include "widgets/navigationsubwidget.h"

#include <QHBoxLayout>

PartsManagerView::PartsManagerView(QWidget *parent) : QWidget(parent)
{

    NavigationSubWidget * navWidget = new NavigationSubWidget(this);
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    setLayout(hLayout);
    hLayout->addWidget(navWidget);
    hLayout->addWidget(new QWidget);

}

PartsManagerView::~PartsManagerView()
{

}

