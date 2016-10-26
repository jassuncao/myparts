#include "iconsrepository.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

class IconsRepositoryReader {
public:
    IconsRepositoryReader(const QString & filename, IconsRepository & repository);
    void read();
private:
    void processIconTag();
    QString readNextText();
    QIcon::State getIconState();
    QString _filename;
    IconsRepository & _repository;
    QXmlStreamReader _xmlReader;
};

class IconsRepositoryModel : public QAbstractListModel {
public:
    IconsRepositoryModel(QList<QString> keys, QList<QIcon> icons, QObject *parent);
    virtual ~IconsRepositoryModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
private:
    const QList<QString> _keys;
    const QList<QIcon> _icons;
};

IconsRepository::IconsRepository(const QString &iconsDefinitionFile)
{
    IconsRepositoryReader reader(iconsDefinitionFile, *this);
    reader.read();
}

void IconsRepository::addIcon(const QString& key, const QIcon & icon)
{
    _cache.insert(key, icon);
}


QIcon IconsRepository::getIcon(const QString & key) const
{
    return _cache.value(key);
}

QAbstractListModel* IconsRepository::model(QObject *parent) const
{
    return new IconsRepositoryModel(_cache.keys(), _cache.values(),  parent);
}

IconsRepositoryModel::IconsRepositoryModel(QList<QString> keys, QList<QIcon> icons, QObject *parent)
    : QAbstractListModel(parent),
      _keys(keys),
      _icons(icons)
{
    Q_ASSERT(_keys.size() == icons.size());
}

IconsRepositoryModel::~IconsRepositoryModel()
{
}


int IconsRepositoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _keys.size();
}

int IconsRepositoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant IconsRepositoryModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _keys.size())
        return QVariant();
    if (role == Qt::DecorationRole)
        return _icons.at(index.row());
    if (role == Qt::EditRole)
        return _keys.at(index.row());
    return QVariant();
}

IconsRepositoryReader::IconsRepositoryReader(const QString &filename, IconsRepository & repository)
    : _filename(filename),
      _repository(repository)
{
}

void IconsRepositoryReader::read()
{
    QFile file(_filename);
    file.open(QFile::ReadOnly);
    _xmlReader.setDevice(&file);

    if (_xmlReader.readNextStartElement() && _xmlReader.name() == "icons"){
        while (_xmlReader.readNextStartElement()) {
            if (_xmlReader.name() == "icon"){
                processIconTag();
            } else{
                _xmlReader.skipCurrentElement();
            }
        }
    }
    if (_xmlReader.tokenType() == QXmlStreamReader::Invalid){
        _xmlReader.readNext();
    }
    if (_xmlReader.hasError()) {
        _xmlReader.raiseError();
        qDebug() << _xmlReader.errorString();
    }
    file.close();
}

void IconsRepositoryReader::processIconTag()
{
    if (!_xmlReader.isStartElement() || _xmlReader.name() != "icon"){
        return;
    }
    QString id = _xmlReader.attributes().value("", "id").toString();
    qDebug()<<"Reading icon "<<id;
    QIcon icon;
    while (_xmlReader.readNextStartElement()) {
        if (_xmlReader.name() == "pixmap"){
            QIcon::State state = getIconState();
            QString pixmap = readNextText();
            icon.addFile(pixmap, QSize(), QIcon::Normal, state);
        }
        _xmlReader.skipCurrentElement();
   }
    if(!id.isEmpty()){
        _repository.addIcon(id, icon);
    }
}

QString IconsRepositoryReader::readNextText()
{
    _xmlReader.readNext();
    return _xmlReader.text().toString();
}

QIcon::State IconsRepositoryReader::getIconState()
{
    QStringRef stateAttr = _xmlReader.attributes().value("","state");
    bool isOnState = stateAttr.compare(QString("on")) == 0 ;
    QIcon::State state = isOnState ? QIcon::On : QIcon::Off;
    return state;
}


