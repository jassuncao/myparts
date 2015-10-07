#include "qtabwidget2.h"
#include <QTabBar>
#include <QMenu>
#include <QDebug>

QTabWidget2::QTabWidget2(QWidget *parent) :
    QTabWidget(parent),
    _contextTab(-1)
{
    QTabBar * newTab = new QTabBar;
    newTab->setContextMenuPolicy(Qt::CustomContextMenu);
    setTabBar(newTab);

    _actionCloseTab = new QAction(tr("Close Tab"), this);
    _actionCloseOtherTabs = new QAction(tr("Close Other Tabs"), this);
    _actionCloseAllTabs = new QAction(tr("Close All Tabs"), this);

    connect(_actionCloseTab, SIGNAL(triggered()), this, SLOT(slotCloseCurrentTab()));
    connect(_actionCloseOtherTabs, SIGNAL(triggered()), this, SLOT(slotCloseOtherTabs()));
    connect(_actionCloseAllTabs, SIGNAL(triggered()), this, SLOT(slotCloseAllTabs()));
    connect(newTab,SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotTabContextMenu(QPoint)));
}

QTabWidget2::~QTabWidget2()
{
}


void QTabWidget2::slotTabContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    menu.addAction(_actionCloseTab);
    menu.addSeparator();
    menu.addAction(_actionCloseOtherTabs);
    menu.addAction(_actionCloseAllTabs);    
    _contextTab = tabBar()->tabAt(pos);
    _actionCloseTab->setEnabled(_contextTab>=0);
    bool f = tabBar()->count()>1;
    _actionCloseOtherTabs->setEnabled(f);
    menu.exec(tabBar()->mapToGlobal(pos));
}

void QTabWidget2::slotCloseCurrentTab(){
    if(_contextTab<0){
        qDebug()<<"Invalid Tab: "<<_contextTab;
        return;
    }
    emit QTabWidget::tabCloseRequested(_contextTab);
}

void QTabWidget2::slotCloseOtherTabs(){
    if(_contextTab<0){
        qDebug()<<"Invalid Tab: "<<_contextTab;
        return;
    }    
    tabBar()->moveTab(_contextTab, 0);
    int index = tabBar()->count()-1;
    while(index>0){
        emit QTabWidget::tabCloseRequested(index);
        --index;
    }
}

void QTabWidget2::slotCloseAllTabs(){
    int index = tabBar()->count()-1;
    while(index>=0){
        emit QTabWidget::tabCloseRequested(index);
        --index;
    }
}

