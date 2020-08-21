#-------------------------------------------------
#
# Project created by QtCreator 2014-04-24T18:09:35
#
#-------------------------------------------------

include(../defaults.pri)

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyParts
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    attachment/attachmenttablemodel.cpp \
    part/model/partdistributortablemodel.cpp \
    part/model/partmanufacturertablemodel.cpp \
    part/ui/partdialog.cpp \
    core/treemodel/treeitem.cpp \
    widgets/booleanitemdelegate.cpp \
    optionsdialog.cpp \    
    part/partmimedata.cpp \
    widgets/partstableview.cpp \
    widgets/qpartsfiltermodel.cpp \
    widgets/datetimedelegate.cpp \
    stock/addstockdialog.cpp \
    dialogs/removestockdialog.cpp \
    widgets/stockactioncolumndelegate.cpp \
    widgets/qtreebox.cpp \
    widgets/partstoragewidget.cpp \
    widgets/currencydelegate.cpp \
    core/treemodel/treeitemmodel.cpp \
    part/model/storagetreemodel.cpp \
    widgets/treeviewcombobox.cpp \
    core/treemodel/treemodelmimedata.cpp \
    part/ui/partdetailsview.cpp \
    widgets/qtabwidget2.cpp \
    unit/unittablemodel.cpp \
    parameter/parametervaluevalidator.cpp \
    unit/unitparser.cpp \
    unit/unitformatter.cpp \
    parameter/parametervaluedelegate.cpp \
    widgets/relationalcombobox.cpp \
    utils.cpp \
    attachment/attachmentselectiondialog.cpp \
    core/valuewithunit.cpp \
    parameter/parametervalue.cpp \
    core/sql/customtablemodel.cpp \
    widgets/validatingitemdelegate.cpp \
    widgets/navigator.cpp \
    widgets/navigationsubwidget.cpp \
    part/ui/partsmanagerview.cpp \
    widgets/navigationtreeview.cpp \
    widgets/itemstreeview.cpp \
    widgets/listnavigatorwidget.cpp \
    editors/companyeditorwidget.cpp \
    core/ui/abstracteditor.cpp \
    editormanagerview.cpp \
    widgets/flowlayout.cpp \
    widgets/qdualpushbutton.cpp \
    widgets/filteritemwidget.cpp \
    widgets/partsfilterwidget.cpp \
    widgets/qactionpushbutton.cpp \
    part/model/partssqltablemodel.cpp \
    widgets/gridwidget.cpp \
    part/model/partsquerybuilder.cpp \
    editors/packageeditorwidget.cpp \
    widgets/extendedtableview.cpp \
    core/treemodel/treeitemmodelpersistence.cpp \
    core/treemodel/treeitemeditdialog.cpp \
    part/model/categorytreemodel.cpp \
    models/stocktableformatproxymodel.cpp \
    widgets/elidinglabel.cpp \
    part/model/partstableproxymodel.cpp \
    widgets/stockinlinedelegate.cpp \
    part/generator/quickaddresistordialog.cpp \
    widgets/colorbandcombobox.cpp \
    part/model/partconditionmodel.cpp \
    part/model/partparametertablemodel.cpp \
    parameter/parameter.cpp \
    widgets/comboitemdelegate.cpp \
    parameter/parametermodel.cpp \
    models/databasehelper.cpp \
    widgets/kmessagewidget.cpp \
    parameter/parameterunitmodel.cpp \
    models/modelsrepository.cpp \
    util/proxymodelnoneentry.cpp \
    core/sql/basicentitytablemodel.cpp \
    dialogs/multistoragedialog.cpp \
    core/iconsrepository.cpp \
    part/generator/quickaddcapacitordialog.cpp \
    core/csv/qxtcsvmodel.cpp \
    core/treemodel/treeproxyfilter.cpp \
    dialogs/imageviewer.cpp \
    dialogs/tableexportdialog.cpp \
    core/csv/csvexporter.cpp \
    project/model/projecttablemodel.cpp \
    project/projecteditorwidget.cpp \
    attachment/attachmentstableview.cpp \
    widgets/partpicker.cpp \
    core/sql/cachedsqltablemodel.cpp \
    project/model/projectparttablemodel.cpp \
    util/alignmentproxymodel.cpp \
    part/generator/smdresistorcalculator.cpp \
    widgets/savebuttonhelper.cpp \
    widgets/hintlineedit.cpp \
    core/price.cpp \    
    widgets/priceitemdelegate.cpp \
    core/currencyhelper.cpp \
    stock/model/partstocktablemodel2.cpp \
    stock/addstockentrydialog.cpp \
    stock/model/partstocklogtablemodel.cpp \
    dialogs/movestockdialog.cpp \
    stock/partstockmimedata.cpp \
    part/merge/mergepartsdialog.cpp \
    part/merge/mergehelper.cpp \
    models/partunitcache.cpp \
    widgets/quantitydelegate.cpp

