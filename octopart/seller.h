#ifndef SELLER_H
#define SELLER_H

#include <QSharedDataPointer>
#include <QString>

namespace Octopart {

    class Seller
    {
    public:
        Seller();
        Seller(const Seller &);
        Seller &operator=(const Seller &);
        ~Seller();        

        QString name() const;
        void setName(const QString &value);

        QString url() const;
        void setUrl(const QString &value);

        QString uid() const;
        void setUid(const QString &value);

        QString countryCode() const;
        void setCountryCode(const QString &value);

    private:
        class SellerData;
        QSharedDataPointer<SellerData> data;
    };

}

#endif // SELLER_H
