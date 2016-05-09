#-------------------------------------------------
#
# Project created by QtCreator 2014-04-24T18:09:35
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyParts
TEMPLATE = app

win32:DQUEST_INCLUDE_PATH=$$quote(F:\MyProjects\QtApps\dquest-0.2\src\dquest.pri)
unix:DQUEST_INCLUDE_PATH=$$quote(/home/jassuncao/MyProjects/dquest-0.2/src/dquest.pri)

include($$DQUEST_INCLUDE_PATH)


SOURCES += main.cpp\
        mainwindow.cpp \
    partdialog.cpp \
    entities/entities.cpp \
    dialogs/partcategorydialog.cpp \
    models/treeitem.cpp \
    widgets/booleanitemdelegate.cpp \
    models/partunitssqltablemodel.cpp \
    optionsdialog.cpp \
    qsortfiltersqlquerymodel.cpp \
    widgets/qsearchlineedit.cpp \
    partmimedata.cpp \
    widgets/partstableview.cpp \
    widgets/qpartsfiltermodel.cpp \
    widgets/datetimedelegate.cpp \
    dialogs/addstockdialog.cpp \
    dialogs/removestockdialog.cpp \
    widgets/stockactioncolumndelegate.cpp \
    widgets/qtreebox.cpp \
    widgets/partstoragewidget.cpp \
    dialogs/quickaddstoragedialog.cpp \
    widgets/currencydelegate.cpp \
    models/treeitemmodel.cpp \
    models/storagetreemodel.cpp \
    widgets/treeviewcombobox.cpp \
    models/treemodelmimedata.cpp \
    partdetailsview.cpp \
    storagemanagerform.cpp \
    packagedetailswidget.cpp \
    widgets/qtabwidget2.cpp \
    models/unittablemodel.cpp \
    widgets/parametervaluevalidator.cpp \
    widgets/unitparser.cpp \
    widgets/unitformatter.cpp \
    widgets/parametervaluedelegate.cpp \
    widgets/relationalcombobox.cpp \
    distributormanagerwidget.cpp \
    distributordetailswidget.cpp \
    utils.cpp \
    dialogs/attachmentselectiondialog.cpp \
    valuewithunit.cpp \
    parametervalue.cpp \
    models/customtablemodel.cpp \
    widgets/validatingitemdelegate.cpp \
    widgets/navigator.cpp \
    widgets/navigationsubwidget.cpp \
    partsmanagerview.cpp \
    widgets/navigationtreeview.cpp \
    distributorsmanagerview.cpp \
    widgets/itemstreeview.cpp \
    widgets/listnavigatorwidget.cpp \
    editors/companyeditorwidget.cpp \
    editors/abstracteditor.cpp \
    editormanagerview.cpp \
    widgets/flowlayout.cpp \
    widgets/qdualpushbutton.cpp \
    widgets/filteritemwidget.cpp \
    widgets/partsfilterwidget.cpp \
    widgets/qactionpushbutton.cpp \
    models/extrarowproxymodel.cpp \
    models/partssqltablemodel.cpp \
    widgets/gridwidget.cpp \
    models/partsquerybuilder.cpp \
    models/packagetablemodel.cpp \
    editors/packageeditorwidget.cpp \
    packagemanagerwidget.cpp \
    widgets/extendedtableview.cpp \
    models/treeitemmodelpersistence.cpp \
    dialogs/treeitemeditdialog.cpp \
    models/categorytreemodel.cpp \
    models/partstocktablemodel.cpp \
    models/stocktableformatproxymodel.cpp \
    widgets/elidinglabel.cpp \
    models/partstableproxymodel.cpp \
    widgets/stockinlinedelegate.cpp \
    dialogs/quickaddresistordialog.cpp \
    widgets/colorbandcombobox.cpp \
    models/modelsprovider.cpp \
    models/partunitsreadonlymodel.cpp \
    models/partconditionmodel.cpp \
    models/partparametertablemodel.cpp \
    models/parameter.cpp \
    widgets/comboitemdelegate.cpp \
    models/parametermodel.cpp

