#ifndef PARTMIMEDATA_H
#define PARTMIMEDATA_H

#include <QMimeData>

class PartMimeData : public QMimeData
{
    Q_OBJECT
public:
    static const char * PART_MIME_TYPE;

    explicit PartMimeData();
    QStringList formats() const;
    
signals:
    void dataRequested(const QString &mimeType) const;
    
protected:
    QVariant retrieveData(const QString &mimetype, QVariant::Type type) const;
    
};

#endif // PARTMIMEDATA_H
