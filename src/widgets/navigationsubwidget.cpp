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
   QStringListModel * model = new QStringListModel();
   QStringList list;
   list << "a" << "b" << "c"<<"d"<<"e"<<"f";
   model->setStringList(list);

   m_navigationComboBox = new QComboBox(this);
   m_navigationComboBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   m_navigationComboBox->setFocusPolicy(Qt::TabFocus);
   m_navigationComboBox->setMinimumContentsLength(0);
   m_navigationComboBox->setModel(model);
   m_navigationWidget = 0;
   //m_navigationWidgetFactory = 0;

   m_toolBar = new Manhattan::StyledBar(this);
   QHBoxLayout *toolBarLayout = new QHBoxLayout;
   toolBarLayout->setMargin(0);
   toolBarLayout->setSpacing(0);
   m_toolBar->setLayout(toolBarLayout);
   toolBarLayout->addWidget(m_navigationComboBox);

   QVBoxLayout *lay = new QVBoxLayout();
   lay->setMargin(0);
   lay->setSpacing(0);
   setLayout(lay);
   lay->addWidget(m_toolBar);
   connect(m_navigationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxIndexChanged(int)));
   //comboBoxIndexChanged(factoryIndex);
}

NavigationSubWidget::~NavigationSubWidget()
{

}

void NavigationSubWidget::comboBoxIndexChanged(int factoryIndex)
{
    //jassuncao saveSettings();

    // Remove toolbutton
    foreach (QWidget *w, m_additionalToolBarWidgets)
        delete w;
    m_additionalToolBarWidgets.clear();

    // Remove old Widget
    delete m_navigationWidget;
    m_navigationWidget = 0;
    //jassuncao: m_navigationWidgetFactory = 0;
    if (factoryIndex == -1)
        return;

    // Get new stuff
    /*
    m_navigationWidgetFactory = m_navigationComboBox->itemData(factoryIndex,
                           NavigationWidget::FactoryObjectRole).value<INavigationWidgetFactory *>();
    NavigationView n = m_navigationWidgetFactory->createWidget();
    m_navigationWidget = n.widget;
    */
    m_navigationWidget = new NavigationTreeView();
    layout()->addWidget(m_navigationWidget);

    QList<QToolButton *> list;
    QToolButton * button1 = new QToolButton();
    button1->setIcon(QIcon(QLatin1String(":icons/toggle_collapse")));
    list << button1;
    // Add Toolbutton
    m_additionalToolBarWidgets = list;
    QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(m_toolBar->layout());
    foreach (QToolButton *w, m_additionalToolBarWidgets) {
        layout->insertWidget(layout->count()-2, w);
    }

    //restoreSettings();
}

void NavigationSubWidget::setFocusWidget()
{
    if (m_navigationWidget)
        m_navigationWidget->setFocus();
}

/*
CommandComboBox::CommandComboBox(QWidget *parent) : QComboBox(parent)
{
}

bool CommandComboBox::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        const QString text = currentText();
        if (const Core::Command *cmd = command(text)) {
            const QString tooltip = tr("Activate %1 View").arg(text);
            setToolTip(cmd->stringWithAppendedShortcut(tooltip));
        } else {
            setToolTip(text);
        }
    }
    return QComboBox::event(e);
}
*/

