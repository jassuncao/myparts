#include "quickaddcapacitordialog.h"
#include "ui_quickaddcapacitordialog.h"
#include "widgets/parametervaluevalidator.h"
#include "models/modelsrepository.h"
#include "models/categorytreemodel.h"
#include "models/storagetreemodel.h"
#include "models/partconditionmodel.h"
#include "models/partssqltablemodel.h"
#include "models/partparametertablemodel.h"
#include "models/partstocktablemodel.h"
#include "models/basicentitytablemodel.h"
#include "widgets/unitformatter.h"
#include "widgets/unitparser.h"
#include "widgets/savebuttonhelper.h"
#include "utils.h"

#include <QSettings>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QSqlRecord>
#include <math.h>
#include <QKeyEvent>
#include <QShortcut>

#ifdef __MINGW32__

inline double pow10(int x){
    return pow(10, x);
}

#endif

static const double MULTIPLIERS[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 0.01, 0.1};

QuickAddCapacitorDialog::QuickAddCapacitorDialog(ModelsRepository *modelsProvider, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickAddCapacitorDialog),
    _partsModel(modelsProvider->partsModel())
{
    ui->setupUi(this);
    ui->messageWidget->hide();

    _saveButtonHelper = new SaveButtonHelper(this);

    QSettings settings;
    _capacitorNameTemplate = settings.value("capacitor_template", tr("Capacitor %1 %2 (%3)")).toString();

    _capacitanceParam = PartParameterTableModel::findParameter("capacitance");
    _voltageRatingParam = PartParameterTableModel::findParameter("voltage_rating_dc");
    _toleranceParam = PartParameterTableModel::findParameter("capacitance_tolerance");
    _partParams = new PartParameterTableModel(this);
    _partStockModel = PartStockTableModel::createNew(this);

    //XXX: Disabled the validator to allow "auto complete" (ctrl+space) to work
    //ui->capacitanceValueLineEdit->setValidator(new ParameterValueValidator(_capacitanceParam.unitSymbol(), this));
    QShortcut *sc = new QShortcut(QKeySequence("Ctrl+Space"), this);
    connect(sc, SIGNAL(activated()), this, SLOT(slotAttemptAutoComplete()));
    //ui->capacitanceValueLineEdit->installEventFilter(this);
    ui->voltageRatingLineEdit->setValidator(new ParameterValueValidator(_voltageRatingParam.unitSymbol(), this));
    ui->partCategoryComboBox->setModel(modelsProvider->partCategoryModel());
    ui->partCategoryComboBox->setMinimumContentsLength(22);
    ui->partCategoryComboBox->setMaxVisibleItems(20);

    ui->partStorageComboBox->setModel(modelsProvider->partStorageModel());
    ui->partStorageComboBox->setMinimumContentsLength(22);
    ui->partStorageComboBox->setMaxVisibleItems(20);

    ui->partConditionComboBox->setModel(modelsProvider->partConditionModel());
    ui->partConditionComboBox->setModelKeyColumn(PartConditionModel::ColumnId);
    ui->partConditionComboBox->setModelColumn(PartConditionModel::ColumnValue);
    int defaultConditionRow = modelsProvider->partConditionModel()->findDefaultValueRow();
    ui->partConditionComboBox->setCurrentIndex(defaultConditionRow);

    ui->partPackageComboBox->setModel(modelsProvider->packageModel());
    ui->partPackageComboBox->setModelKeyColumn(PackageTableModel::ColumnId);
    ui->partPackageComboBox->setModelColumn(PackageTableModel::ColumnName);

    QPushButton * resetButton = ui->buttonBox->button(QDialogButtonBox::Reset);
    connect(resetButton, SIGNAL(clicked(bool)), this, SLOT(slotReset()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAddCapacitor()));
    connect(ui->messageWidget, SIGNAL(hideAnimationFinished()), ui->messageWidget, SLOT(hide()));

    _saveButtonHelper->monitor(ui->capacitanceValueLineEdit);
    _saveButtonHelper->monitor(ui->voltageRatingLineEdit);
    _saveButtonHelper->monitor(ui->toleranceLineEdit);
    _saveButtonHelper->monitor(ui->partCategoryComboBox);
    _saveButtonHelper->monitor(ui->partPackageComboBox);
    _saveButtonHelper->monitor(ui->partStorageComboBox);
    _saveButtonHelper->monitor(ui->partConditionComboBox);
    _saveButtonHelper->monitor(ui->quantitySpinBox);
    _saveButtonHelper->setButtonBox(ui->buttonBox, QDialogButtonBox::Save);
}

QuickAddCapacitorDialog::~QuickAddCapacitorDialog()
{
    delete ui;
}

static void resetCombo(QComboBox * combo)
{
    bool oldState = combo->blockSignals(true);
    combo->setCurrentIndex(0);
    combo->blockSignals(oldState);
}

