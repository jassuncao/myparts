#ifndef COMPANYEDITORWIDGET_H
#define COMPANYEDITORWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QString>
#include "abstracteditor.h"

class QLineEdit;
class QTextEdit;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapper;

class CompanyEditorWidget : public AbstractEditor
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnWebsite,
        ColumnComment
    };

    explicit CompanyEditorWidget(QWidget *parent = 0);
    ~CompanyEditorWidget();
    QModelIndex currentModelIndex() const;
    void setCurrentModelIndex(const QModelIndex & modelIndex);
    void setModel(QAbstractItemModel * model);    
    bool validate();
public slots:
    void submit();
    void revert();
private slots:
private:    
    QLineEdit * _nameLineEdit;
    QLineEdit * _websiteLineEdit;
    QTextEdit * _commentTextEdit;      
    QDataWidgetMapper * _mapper;    
    QModelIndex _currentIndex;
};

#endif // COMPANYEDITORWIDGET_H
