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
#include <QModelIndex>
#include <QStackedLayout>
#include <QTreeView>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDebug>

#include "styledbar.h"
#include "navigationtreeview.h"
#include "qsearchlineedit.h"
#include "models/partssqltablemodel.h"

NavigationSubWidget::NavigationSubWidget(QWidget *parent) : QWidget(parent)
{  
   _navigationComboBox = new QComboBox(this);
   _navigationComboBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   _navigationComboBox->setFocusPolicy(Qt::TabFocus);
   _navigationComboBox->setMinimumContentsLength(0);

   _toolBar = new Manhattan::StyledBar(this);
   QHBoxLayout * toolBarLayout = new QHBoxLayout;
   toolBarLayout->setMargin(0);
   toolBarLayout->setSpacing(0);
   _toolBar->setLayout(toolBarLayout);
   toolBarLayout->addWidget(_navigationComboBox);

   _stack = new QStackedLayout();
   _stack->setMargin(0);
   _stack->setSpacing(0);

   QVBoxLayout *lay = new QVBoxLayout();
   lay->setMargin(0);
   lay->setSpacing(0);
   setLayout(lay);
   lay->addWidget(_toolBar);
   lay->addLayout(_stack);
   connect(_navigationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComboBoxIndexChanged(int)));
}

NavigationSubWidget::~NavigationSubWidget()
{
}

void NavigationSubWidget::addNavigator(TreeNavigator * navigator)
{
    _navigationComboBox->addItem(navigator->title());
    int index = _stack->addWidget(navigator);
    if(_navigationComboBox->currentIndex()!=index)
        _navigationComboBox->setCurrentIndex(index);
}

void NavigationSubWidget::setCurrentNavigator(int index)
{
    _navigationComboBox->setCurrentIndex(index);
}

void NavigationSubWidget::slotComboBoxIndexChanged(int index)
{        
    if(index==_stack->currentIndex())
        return;

    QLayout * toolbarLayout = _toolBar->layout();
    while (!_extraToolButtons.isEmpty()){
        QToolButton * btn = _extraToolButtons.takeFirst();
        toolbarLayout->removeWidget(btn);
        delete btn;
    }
    _stack->setCurrentIndex(index);
    QWidget * current = _stack->currentWidget();
    if(current){
        setFocusProxy(current);
        TreeNavigator * navigator = dynamic_cast<TreeNavigator*>(current);
        if(navigator){
            QList<QToolButton *> newButtons = navigator->toolButtons();
            QToolButton * btn;
            foreach (btn, newButtons) {
                _extraToolButtons.append(btn);
                toolbarLayout->addWidget(btn);
            }
            foreach (btn, newButtons) {
                btn->show();
            }
        }
    }
}


TreeNavigator::TreeNavigator(QWidget *parent) : QWidget(parent),
  _filterSelectedItemChecked(false)
{
    _filterLineEdit = new QSearchLineEdit(this);

    _treeView = new QTreeView;
    _treeView->setFrameStyle(QFrame::NoFrame);
    _treeView->setTextElideMode(Qt::ElideNone);
    _treeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    //_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    _treeView->setDragDropMode(QAbstractItemView::InternalMove);
    _treeView->setDefaultDropAction(Qt::MoveAction);
    _treeView->setHeaderHidden(true);
    _treeView->setSelectionMode(QTreeView::SingleSelection);
    _treeView->setSelectionBehavior(QAbstractItemView::SelectItems);
    _treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView->setDropIndicatorShown(true);
    _treeView->setDragEnabled(true);
    _treeView->setAutoExpandDelay(750);
    _treeView->setAnimated(true);
    setFocusProxy(_treeView);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_filterLineEdit);
    layout->addWidget(_treeView);

    setLayout(layout);

    connect(_filterLineEdit, SIGNAL(textClear()), this, SLOT(slotTextChanged()));
    connect(_filterLineEdit, SIGNAL(returnPressed()), this, SLOT(slotTextChanged()));
    connect(_treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomContextMenuRequested(QPoint)));
}

void TreeNavigator::setModel(QAbstractItemModel *model)
{
    if(_treeView->selectionModel()){
        disconnect(_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(slotCurrentChanged(QModelIndex,QModelIndex)));
    }
    _treeView->setModel(model);
    connect(_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotCurrentChanged(QModelIndex,QModelIndex)));
}

QAbstractItemModel * TreeNavigator::model() const
{
    return _treeView->model();
}

void TreeNavigator::setPartsModel(PartsSqlTableModel * partsModel)
{
    _partsModel = partsModel;
}