void QuickAddCapacitorDialog::slotReset()
{
    resetCombo(ui->partPackageComboBox);
    ui->capacitanceValueLineEdit->clear();
    ui->voltageRatingLineEdit->clear();
    ui->toleranceLineEdit->clear();
    ui->quantitySpinBox->clear();
    _saveButtonHelper->reset();
    ui->capacitanceValueLineEdit->setFocus();
}

void QuickAddCapacitorDialog::slotAddCapacitor()
{   bool capacitanceValid=false;
    bool voltageRatingValid=false;
    double capacitance;
    double voltageRating;

    QString capacitanceStr = ui->capacitanceValueLineEdit->text();
    QString voltageStr = ui->voltageRatingLineEdit->text();
    QString toleranceStr = ui->toleranceLineEdit->text();

    if(capacitanceStr.isEmpty()){
        showError(tr("The capacitance value is required."));
        ui->capacitanceValueLineEdit->setFocus();
        return;
    }

    capacitance = UnitParser::parseUnit(capacitanceStr, _capacitanceParam.unitSymbol(), &capacitanceValid);
    if(capacitanceValid){
        capacitanceStr = UnitFormatter::format(capacitance, _capacitanceParam.unitSymbol());
    }
    else {
        qWarning("Capacitance value is invalid %s", qPrintable(capacitanceStr));
        showError(tr("MyParts failed to recognize the capacitance value."));
        ui->capacitanceValueLineEdit->setFocus();
        return;
    }

    if(!voltageStr.isEmpty()){
        voltageRating = UnitParser::parseUnit(voltageStr, _voltageRatingParam.unitSymbol(), &voltageRatingValid);
        if(voltageRatingValid){
            voltageStr = UnitFormatter::format(voltageRating, _voltageRatingParam.unitSymbol());
        }
        else {
            qWarning("Voltage rating value is invalid %s", qPrintable(voltageStr));
            showError(tr("MyParts failed to recognize the voltage rating value."));
            ui->voltageRatingLineEdit->setFocus();
            return;
        }
    }

    if(capacitanceValid){
        _partParams->appendParameter(_capacitanceParam.id(), capacitance);
    }
    if(voltageRatingValid){
        _partParams->appendParameter(_voltageRatingParam.id(), voltageRating);
    }
    if(!toleranceStr.isEmpty()){
        _partParams->appendParameter(_toleranceParam.id(), toleranceStr);
    }

    QVariant createDate = QVariant(QDateTime::currentDateTimeUtc().toTime_t());
    QString partName = _capacitorNameTemplate.arg(capacitanceStr, voltageStr, toleranceStr);
    QVariant category = selectedCategory();
    QVariant storage = selectedStorage();
    QVariant condition = selectedCondition();
    QVariant package = selectedPackage();
    int quantity = ui->quantitySpinBox->value();
    QSqlRecord initialData = _partsModel->record();

    initialData.setValue(PartsSqlTableModel::ColumnName, partName);
    initialData.setValue(PartsSqlTableModel::ColumnConditionId, condition);
    initialData.setValue(PartsSqlTableModel::ColumnPartUnitId, 1);//XXX
    initialData.setValue(PartsSqlTableModel::ColumnCategoryId, category);
    initialData.setValue(PartsSqlTableModel::ColumnStorageId, storage);
    initialData.setValue(PartsSqlTableModel::ColumnCreateDate, createDate);
    initialData.setValue(PartsSqlTableModel::ColumnActualStock, quantity);
    initialData.setValue(PartsSqlTableModel::ColumnPackageId, package);

    int newRow = _partsModel->rowCount();
    bool success = _partsModel->insertRecord(newRow, initialData);
    if(success) {
        _partsModel->database().transaction();
        if(_partsModel->submitAll()){
            _partParams->setCurrentPartId(_partsModel->lastInsertedId());
            _partParams->submitAll();
            if(quantity != 0){
                _partStockModel->appendRow(quantity, QVariant(), QString());
                _partStockModel->setCurrentPartId(_partsModel->lastInsertedId());
                _partStockModel->submitAll();
            }
            _partsModel->database().commit();
            saveSuccessFeedback();
            slotReset();
        }
        else{
            qWarning()<<"Failed to add part: Reason: " << _partsModel->lastError();
            _partsModel->database().rollback();
            Utils::reportDatabaseError(this, tr("Add part error"),
                                       tr("MyParts failed to save your part."),
                                       _partsModel->lastError());
        }
        _partParams->setCurrentPartId(QVariant());
        _partParams->select();
    }
    else {
        qWarning("Failed to insert new part");
    }    
}

QVariant QuickAddCapacitorDialog::selectedCategory() const
{
    TreeItemModel* model = static_cast<TreeItemModel*>(ui->partCategoryComboBox->model());
    QModelIndex currIndex = ui->partCategoryComboBox->view()->currentIndex();
    int itemId;
    if(currIndex.isValid()){
        itemId = model->getItemId(currIndex);
    }
    else{
        itemId = model->rootItemId();
    }
    return QVariant(itemId);
}

