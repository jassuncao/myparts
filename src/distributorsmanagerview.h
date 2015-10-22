#ifndef DISTRIBUTORSMANAGERVIEW_H
#define DISTRIBUTORSMANAGERVIEW_H

#include <QWidget>
#include "minisplitter.h"

namespace Manhattan {
class StyledBar;
class MiniSplitter;
}

class DistributorsManagerView : public Manhattan::MiniSplitter
{
    Q_OBJECT
public:
    explicit DistributorsManagerView(QWidget *parent = 0);
    ~DistributorsManagerView();

signals:

public slots:
    void slotAddDistributor();
    void slotDeleteDistributor();
private:
    Manhattan::StyledBar * _toolBar;
};

#endif // DISTRIBUTORSMANAGERVIEW_H
