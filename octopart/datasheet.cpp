#include "datasheet.h"

using namespace Octopart ;

class Q_DECL_HIDDEN Datasheet::Private : public QSharedData
{
public:
    QString sourceName;
    QString mimetype;
    QString url;
    int numPages;
};

Datasheet::Datasheet() : data(new Private)
{
}

Datasheet::Datasheet(const Datasheet &other) : data(other.data)
{
}

Datasheet &Datasheet::operator=(const Datasheet &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

Datasheet::~Datasheet()
{
}

QString Datasheet::sourceName() const { return data->sourceName;}

void Datasheet::setSourceName(const QString &sourceName)
{
    data->sourceName = sourceName;
}

QString Datasheet::mimetype() const { return data->sourceName;}

void Datasheet::setMimetype(const QString &mimetype)
{
    data->mimetype = mimetype;
}

QString Datasheet::url() const { return data->sourceName;}

void Datasheet::setUrl(const QString &url)
{
    data->url = url;
}

int Datasheet::numPages() const
{
    return data->numPages;
}

void Datasheet::setNumPages(const int numPages)
{
    data->numPages = numPages;
}
