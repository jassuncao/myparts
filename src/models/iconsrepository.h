#ifndef ICONSREPOSITORY_H
#define ICONSREPOSITORY_H

#include <QString>
#include <QAbstractListModel>
#include <QHash>
#include <QIcon>
#include <QPair>

class IconsRepository
{
public:
    IconsRepository(const QString & iconsDefinitionFile);
    void addIcon(const QString& key, const QIcon & icon, const QString& name);
    QIcon getIcon(const QString & key) const;
    QAbstractListModel* model(QObject *parent=0) const;
    QString getDefaultIcon() const {return _defaultIcon;}
    void setDefaultIcon(const QString & defaultIcon) {_defaultIcon = defaultIcon;}
private:
    QHash<QString, QPair<QIcon,QString> > _cache;
    QString _defaultIcon;
};

#endif // ICONSREPOSITORY_H
