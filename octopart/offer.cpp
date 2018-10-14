#include "offer.h"

namespace Octopart {


class Offer::Private : public QSharedData
{
public:
    QVariant moq;
    QString packaging;
    QString sku;
    Seller seller;
    QVariant moqPrice;
    QString currency;
    QString url;

};

Offer::Offer() : data(new Offer::Private)
{

}

Offer::Offer(const Offer &rhs) : data(rhs.data)
{

}

Offer &Offer::operator=(const Offer &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

Offer::~Offer()
{

}

QVariant Offer::moq() const {return data->moq;}

void Offer::setMoq(const QVariant& value)
{
    data->moq = value;
}


QString Offer::packaging() const {return data->packaging;}

void Offer::setPackaging(const QString& value)
{
    data->packaging = value;
}

QString Offer::sku() const {return data->sku;}

void Offer::setSku(const QString& value)
{
    data->sku = value;
}


Seller Offer::seller() const {return data->seller;}

void Offer::setSeller(const Seller& value)
{
    data->seller = value;
}


QVariant Offer::moqPrice() const {return data->moqPrice;}

void Offer::setMoqPrice(const QVariant& value)
{
    data->moqPrice = value;
}

QString Offer::currency() const {return data->currency;}

void Offer::setCurrency(const QString& value)
{
    data->currency = value;
}

QString Offer::url() const {return data->url;}

void Offer::setUrl(const QString& value)
{
    data->url = value;
}


} //namespace
