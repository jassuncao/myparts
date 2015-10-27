#include "abstracteditor.h"

AbstractEditor::AbstractEditor(QWidget *parent) : QWidget(parent)
{
}

AbstractEditor::~AbstractEditor()
{
}

void AbstractEditor::submitChilds(const QVariant &)
{
}

bool AbstractEditor::validate()
{
    return true;
}

void AbstractEditor::slotContentChanged()
{
    emit contentChanged();
}

