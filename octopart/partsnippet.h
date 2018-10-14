#ifndef PARTSNIPPET_H
#define PARTSNIPPET_H

#include <QSharedDataPointer>
#include <QString>

namespace Octopart {

    class PartSnippet
    {
    public:
        PartSnippet();
        PartSnippet(const PartSnippet &);
        PartSnippet &operator=(const PartSnippet &);
        ~PartSnippet();

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


    private:
        class Private;
        QSharedDataPointer<Private> data;
    };

}// namespace

#endif // PARTSNIPPET_H
