#ifndef COMPANYEDITORWIDGET_H
#define COMPANYEDITORWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QString>
#include "core/ui/abstracteditor.h"

class QLineEdit;
class QPlainTextEdit;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapper;

class CompanyEditorWidget : public AbstractEditor
{
    Q_OBJECT
public:
//    enum ColumnsIndex {
//        ColumnId = 0,
//        ColumnName,
//        ColumnWebsite,
//        ColumnComment
//    };

    explicit CompanyEditorWidget(QWidget *parent = 0);
    ~CompanyEditorWidget();
    QModelIndex currentModelIndex() const;
    //void setCurrentModelIndex(const QModelIndex & modelIndex);
    void setModel(QAbstractItemModel * model);    
    void setCurrentIndex(int row);
    int currentIndex() const;
    bool validate();
public slots:
    void submit();
    void revert();
private slots:
private:    
    QLineEdit * _nameLineEdit;
    QLineEdit * _websiteLineEdit;
    QPlainTextEdit * _commentTextEdit;
    QDataWidgetMapper * _mapper;      
};

#endif // COMPANYEDITORWIDGET_H
