#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QObject>

class FlowLayout : public QObject
{
    Q_OBJECT
public:
    explicit FlowLayout(QObject *parent = 0);
    ~FlowLayout();

signals:

public slots:
};

#endif // FLOWLAYOUT_H