HEADERS  += mainwindow.h \
    attachment/attachmenttablemodel.h \
    part/model/partdistributortablemodel.h \
    part/model/partmanufacturertablemodel.h \
    part/ui/partdialog.h \
    core/treemodel/treeitem.h \
    widgets/booleanitemdelegate.h \
    optionsdialog.h \    
    part/partmimedata.h \
    widgets/partstableview.h \
    widgets/qpartsfiltermodel.h \
    widgets/datetimedelegate.h \
    stock/addstockdialog.h \
    dialogs/removestockdialog.h \
    widgets/stockactioncolumndelegate.h \
    widgets/qtreebox.h \
    widgets/partstoragewidget.h \
    widgets/currencydelegate.h \
    core/treemodel/treeitemmodel.h \
    part/model/storagetreemodel.h \
    widgets/treeviewcombobox.h \
    core/treemodel/treemodelmimedata.h \
    part/ui/partdetailsview.h \
    widgets/qtabwidget2.h \
    unit/unittablemodel.h \
    parameter/parametervaluevalidator.h \
    unit/unitparser.h \
    parameter/parametervaluedelegate.h \
    widgets/relationalcombobox.h \
    utils.h \
    attachment/attachmentselectiondialog.h \
    core/valuewithunit.h \
    core/parametervalue.h \
    core/sql/customtablemodel.h \
    widgets/validatingitemdelegate.h \
    constants.h \
    widgets/navigator.h \
    widgets/navigationsubwidget.h \
    part/ui/partsmanagerview.h \
    widgets/navigationtreeview.h \
    widgets/itemstreeview.h \
    widgets/listnavigatorwidget.h \
    editors/companyeditorwidget.h \
    core/ui/abstracteditor.h \
    editormanagerview.h \
    widgets/flowlayout.h \
    widgets/qdualpushbutton.h \
    widgets/filteritemwidget.h \
    widgets/partsfilterwidget.h \
    widgets/qactionpushbutton.h \
    part/model/partssqltablemodel.h \
    widgets/gridwidget.h \
    part/model/partsquerybuilder.h \
    editors/packageeditorwidget.h \
    widgets/extendedtableview.h \
    core/treemodel/treeitemmodelpersistence.h \
    core/treemodel/treeitemeditdialog.h \
    part/model/categorytreemodel.h \
    models/stocktableformatproxymodel.h \
    widgets/elidinglabel.h \
    part/model/partstableproxymodel.h \
    widgets/stockinlinedelegate.h \
    part/generator/quickaddresistordialog.h \
    widgets/colorbandcombobox.h \
    part/model/partconditionmodel.h \
    part/model/partparametertablemodel.h \
    parameter/parameter.h \
    widgets/comboitemdelegate.h \
    core/sql/modelwithforeignkey.h \
    parameter/parametermodel.h \
    stacktrace.h \
    models/databasehelper.h \
    widgets/kmessagewidget.h \
    parameter/parameterunitmodel.h \
    models/modelsrepository.h \
    util/proxymodelnoneentry.h \
    core/sql/basicentitytablemodel.h \
    dialogs/multistoragedialog.h \
    core/iconsrepository.h \
    part/generator/quickaddcapacitordialog.h \
    core/csv/qxtcsvmodel.h \
    core/treemodel/treeproxyfilter.h \
    dialogs/imageviewer.h \
    dialogs/tableexportdialog.h \
    core/csv/csvexporter.h \
    project/model/projecttablemodel.h \
    project/projecteditorwidget.h \
    attachment/attachmentstableview.h \
    widgets/partpicker.h \
    core/sql/cachedsqltablemodel.h \
    project/model/projectparttablemodel.h \
    util/alignmentproxymodel.h \
    part/generator/smdresistorcalculator.h \
    widgets/savebuttonhelper.h \
    widgets/hintlineedit.h \
    core/price.h \    
    widgets/priceitemdelegate.h \
    core/currencyhelper.h \
    stock/model/partstocktablemodel2.h \
    stock/addstockentrydialog.h \
    stock/model/partstocklogtablemodel.h \
    dialogs/movestockdialog.h \
    stock/partstockmimedata.h \
    part/merge/mergepartsdialog.h \
    part/merge/mergehelper.h \
    models/partunitcache.h \
    widgets/quantitydelegate.h

FORMS    += mainwindow.ui \
    part/ui/partdialog.ui \
    part/ui/partunitsdialog.ui \
    stock/addstockdialog.ui \
    dialogs/removestockdialog.ui \
    dialog.ui \
    part/ui/partdetailsview.ui \
    testform1.ui \
    attachment/attachmentselectiondialog.ui \
    testform2.ui \
    optionsdialog.ui \
    part/generator/quickaddresistordialog.ui \
    dialogs/multistoragedialog.ui \
    part/generator/quickaddcapacitordialog.ui \
    dialogs/tableexportdialog.ui \
    project/projecteditorform.ui \
    stock/addstockentrydialog.ui \
    dialogs/movestockdialog.ui \
    octopart/octopartimportdialog.ui \
    octopart/entityselectiondialog.ui

RESOURCES += \
    resources.qrc \
    extras.qrc

OTHER_FILES += \
    todo.txt


INCLUDEPATH += $$PWD/../qt-manhattan-style
DEPENDPATH += $$PWD/../qt-manhattan-style

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qt-manhattan-style/release/ -lqt-manhattan-style
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qt-manhattan-style/debug/ -lqt-manhattan-style
else:unix: LIBS += -L$$OUT_PWD/../qt-manhattan-style/ -lqt-manhattan-style

lessThan(QT_MAJOR_VERSION, 5){

 INCLUDEPATH += $$PWD/../qjson-backport
 DEPENDPATH += $$PWD/../qjson-backport

 win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/release/ -lqjson-backport
 else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/debug/ -lqjson-backport
 else:unix: LIBS += -L$$OUT_PWD/../qjson-backport/ -lqjson-backport

}

#unix:!macx: LIBS += -lwwwidgets4

DISTFILES += \
    SQLiteInit.sql \
    initialdata.sql \
    storage.xml \
    category.xml \
    ../res/packages.csv

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../widgets/release/ -lwidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../widgets/debug/ -lwidgets
else:unix:!macx: LIBS += -L$$OUT_PWD/../widgets/ -lwidgets

INCLUDEPATH += $$PWD/../widgets
DEPENDPATH += $$PWD/../widgets
