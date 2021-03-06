#ifndef ABSTRACTEDITOR_H
#define ABSTRACTEDITOR_H

#include <QWidget>
#include <QModelIndex>

class AbstractEditor : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractEditor(QWidget *parent = 0);
    virtual ~AbstractEditor();       
    virtual void setModel(QAbstractItemModel * model) = 0;
    virtual void setCurrentIndex(int row) = 0;
    virtual int currentIndex() const = 0;
    virtual bool validate();
signals:
    void contentChanged();
public slots:
    virtual void submit() = 0;
    virtual void submitChilds(const QVariant &);
    virtual void revert() = 0;
protected slots:
    virtual void slotContentChanged();
};

#endif // ABSTRACTEDITOR_H