HEADERS  += mainwindow.h \
    entities/entities.h \
    partdialog.h \
    dialogs/partcategorydialog.h \
    models/treeitem.h \
    widgets/booleanitemdelegate.h \
    models/partunitssqltablemodel.h \
    optionsdialog.h \
    qsortfiltersqlquerymodel.h \
    widgets/qsearchlineedit.h \
    partmimedata.h \
    widgets/partstableview.h \
    widgets/qpartsfiltermodel.h \
    widgets/datetimedelegate.h \
    dialogs/addstockdialog.h \
    dialogs/removestockdialog.h \
    widgets/stockactioncolumndelegate.h \
    widgets/qtreebox.h \
    widgets/partstoragewidget.h \
    dialogs/quickaddstoragedialog.h \
    widgets/currencydelegate.h \
    models/treeitemmodel.h \
    models/storagetreemodel.h \
    widgets/treeviewcombobox.h \
    models/treemodelmimedata.h \
    partdetailsview.h \
    storagemanagerform.h \
    packagedetailswidget.h \
    widgets/qtabwidget2.h \
    models/unittablemodel.h \
    widgets/parametervaluevalidator.h \
    widgets/unitparser.h \
    widgets/parametervaluedelegate.h \
    widgets/relationalcombobox.h \
    distributormanagerwidget.h \
    distributordetailswidget.h \
    utils.h \
    dialogs/attachmentselectiondialog.h \
    valuewithunit.h \
    parametervalue.h \
    models/customtablemodel.h \
    widgets/validatingitemdelegate.h \
    constants.h \
    widgets/navigator.h \
    widgets/navigationsubwidget.h \
    partsmanagerview.h \
    widgets/navigationtreeview.h \
    distributorsmanagerview.h \
    widgets/itemstreeview.h \
    widgets/listnavigatorwidget.h \
    editors/companyeditorwidget.h \
    editors/abstracteditor.h \
    editormanagerview.h \
    widgets/flowlayout.h \
    widgets/qdualpushbutton.h \
    widgets/filteritemwidget.h \
    widgets/partsfilterwidget.h \
    widgets/qactionpushbutton.h \
    models/extrarowproxymodel.h \
    models/partssqltablemodel.h \
    widgets/gridwidget.h \
    models/partsquerybuilder.h \
    models/packagetablemodel.h \
    editors/packageeditorwidget.h \
    packagemanagerwidget.h \
    widgets/extendedtableview.h \
    models/treeitemmodelpersistence.h \
    dialogs/treeitemeditdialog.h \
    models/categorytreemodel.h \
    models/partstocktablemodel.h \
    models/stocktableformatproxymodel.h \
    widgets/elidinglabel.h \
    models/partstableproxymodel.h \
    widgets/stockinlinedelegate.h \
    dialogs/quickaddresistordialog.h \
    widgets/colorbandcombobox.h \
    models/modelsprovider.h \
    models/partunitsreadonlymodel.h \
    models/partconditionmodel.h \
    models/partparametertablemodel.h \
    models/parameter.h \
    widgets/comboitemdelegate.h \
    models/modelwithforeignkey.h \
    models/parametermodel.h

FORMS    += mainwindow.ui \
    partdialog.ui \
    partunitsdialog.ui \
    dialogs/addstockdialog.ui \
    dialogs/removestockdialog.ui \
    dialog.ui \
    partdetailsview.ui \
    storagemanagerform.ui \
    testform1.ui \
    dialogs/attachmentselectiondialog.ui \
    testform2.ui \
    optionsdialog.ui \
    dialogs/quickaddresistordialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    todo.txt

#unix:!macx: LIBS += -L$$PWD/../../../lixo/qt-manhattan-style/build/ -lqt-manhattan-style

INCLUDEPATH += $$PWD/../../qt-manhattan-style
DEPENDPATH += $$PWD/../../qt-manhattan-style

unix:!macx: LIBS += -L$$PWD/../../build-qt-manhattan-style-Desktop-Debug/ -lqt-manhattan-style
unix:!macx: PRE_TARGETDEPS += $$PWD/../../build-qt-manhattan-style-Desktop-Debug/libqt-manhattan-style.a

win32: LIBS += -L$$PWD/../../build-qt-manhattan-style-Desktop_Qt_5_2_1_MinGW_32bit-Debug/debug/ -lqt-manhattan-style
win32-g++: PRE_TARGETDEPS += $$PWD/../../build-qt-manhattan-style-Desktop_Qt_5_2_1_MinGW_32bit-Debug/debug/libqt-manhattan-style.a

#unix:!macx: LIBS += -lwwwidgets4

DISTFILES +=
