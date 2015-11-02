#include "qactionpushbutton.h"
#include <QAction>

QActionPushButton::QActionPushButton(QAction *action, QWidget *parent)
    : QToolButton(parent)
{
    setIcon(action->icon());
    setText(action->text());
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    connect(this, SIGNAL(clicked()), action, SLOT(trigger()));
    setEnabled(action->isEnabled());
}

void QActionPushButton::actionChanged()
{
    if (const QAction *a = qobject_cast<QAction*>(sender())) {
        setEnabled(a->isEnabled());
        setText(a->text());
    }
}


