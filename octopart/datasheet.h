#ifndef DATASHEET_H
#define DATASHEET_H

#include "octopart_global.h"
#include <QSharedDataPointer>
#include <QString>

namespace Octopart {

class OCTOPART_SHARED_EXPORT Datasheet
{
public:
    Datasheet();
    Datasheet(const Datasheet &other);
    Datasheet &operator=(const Datasheet &other);
    ~Datasheet();

    QString sourceName() const;
    void setSourceName(const QString &sourceName);
    QString mimetype() const;
    void setMimetype(const QString &mimetype);
    QString url() const;
    void setUrl(const QString &url);
    int numPages() const;
    void setNumPages(const int numPages);

private:
    class Private;
    QSharedDataPointer<Private> data;
};


} // namespace

#endif // DATASHEET_H
