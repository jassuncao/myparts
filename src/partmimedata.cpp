#include <QtGui>
#include "partmimedata.h"

const char * PartMimeData::PART_MIME_TYPE= "myparts/part";

PartMimeData::PartMimeData() :
    QMimeData()
{
}

QStringList PartMimeData::formats() const
{
    return QMimeData::formats() << "myparts/part" << "text/plain" << "text/html";
}

QVariant PartMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const
{
    emit dataRequested(mimeType);
    return QMimeData::retrieveData(mimeType, type);
}
