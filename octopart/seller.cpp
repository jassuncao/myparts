#include "seller.h"
#include <QSharedData>

using namespace Octopart;

class Q_DECL_HIDDEN Seller::SellerData : public QSharedData
{
public:
    SellerData() {}
    SellerData(const SellerData &other) : QSharedData(other),
        name(other.name),
        url(other.url),
        uid(other.uid),
        countryCode(other.countryCode) {}
    //~SellerData() { }

    QString name;
    QString url;
    QString uid;
    QString countryCode;
};


Seller::Seller() : data(new SellerData)
{

}

Seller::Seller(const Seller &rhs) : data(rhs.data)
{

}

Seller &Seller::operator=(const Seller &rhs)
{   
    if (this != &rhs)
        data.operator=(rhs.data); 
    return *this;
}

Seller::~Seller()
{
}

QString Seller::name() const {return data->name;}

void Seller::setName(const QString &value)
{
    data->name = value;
}

QString Seller::url() const {return data->url;}

void Seller::setUrl(const QString &value)
{
    data->url = value;
}

QString Seller::uid() const {return data->uid;}

void Seller::setUid(const QString &value)
{
    data->uid = value;
}

QString Seller::countryCode() const {return data->countryCode;}

void Seller::setCountryCode(const QString &value)
{
    data->countryCode = value;
}

//} //namespace
