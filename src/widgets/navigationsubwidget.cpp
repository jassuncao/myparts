/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "navigationsubwidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMenu>
#include <QResizeEvent>
#include <QToolButton>
#include <QStandardItemModel>
#include <QStringListModel>

#include "styledbar.h"

#include <QTreeView>
#include "navigationtreeview.h"

NavigationSubWidget::NavigationSubWidget(QWidget *parent) : QWidget(parent)
{  
   _navigationComboBox = new QComboBox(this);
   _navigationComboBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   _navigationComboBox->setFocusPolicy(Qt::TabFocus);
   _navigationComboBox->setMinimumContentsLength(0);
   _navigationComboBox->addItem(tr("Categories"), Category);
   _navigationComboBox->addItem(tr("Storage"), Storage);

   _toolBar = new Manhattan::StyledBar(this);
   QHBoxLayout * toolBarLayout = new QHBoxLayout;
   toolBarLayout->setMargin(0);
   toolBarLayout->setSpacing(0);
   _toolBar->setLayout(toolBarLayout);
   toolBarLayout->addWidget(_navigationComboBox);

   QToolButton * expandTreeBtn = new QToolButton(this);
   expandTreeBtn->setIcon(QIcon(QLatin1String(":/icons/expand_tree")));
   expandTreeBtn->setToolTip("Expand All");

   QToolButton * collapseTreeBtn = new QToolButton(this);
   collapseTreeBtn->setIcon(QIcon(QLatin1String(":/icons/collapse_tree")));
   collapseTreeBtn->setToolTip("Collapse All");

   QIcon filterModeIcon;
   filterModeIcon.addPixmap(QPixmap(QLatin1String(":/icons/tree_filter_childs")), QIcon::Normal,  QIcon::Off);
   filterModeIcon.addPixmap(QPixmap(QLatin1String(":/icons/tree_filter_selected")), QIcon::Normal, QIcon::On);

   _treeFilterModeBtn = new QToolButton(this);
   _treeFilterModeBtn->setIcon(filterModeIcon);
   _treeFilterModeBtn->setCheckable(true);
   connect(_treeFilterModeBtn, SIGNAL(toggled(bool)), this, SLOT(slotTreeFilterModeToggled(bool)));

   toolBarLayout->addWidget(expandTreeBtn);
   toolBarLayout->addWidget(collapseTreeBtn);
   toolBarLayout->addWidget(new Manhattan::StyledSeparator());
   toolBarLayout->addWidget(_treeFilterModeBtn);


   _treeView = new QTreeView;
   _treeView->setFrameStyle(QFrame::NoFrame);
   _treeView->setTextElideMode(Qt::ElideNone);
   _treeView->setAttribute(Qt::WA_MacShowFocusRect, false);
   _treeView->setDragDropMode(QAbstractItemView::DragDrop);
   _treeView->setDefaultDropAction(Qt::MoveAction);
   _treeView->setHeaderHidden(true);

   QVBoxLayout *lay = new QVBoxLayout();
   lay->setMargin(0);
   lay->setSpacing(0);
   setLayout(lay);
   lay->addWidget(_toolBar);
   connect(_navigationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComboBoxIndexChanged(int)));
   connect(expandTreeBtn, SIGNAL(clicked()), _treeView, SLOT(expandAll()));
   connect(collapseTreeBtn, SIGNAL(clicked()), this, SLOT(slotColllapseAll()));
   lay->addWidget(_treeView);
   slotTreeFilterModeToggled(false);
}

NavigationSubWidget::~NavigationSubWidget()
{
}

void NavigationSubWidget::setModel(QAbstractItemModel *model)
{
    _treeView->setModel(model);
    _treeView->expandToDepth(0);
}

void NavigationSubWidget::slotColllapseAll()
{
    _treeView->expandToDepth(0);
}

void NavigationSubWidget::slotTreeFilterModeToggled(bool checked)
{
    if(checked){
        _treeFilterModeBtn->setToolTip(tr("Scope: Selected item"));
    }
    else{
        _treeFilterModeBtn->setToolTip(tr("Scope: Include childs"));
    }
}

void NavigationSubWidget::slotComboBoxIndexChanged(int index)
{    
    int mode = _navigationComboBox->itemData(index).toInt();
    emit modeChanged(mode);
}

void NavigationSubWidget::setFocusWidget()
{
    _treeView->setFocus();
}

