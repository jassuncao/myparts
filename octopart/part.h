#ifndef PART_H
#define PART_H

#include <QSharedDataPointer>
#include <QList>
#include "datasheet.h"
#include "partspec.h"
#include "offer.h"

namespace Octopart {

    class Part
    {
    public:
        Part();
        Part(const Part &);
        Part &operator=(const Part &);
        ~Part();

        QString uid() const;
        void setUid(const QString value);

        QString mpn() const;
        void setMpn(const QString value);

        QString url() const;
        void setUrl(const QString value);

        QString description() const;
        void setDescription(const QString value);

        QString brandName() const;
        void setBrandName(const QString value);

        QList<Datasheet> datasheets() const;
        void setDatasheets(const QList<Datasheet> value);

        QList<PartSpec> specs() const;
        void setSpecs(const QList<PartSpec> value);

        QList<Offer> offers() const;
        void setOffers(const QList<Offer> value);

    private:
        class Private;
        QSharedDataPointer<Private> data;
    };

} // namespace



#endif // PART_H
