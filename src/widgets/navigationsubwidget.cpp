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
#include "models/partsquerybuilder.h"
#include "models/treeitemmodel.h"
#include "dialogs/treeitemeditdialog.h"

NavigationSubWidget::NavigationSubWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("NavigationSubWidget");
   _navigationComboBox = new QComboBox(this);   
   _navigationComboBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   _navigationComboBox->setFocusPolicy(Qt::TabFocus);
   _navigationComboBox->setMinimumContentsLength(0);
   _navigationComboBox->setObjectName("navigationComboBox");

   _toolBar = new Manhattan::StyledBar(this);
   _toolBar->setObjectName("toolBar");
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
    _filterLineEdit->setObjectName("QSearchLineEdit");

    _treeView = new QTreeView;
    _treeView->setObjectName("QTreeView");
    _treeView->setFrameStyle(QFrame::NoFrame);
    _treeView->setTextElideMode(Qt::ElideNone);
    _treeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    _treeView->setDragDropMode(QAbstractItemView::DragDrop);
    //_treeView->setDragDropMode(QAbstractItemView::InternalMove);
    _treeView->setDefaultDropAction(Qt::MoveAction);
    _treeView->setHeaderHidden(true);
    _treeView->setSelectionMode(QTreeView::SingleSelection);
    _treeView->setSelectionBehavior(QAbstractItemView::SelectItems);
    _treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView->setDropIndicatorShown(true);
    _treeView->setDragEnabled(true);
    _treeView->setAutoExpandDelay(750);
    _treeView->setAnimated(true);
    _treeView->setUniformRowHeights(true);       
    //_treeView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
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

void TreeNavigator::setModel(TreeItemModel *model)
{
    if(_treeView->selectionModel()){
        disconnect(_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(slotCurrentChanged(QModelIndex,QModelIndex)));
    }
    _model = model;
    _treeView->setModel(model);
#if QT_VERSION >= 0x050000
    _treeView->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
#else
    _treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    _treeView->header()->setStretchLastSection(false);
    connect(_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotCurrentChanged(QModelIndex,QModelIndex)));
}

