#include "part.h"

namespace Octopart {

    class Q_DECL_HIDDEN Part::Private : public QSharedData
    {
    public:
        Private() {}
        Private(const Private &other) : QSharedData(other),
            uid(other.uid),
            mpn(other.mpn),
            url(other.url),
            description(other.description),
            brandName(other.brandName),
            datasheets(other.datasheets),
            specs(other.specs),
            offers(other.offers) {}

        QString uid;
        QString mpn;
        QString url;
        QString description;
        QString brandName;
        QList<Datasheet> datasheets;
        QList<PartSpec> specs;
        QList<Offer> offers;
    };

    Part::Part() : data(new Part::Private)
    {

    }

    Part::Part(const Part &rhs) : data(rhs.data)
    {

    }

    Part &Part::operator=(const Part &rhs)
    {
        if (this != &rhs)
            data.operator=(rhs.data);
        return *this;
    }

    Part::~Part()
    {

    }

    QString Part::uid() const
    {
        return data->uid;
    }

    void Part::setUid(const QString value)
    {
        data->uid = value;
    }

    QString Part::mpn() const
    {
        return data->mpn;
    }

    void Part::setMpn(const QString value)
    {
        data->mpn = value;
    }

    QString Part::url() const
    {
        return data->url;
    }

    void Part::setUrl(const QString value)
    {
        data->url = value;
    }

    QString Part::description() const
    {
        return data->description;
    }

    void Part::setDescription(const QString value)
    {
        data->description = value;
    }

    QString Part::brandName() const
    {
        return data->brandName;
    }

    void Part::setBrandName(const QString value)
    {
        data->brandName = value;
    }

    QList<Datasheet> Part::datasheets() const
    {
        return data->datasheets;
    }

    void Part::setDatasheets(const QList<Datasheet> value)
    {
        data->datasheets = value;
    }

    QList<PartSpec> Part::specs() const
    {
        return data->specs;
    }

    void Part::setSpecs(const QList<PartSpec> value)
    {
        data->specs = value;
    }

    QList<Offer> Part::offers() const
    {
        return data->offers;
    }

    void Part::setOffers(const QList<Offer> value)
    {
        data->offers = value;
    }

} // namespace

