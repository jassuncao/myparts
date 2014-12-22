#ifndef CATEGOYMIMEDATA_H
#define CATEGOYMIMEDATA_H

#include <QMimeData>
#include <QModelIndexList>

class CategoyMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit CategoyMimeData(const QModelIndexList &indexes);
    const QModelIndexList & indexes() const {return _indexes;}
signals:
    
public slots:
private:
    const QModelIndexList & _indexes;
};

#endif // CATEGOYMIMEDATA_H