TreeItemModel * TreeNavigator::model() const
{
    return _model;
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

QModelIndex TreeNavigator::currentIndex() const
{
    return _treeView->currentIndex();
}

QTreeView * TreeNavigator::view()
{
    return _treeView;
}


void TreeNavigator::slotTreeFilterModeToggled(bool checked)
{
    _filterSelectedItemChecked=checked;    
    QString tooltip = checked ? tr("Scope: Selected item") : tr("Scope: Include childs");
    static_cast<QWidget*>(sender())->setToolTip(tooltip);
    slotCurrentChanged(_treeView->currentIndex(), QModelIndex());
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

void TreeNavigator::slotCurrentChanged(const QModelIndex &current, const QModelIndex &)
{    
    QList<int> selected;
    if(_filterSelectedItemChecked){
        int itemId = _model->getItemId(current);
        if(itemId>=0){
            selected.append(itemId);
        }
    }
    else{
        selected = _model->getSubTreeIds(current);
    }
    emit selectionChanged(selected);    
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

QVariant CategoryNavigator::currentCategory() const
{
    QModelIndex currentIdx = currentIndex();
    if(currentIdx.isValid()){
        return model()->getItemId(currentIdx);
    }
    else{
        return model()->rootItemId();
    }
}

void CategoryNavigator::onContextMenuRequested(const QPoint & globalPos, const QModelIndex & index)
{
    bool indexValid = index.isValid();
    _actionEditCategory->setEnabled(indexValid);
    _actionDeleteCategory->setEnabled(indexValid);
    _actionsMenu->exec(globalPos);
}

void CategoryNavigator::onFilterChanged(const QString & text)
{
}

void CategoryNavigator::slotAddCategory()
{
    const QModelIndex & parentIndex = currentIndex();
    if(model()->insertItem(parentIndex)){
        view()->expand(parentIndex);        
        int newRow = model()->rowCount(parentIndex) - 1;
        QModelIndex itemIndex = model()->index(newRow, 0, parentIndex);
        TreeItemEditDialog dlg(this);
        dlg.setWindowTitle(tr("Add new category"));
        dlg.setItemName(model()->data(itemIndex, Qt::EditRole).toString());
        dlg.setItemDescription(model()->data(itemIndex, Qt::ToolTipRole).toString());
        if(dlg.exec()==QDialog::Accepted){
            model()->setData(itemIndex, dlg.itemName(), Qt::EditRole);
            model()->setData(itemIndex, dlg.itemDescription(), Qt::ToolTipRole);
            model()->submit();
            QModelIndex childIndex = model()->index(newRow, 0, parentIndex);
            view()->setCurrentIndex(childIndex);
        }
        else{
            model()->revert();
            view()->setCurrentIndex(parentIndex);
        }

        view()->setFocus();
    }
    else{
        qWarning()<<"Failed to insert category";
    }

}

void CategoryNavigator::slotDeleteCategory()
{
    QModelIndex currentIdx = currentIndex();
    if(currentIdx.isValid()){
        model()->removeItem(currentIdx);
    }
}

void CategoryNavigator::slotEditCategory()
{
    QModelIndex itemIndex = currentIndex();
    if(!itemIndex.isValid()){
        return;
    }

    TreeItemEditDialog dlg(this);
    dlg.setWindowTitle(tr("Edit category"));
    dlg.setItemName(model()->data(itemIndex, Qt::EditRole).toString());
    dlg.setItemDescription(model()->data(itemIndex, Qt::ToolTipRole).toString());
    if(dlg.exec()==QDialog::Accepted){
        model()->setData(itemIndex, dlg.itemName(), Qt::EditRole);
        model()->setData(itemIndex, dlg.itemDescription(), Qt::ToolTipRole);
        model()->submit();
    }
    else{
        model()->revert();
    }
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

QVariant StorageNavigator::currentStorage() const
{
    QModelIndex currentIdx = currentIndex();
    if(currentIdx.isValid()){
        return model()->getItemId(currentIdx);
    }
    else{
        return model()->rootItemId();
    }
}

void StorageNavigator::onContextMenuRequested(const QPoint & globalPos, const QModelIndex & index)
{
    bool indexValid = index.isValid();
    _actionDeleteStorage->setEnabled(indexValid);
    _actionEditStorage->setEnabled(indexValid);
    _actionsMenu->exec(globalPos);
}

void StorageNavigator::onFilterChanged(const QString & text)
{
}

void StorageNavigator::slotAddStorage()
{
    const QModelIndex & parentIndex = currentIndex();
    if(model()->insertItem(parentIndex)){
        view()->expand(parentIndex);
        int newRow = model()->rowCount(parentIndex) - 1;
        QModelIndex itemIndex = model()->index(newRow, 0, parentIndex);
        TreeItemEditDialog dlg(this);
        dlg.setWindowTitle(tr("Add new storage"));
        dlg.setItemName(model()->data(itemIndex, Qt::EditRole).toString());
        dlg.setItemDescription(model()->data(itemIndex, Qt::ToolTipRole).toString());
        if(dlg.exec()==QDialog::Accepted){
            model()->setData(itemIndex, dlg.itemName(), Qt::EditRole);
            model()->setData(itemIndex, dlg.itemDescription(), Qt::ToolTipRole);
            model()->submit();
            QModelIndex childIndex = model()->index(newRow, 0, parentIndex);
            view()->setCurrentIndex(childIndex);
        }
        else{
            model()->revert();
            view()->setCurrentIndex(parentIndex);
        }

        view()->setFocus();
    }
    else{
        qWarning()<<"Failed to insert storage";
    }
}

void StorageNavigator::slotDeleteStorage()
{
    QModelIndex currentIdx = currentIndex();
    if(currentIdx.isValid()){
        model()->removeItem(currentIdx);
    }
}

void StorageNavigator::slotEditStorage()
{
    QModelIndex itemIndex = currentIndex();
    if(!itemIndex.isValid()){
        return;
    }

    TreeItemEditDialog dlg(this);
    dlg.setWindowTitle(tr("Edit storage"));
    dlg.setItemName(model()->data(itemIndex, Qt::EditRole).toString());
    dlg.setItemDescription(model()->data(itemIndex, Qt::ToolTipRole).toString());
    if(dlg.exec()==QDialog::Accepted){
        model()->setData(itemIndex, dlg.itemName(), Qt::EditRole);
        model()->setData(itemIndex, dlg.itemDescription(), Qt::ToolTipRole);
        model()->submit();
    }
    else{
        model()->revert();
    }
}
