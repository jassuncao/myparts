#ifndef OFFER_H
#define OFFER_H

#include <QSharedDataPointer>
#include <QVariant>
#include "seller.h"

namespace Octopart {

    class Offer
    {
    public:
        Offer();
        Offer(const Offer &);
        Offer &operator=(const Offer &);
        ~Offer();

        QVariant moq() const;
        void setMoq(const QVariant& value);

        QString packaging() const;
        void setPackaging(const QString& value);

        QString sku() const;
        void setSku(const QString& value);

        Seller seller() const;
        void setSeller(const Seller& value);

        QVariant moqPrice() const;
        void setMoqPrice(const QVariant &value);

        QString currency() const;
        void setCurrency(const QString& value);

        QString url() const;
        void setUrl(const QString& value);

    private:
        class Private;
        QSharedDataPointer<Private> data;
    };

} //namespace


#endif // OFFER_H
