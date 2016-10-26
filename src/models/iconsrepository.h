#ifndef ICONSREPOSITORY_H
#define ICONSREPOSITORY_H

#include <QString>
#include <QAbstractListModel>
#include <QHash>
#include <QIcon>

class IconsRepository
{
public:
    IconsRepository(const QString & iconsDefinitionFile);
    void addIcon(const QString& key, const QIcon & icon);
    QIcon getIcon(const QString & key) const;
    QAbstractListModel* model(QObject *parent=0) const;
private:
    QHash<QString, QIcon> _cache;
};

#endif // ICONSREPOSITORY_H
