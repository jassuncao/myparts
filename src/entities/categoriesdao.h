#ifndef CATEGORIESDAO_H
#define CATEGORIESDAO_H

#include <QStandardItemModel>
#include <QStringList>
#include <QVector>
#include <QList>

class TreeItem;
class CategoryTreeItem;
class QStandardItem;
class QString;

namespace Entities {

class CategoriesDAO
{    
public:    
    static const int NAME_COL = 0;
    static const int DESCRIPTION_COL = 1;
    static const int ID_COL = 2;

    CategoriesDAO();
    static void createCategoriesTree(CategoryTreeItem *rootHolder);
    static void createCategoriesTree(TreeItem *rootItem);
    static bool moveNode(int nodeId, int parentId);
    static bool moveBeforeNode(int nodeId, int siblingId);
    static bool setCategoryName(int nodeId, const QString &name);
    static QStringList getPath(int nodeId);
    static CategoryTreeItem* insertCategoryAtEnd(const QString &name, const QString &description, int parentId);
    static int insertCategoryAtEnd(const QVector<QVariant> & data, int parentId);
    static bool insertAtEnd(TreeItem * item);
    static bool reload(TreeItem * item);
    static bool update(TreeItem * item);
    static bool remove(int nodeId, int newParent);
    static bool setPartCategory(const QVector<int> & parts, int parentCategoryId);
    static QVector<QVariant> listChildCategories(int nodeId);
private:

};

}  //namespace

#endif // CATEGORIESDAO_H
