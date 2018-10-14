#include "partsnippet.h"

using namespace Octopart;


class PartSnippet::Private : public QSharedData
{
public:
    Private() {}
    Private(const Private &other) : QSharedData(other),
        uid(other.uid),
        mpn(other.mpn),
        url(other.url),
        description(other.description),
        brandName(other.brandName)
    {}

    QString uid;
    QString mpn;
    QString url;
    QString description;
    QString brandName;

};

PartSnippet::PartSnippet() : data(new Private)
{

}

PartSnippet::PartSnippet(const PartSnippet &rhs) : data(rhs.data)
{

}

PartSnippet &PartSnippet::operator=(const PartSnippet &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

PartSnippet::~PartSnippet()
{

}

QString PartSnippet::uid() const
{
    return data->uid;
}

void PartSnippet::setUid(const QString value)
{
    data->uid = value;
}

QString PartSnippet::mpn() const
{
    return data->mpn;
}

void PartSnippet::setMpn(const QString value)
{
    data->mpn = value;
}

QString PartSnippet::url() const
{
    return data->url;
}

void PartSnippet::setUrl(const QString value)
{
    data->url = value;
}

QString PartSnippet::description() const
{
    return data->description;
}

void PartSnippet::setDescription(const QString value)
{
    data->description = value;
}

QString PartSnippet::brandName() const
{
    return data->brandName;
}

void PartSnippet::setBrandName(const QString value)
{
    data->brandName = value;
}
