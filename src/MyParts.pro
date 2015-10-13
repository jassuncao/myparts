#-------------------------------------------------
#
# Project created by QtCreator 2014-04-24T18:09:35
#
#-------------------------------------------------

QT       += core gui sql testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyParts
TEMPLATE = app

win32:DQUEST_INCLUDE_PATH=$$quote(F:\MyProjects\QtApps\dquest-0.2\src\dquest.pri)
unix:DQUEST_INCLUDE_PATH=$$quote(/home/jassuncao/MyProjects/dquest-0.2/src/dquest.pri)

include($$DQUEST_INCLUDE_PATH)


SOURCES += main.cpp\
        mainwindow.cpp \
    partdialog.cpp \
    partmanagerform.cpp \
    entities/categoriesdao.cpp \
    category/categorytreeitem.cpp \
    category/categorytreemodel.cpp \
    category/categoymimedata.cpp \
    entities/entities.cpp \
    widgets/customtreeview.cpp \
    partcategorydialog.cpp \
    models/treeitem.cpp \
    category/partunitdialog.cpp \
    widgets/booleanitemdelegate.cpp \
    partunits/partunitssqltablemodel.cpp \
    widgets/partunitswidget.cpp \
    optionsdialog.cpp \
    widgets/partconditionwidget.cpp \
    qsortfiltersqlquerymodel.cpp \
    widgets/qsearchlineedit.cpp \
    partmimedata.cpp \
    widgets/partstableview.cpp \
    widgets/qpartsfiltermodel.cpp \
    parts/partssqlquerymodel.cpp \
    widgets/datetimedelegate.cpp \
    parts/partssqlquerymodel2.cpp \
    addstockdialog.cpp \
    parts/partsdao.cpp \
    removestockdialog.cpp \
    parts/stockhistoryproxymodel.cpp \
    parts/stockhistorymodel.cpp \
    widgets/stockactioncolumndelegate.cpp \
    widgets/qtreebox.cpp \
    models/partconditionsqltablemodel.cpp \
    widgets/partstoragewidget.cpp \
    quickaddstoragedialog.cpp \
    widgets/currencydelegate.cpp \
    widgets/optionswidget.cpp \
    models/treeitemmodel.cpp \
    models/storagetreemodel.cpp \
    entities/storagedao.cpp \
    widgets/treeviewcombobox.cpp \
    models/treemodelmimedata.cpp \
    partsfilterform.cpp \
    filterarguments.cpp \
    partdetailsview.cpp \
    storagemanagerform.cpp \
    storagemanagerwidget.cpp \
    models/footprinttreemodel.cpp \
    footprintmanagerwidget.cpp \
    footprintdetailswidget.cpp \
    widgets/qtabwidget2.cpp \
    unitmanagerwidget.cpp \
    unitdetailswidget.cpp \
    widgets/checkedheader.cpp \
    models/unittablemodel.cpp \
    models/footprinttablemodel.cpp \
    widgets/parametervaluevalidator.cpp \
    widgets/unitparser.cpp \
    widgets/unitformatter.cpp \
    widgets/parametervaluedelegate.cpp \
    widgets/relationalcombobox.cpp \
    distributormanagerwidget.cpp \
    distributordetailswidget.cpp \
    utils.cpp \
    attachmentselectiondialog.cpp \
    valuewithunit.cpp \
    parametervalue.cpp \
    models/customtablemodel.cpp

HEADERS  += mainwindow.h \
    entities/entities.h \
    partdialog.h \
    partmanagerform.h \
    entities/categoriesdao.h \
    category/categorytreeitem.h \
    category/categorytreemodel.h \
    category/categoymimedata.h \
    widgets/customtreeview.h \
    partcategorydialog.h \
    models/treeitem.h \
    category/partunitdialog.h \
    widgets/booleanitemdelegate.h \
    partunits/partunitssqltablemodel.h \
    widgets/partunitswidget.h \
    optionsdialog.h \
    widgets/partconditionwidget.h \
    qsortfiltersqlquerymodel.h \
    widgets/qsearchlineedit.h \
    partmimedata.h \
    widgets/partstableview.h \
    widgets/qpartsfiltermodel.h \
    parts/partssqlquerymodel.h \
    widgets/datetimedelegate.h \
    parts/partssqlquerymodel2.h \
    addstockdialog.h \
    parts/partsdao.h \
    removestockdialog.h \
    parts/stockhistoryproxymodel.h \
    parts/stockhistorymodel.h \
    widgets/stockactioncolumndelegate.h \
    widgets/qtreebox.h \
    models/partconditionsqltablemodel.h \
    widgets/partstoragewidget.h \
    quickaddstoragedialog.h \
    widgets/currencydelegate.h \
    widgets/optionswidget.h \
    models/treeitemmodel.h \
    models/storagetreemodel.h \
    entities/storagedao.h \
    widgets/treeviewcombobox.h \
    models/treemodelmimedata.h \
    partsfilterform.h \
    filterarguments.h \
    partdetailsview.h \
    storagemanagerform.h \
    storagemanagerwidget.h \
    models/footprinttreemodel.h \
    footprintmanagerwidget.h \
    footprintdetailswidget.h \
    widgets/qtabwidget2.h \
    unitmanagerwidget.h \
    unitdetailswidget.h \
    widgets/checkedheader.h \
    models/unittablemodel.h \
    models/footprinttablemodel.h \
    widgets/parametervaluevalidator.h \
    widgets/unitparser.h \
    widgets/parametervaluedelegate.h \
    widgets/relationalcombobox.h \
    distributormanagerwidget.h \
    distributordetailswidget.h \
    utils.h \
    attachmentselectiondialog.h \
    valuewithunit.h \
    parametervalue.h \
    models/customtablemodel.h

FORMS    += mainwindow.ui \
    partdialog.ui \
    partmanagerform.ui \
    partunitsdialog.ui \
    addstockdialog.ui \
    removestockdialog.ui \
    dialog.ui \
    partsfilterform.ui \
    partdetailsview.ui \
    storagemanagerform.ui \
    testform1.ui \
    attachmentselectiondialog.ui \
    testform2.ui \
    optionsdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    todo.txt

#unix:!macx: LIBS += -L$$PWD/../../../lixo/qt-manhattan-style/build/ -lqt-manhattan-style

