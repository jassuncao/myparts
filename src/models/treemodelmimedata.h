#ifndef TREEMODELMIMEDATA_H
#define TREEMODELMIMEDATA_H

#include <QMimeData>
#include <QModelIndexList>

class TreeModelMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit TreeModelMimeData(const QModelIndexList &indexes);
    const QModelIndexList & indexes() const {return _indexes;}
private:
    const QModelIndexList & _indexes;
    
};

#endif // TREEMODELMIMEDATA_H
