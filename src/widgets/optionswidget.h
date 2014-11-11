#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QWidget>

class OptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OptionsWidget(QWidget *parent = 0);
    
signals:
    void dataChanged();
    void dataCommited();
    
public slots:
    virtual bool submit()=0;
    virtual void revert()=0;
    
};

#endif // OPTIONSWIDGET_H