QList<QToolButton *> TreeNavigator::toolButtons()
{

    QToolButton * expandTreeBtn = new QToolButton;
    expandTreeBtn->setIcon(QIcon(QLatin1String(":/icons/expand_tree")));
    expandTreeBtn->setToolTip("Expand All");
    expandTreeBtn->hide();

    QToolButton * collapseTreeBtn = new QToolButton;
    collapseTreeBtn->setIcon(QIcon(QLatin1String(":/icons/collapse_tree")));
    collapseTreeBtn->setToolTip("Collapse All");
    collapseTreeBtn->hide();

    QIcon filterModeIcon;
    filterModeIcon.addPixmap(QPixmap(QLatin1String(":/icons/tree_filter_childs")), QIcon::Normal,  QIcon::Off);
    filterModeIcon.addPixmap(QPixmap(QLatin1String(":/icons/tree_filter_selected")), QIcon::Normal, QIcon::On);

    QToolButton * treeFilterModeBtn = new QToolButton;
    treeFilterModeBtn->setIcon(filterModeIcon);
    treeFilterModeBtn->setCheckable(true);
    treeFilterModeBtn->hide();
    treeFilterModeBtn->setChecked(_filterSelectedItemChecked);

    QList<QToolButton*> _toolButtons;
    _toolButtons.append(expandTreeBtn);
    _toolButtons.append(collapseTreeBtn);
    _toolButtons.append(treeFilterModeBtn);

    connect(treeFilterModeBtn, SIGNAL(toggled(bool)), this, SLOT(slotTreeFilterModeToggled(bool)));
    connect(expandTreeBtn, SIGNAL(clicked()), _treeView, SLOT(expandAll()));
    connect(collapseTreeBtn, SIGNAL(clicked()), this, SLOT(slotCollapseAll()));

    return _toolButtons;
}

void TreeNavigator::slotCollapseAll()
{
    _treeView->expandToDepth(0);
}

void TreeNavigator::onContextMenuRequested(const QPoint &, const QModelIndex &)
{
}

void TreeNavigator::onFilterChanged(const QString &)
{

}

void TreeNavigator::slotTreeFilterModeToggled(bool checked)
{
    _filterSelectedItemChecked=checked;
    QString tooltip = checked ? tr("Scope: Selected item") : tr("Scope: Include childs");
    static_cast<QWidget*>(sender())->setToolTip(tooltip);
}


void TreeNavigator::slotCustomContextMenuRequested(const QPoint &pos)
{
    const QModelIndex & index = _treeView->indexAt(pos);
    const QPoint & globalPos = _treeView->mapToGlobal(pos);
    onContextMenuRequested(globalPos, index);
}

void TreeNavigator::slotTextChanged()
{
    onFilterChanged(_filterLineEdit->text());
}

void TreeNavigator::slotCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    qDebug()<<"Changed to "<<current;
}


CategoryNavigator::CategoryNavigator(QWidget *parent)
    : TreeNavigator(parent)
{   
    QIcon addCategoryIcon(QString::fromLatin1(":/icons/folder_add"));
    QIcon deleteCategoryIcon(QString::fromLatin1(":/icons/folder_delete"));
    QIcon editCategoryIcon(QString::fromLatin1(":/icons/folder_edit"));
    _actionsMenu = new QMenu(this);
    _actionsMenu->addAction(addCategoryIcon, tr("Add Category"), this, SLOT(slotAddCategory()));
    _actionDeleteCategory = _actionsMenu->addAction(deleteCategoryIcon, tr("Delete Category"), this, SLOT(slotDeleteCategory()));
    _actionEditCategory = _actionsMenu->addAction(editCategoryIcon, tr("Edit Category"), this, SLOT(slotEditCategory()));
    _actionDeleteCategory->setEnabled(false);
    _actionEditCategory->setEnabled(false);  
}

void CategoryNavigator::onContextMenuRequested(const QPoint & globalPos, const QModelIndex & index)
{
    bool indexValid = index.isValid();
    bool canDelete = false;
    if(indexValid){
        canDelete = !index.sibling(0, 0).isValid();
    }
    _actionDeleteCategory->setEnabled(indexValid);
    _actionEditCategory->setEnabled(canDelete);
    _actionsMenu->exec(globalPos);
}

void CategoryNavigator::onFilterChanged(const QString & text)
{
}

void CategoryNavigator::slotAddCategory()
{

}

void CategoryNavigator::slotDeleteCategory()
{

}

void CategoryNavigator::slotEditCategory()
{
}

StorageNavigator::StorageNavigator(QWidget *parent)
    : TreeNavigator(parent)
{
    QIcon addStorageIcon(QString::fromLatin1(":/icons/box_add"));
    QIcon deleteStorageIcon(QString::fromLatin1(":/icons/box_delete"));
    QIcon editStorageIcon(QString::fromLatin1(":/icons/box_edit"));
    _actionsMenu = new QMenu(this);
    _actionsMenu->addAction(addStorageIcon, tr("Add Storage"), this, SLOT(slotAddStorage()));
    _actionDeleteStorage = _actionsMenu->addAction(deleteStorageIcon, tr("Delete Storage"), this, SLOT(slotDeleteStorage()));
    _actionEditStorage = _actionsMenu->addAction(editStorageIcon, tr("Edit Storage"), this, SLOT(slotEditStorage()));
    _actionDeleteStorage->setEnabled(false);
    _actionEditStorage->setEnabled(false);

}

void StorageNavigator::onContextMenuRequested(const QPoint & globalPos, const QModelIndex & index)
{
    bool indexValid = index.isValid();
    bool canDelete = false;
    if(indexValid){
        canDelete = !index.sibling(0, 0).isValid();
    }
    _actionDeleteStorage->setEnabled(indexValid);
    _actionEditStorage->setEnabled(canDelete);
    _actionsMenu->exec(globalPos);
}

void StorageNavigator::onFilterChanged(const QString & text)
{
}

void StorageNavigator::slotAddStorage()
{

}

void StorageNavigator::slotDeleteStorage()
{

}

void StorageNavigator::slotEditStorage()
{
}