void QuickAddCapacitorDialog::setSelectedCategory(const QVariant & category)
{
    QModelIndex categoryIndex;
    if(category.isValid()){
        TreeItemModel * viewModel = static_cast<TreeItemModel*>(ui->partCategoryComboBox->model());
        categoryIndex = viewModel->findIndex(category.toInt());
    }
    ui->partCategoryComboBox->setCurrentIndex(categoryIndex);
}


QVariant QuickAddCapacitorDialog::selectedStorage() const
{
    TreeItemModel* model = static_cast<TreeItemModel*>(ui->partStorageComboBox->model());
    QModelIndex currIndex = ui->partStorageComboBox->view()->currentIndex();
    int itemId;
    if(currIndex.isValid()){
        itemId = model->getItemId(currIndex);
    }
    else{
        itemId = model->rootItemId();
    }
    return QVariant(itemId);
}

void QuickAddCapacitorDialog::setSelectedStorage(const QVariant & storage)
{
    QModelIndex storageIndex;
    if(storage.isValid()){
        TreeItemModel * viewModel = static_cast<TreeItemModel*>(ui->partStorageComboBox->model());
        storageIndex = viewModel->findIndex(storage.toInt());
    }
    ui->partStorageComboBox->setCurrentIndex(storageIndex);
}

QVariant QuickAddCapacitorDialog::selectedCondition() const
{
    return ui->partConditionComboBox->currentKey();
}

QVariant QuickAddCapacitorDialog::selectedPackage() const
{
    return ui->partPackageComboBox->currentKey();
}

void QuickAddCapacitorDialog::showError(const QString & errorMessage)
{
    ui->messageWidget->setText(errorMessage);
    ui->messageWidget->setMessageType(KMessageWidget::Error);
    ui->messageWidget->animatedShow();
}

void QuickAddCapacitorDialog::showSuccess(const QString& successMessage)
{
    ui->messageWidget->setText(successMessage);
    ui->messageWidget->setMessageType(KMessageWidget::Positive);
    ui->messageWidget->animatedShow();
    QTimer::singleShot(2000, ui->messageWidget, SLOT(animatedHide()));
}


void QuickAddCapacitorDialog::saveSuccessFeedback()
{
}

/*
bool QuickAddCapacitorDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->capacitanceValueLineEdit && event->type() == QEvent::KeyPress ) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if(ke->key() == Qt::Key_Space &&  ke->modifiers() & Qt::ControlModifier){
            attemptAutoComplete();
        }
    }
    return QWidget::eventFilter(obj, event);
}
*/

QString lookupTolerance(const char c){
    //Source https://wiki.xtronics.com/index.php/Capacitor_Codes
    switch (c) {
    case 'B':
        return QString("+/- 0.10pF");
    case 'C':
        return QString("+/- 0.25pF");
    case 'D':
        return QString("+/- 0.5pF");
    case 'E':
        return QString("+/- 0.5%");
    case 'F':
        return QString("+/- 1%");
    case 'G':
        return QString("+/- 2%");
    case 'H':
        return QString("+/- 3%");
    case 'J':
        return QString("+/- 5%");
    case 'K':
        return QString("+/- 10%");
    case 'M':
        return QString("+/- 20%");
    case 'N':
        return QString("+/- 30%");
    case 'P':
        return QString("+100% ,-0%");
    case 'Z':
        return QString("+80%, -20%");
    default:
        return QString();
    }
}

inline char to_char(QChar c){
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return c.toLatin1();
#else
    return c.toAscii();
#endif
}

void QuickAddCapacitorDialog::slotAttemptAutoComplete()
{
    QString text = ui->capacitanceValueLineEdit->text().trimmed();
    //We accept 3 digit value codes with an optional tolerance code
    if(text.length()==3 || text.length()==4){
        QString multiplicandStr = text.left(2);
        QChar multiplierChar = text.at(2);
        bool multiplicandOk;
        int multiplicand = multiplicandStr.toInt(&multiplicandOk);        
        if(multiplicandOk && multiplierChar.isDigit()){
            double capacitance = multiplicand * pow10(-12);
            int multiplierIdx = to_char(multiplierChar) - '0';
            if(multiplierIdx >= 0 && multiplierIdx <= 9){
                capacitance = capacitance * MULTIPLIERS[multiplierIdx];
            }
            QString capacitanceStr = UnitFormatter::format(capacitance, _capacitanceParam.unitSymbol());
            ui->capacitanceValueLineEdit->setText(capacitanceStr);
        }
        if(text.length()==4){
            QChar toleranceChar = text.at(3);
            if(toleranceChar.isLetter()){
                QString tolerance = lookupTolerance(to_char(toleranceChar));
                if(!tolerance.isEmpty()){
                    ui->toleranceLineEdit->setText(tolerance);
                }
            }
        }
    }

}
