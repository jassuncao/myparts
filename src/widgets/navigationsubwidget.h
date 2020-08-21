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
#ifndef NAVIGATIONSUBWIDGET_H
#define NAVIGATIONSUBWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QList>
#include <QModelIndex>
#include "models/modelsrepository.h"
#include "core/iconsrepository.h"

QT_BEGIN_NAMESPACE
class QMenu;
class QToolButton;
class QTreeView;
class QToolButton;
class QAction;
class QStackedLayout;
class QSortFilterProxyModel;
class QShortcut;
QT_END_NAMESPACE

class QSearchLineEdit;
class TreeNavigator;
class PartsQueryBuilder;
class TreeItemModel;
class TreeProxyFilter;

namespace Manhattan { class StyledBar; }

class NavigationSubWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        Category,
        Storage
    };

    explicit NavigationSubWidget(QWidget *parent = 0);
    ~NavigationSubWidget();
    void addNavigator(TreeNavigator * navigator);
public slots:
    void setCurrentNavigator(int index);
private slots:
    void slotComboBoxIndexChanged(int index);  
private:    
    QComboBox * _navigationComboBox;
    Manhattan::StyledBar * _toolBar;
    QStackedLayout * _stack;
    QList<QToolButton*> _extraToolButtons;
};

class INavigator {
public:

};

class TreeNavigator : public QWidget {
    Q_OBJECT
public:
    virtual QString title() const = 0;
    void setModel(TreeItemModel *model);
    TreeItemModel *model() const;
    QList<QToolButton*> toolButtons();
    QModelIndex currentIndex() const;
signals:
    void selectionChanged(const QList<int> selectedIds);
protected:
    explicit TreeNavigator(QWidget *parent = 0);
    virtual void onContextMenuRequested(const QPoint &globalPos, const QModelIndex & index);
    virtual IconsRepository * iconsRepository() = 0;
    QTreeView * view();    
    bool doEdit(const QModelIndex itemIndex, const QString & title);
private slots:
    void slotCustomContextMenuRequested(const QPoint &pos);
    void slotTextChanged();
    void slotCurrentChanged(const QModelIndex &current, const QModelIndex &);
    void slotCollapseAll();
    void slotTreeFilterModeToggled(bool checked);
private:
    TreeItemModel * _model;
    QSearchLineEdit * _filterLineEdit;
    QTreeView * _treeView;
    TreeProxyFilter * _treeProxyModel;
    bool _filterSelectedItemChecked;
};

class CategoryNavigator : public TreeNavigator
{
    Q_OBJECT
public:
    explicit CategoryNavigator(ModelsRepository * modelsRepo, QWidget *parent = 0);
    virtual QString title() const { return tr("Categories");}
    QVariant currentCategory() const;
protected:
    void onContextMenuRequested(const QPoint &globalPos, const QModelIndex & index);  
    IconsRepository * iconsRepository();
private slots:
    void slotAddCategory();
    void slotDeleteCategory();
    void slotEditCategory();
private:
    QAction * _actionDeleteCategory;
    QAction * _actionEditCategory;
    QMenu * _actionsMenu;
    ModelsRepository * _modelsRepo;
};

class StorageNavigator : public TreeNavigator
{
    Q_OBJECT
public:
    explicit StorageNavigator(ModelsRepository * modelsRepo, QWidget *parent = 0);
    virtual ~StorageNavigator();
    virtual QString title() const { return tr("Storage");}
    QVariant currentStorage() const;
protected:
protected:
    void onContextMenuRequested(const QPoint &globalPos, const QModelIndex & index);    
    IconsRepository * iconsRepository();
private slots:
    void slotAddStorage();
    void slotAddMultipleStorage();
    void slotDeleteStorage();
    void slotEditStorage();
private:
    QAction * _actionDeleteStorage;
    QAction * _actionEditStorage;
    QMenu * _actionsMenu;
    QShortcut * _deleteShortcut;
    ModelsRepository * _modelsRepo;
};


#endif // NAVIGATIONSUBWIDGET_H
