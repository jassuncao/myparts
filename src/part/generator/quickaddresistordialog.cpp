#include "quickaddresistordialog.h"
#include "ui_quickaddresistordialog.h"
#include "parameter/parametervaluevalidator.h"
#include "models/modelsrepository.h"
#include "part/model/categorytreemodel.h"
#include "part/model/storagetreemodel.h"
#include "part/model/partconditionmodel.h"
#include "part/model/partssqltablemodel.h"
#include "part/model/partparametertablemodel.h"
#include "stock/model/partstocklogtablemodel.h"
#include "stock/model/partstocktablemodel2.h"
#include "core/sql/basicentitytablemodel.h"
#include "unit/unitformatter.h"
#include "unit/unitparser.h"
#include "widgets/savebuttonhelper.h"
#include "utils.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStandardItemModel>
#include <QStylePainter>
#include <QKeyEvent>
#include <QStyle>
#include <QPushButton>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>
#include <QSqlRecord>
#include <QShortcut>
#include <QCompleter>

static QStandardItem * newColorItem(const QString & name, const QColor & color, const QVariant & value, const int shortcutKey)
{
    QStandardItem * item = new QStandardItem(name);  
    item->setData(color, Qt::DecorationRole);
    item->setData(shortcutKey, ColorBandComboBox::ShortcutRole);
    item->setData(value, ColorBandComboBox::ValueRole);
    return item;
}

QuickAddResistorDialog::QuickAddResistorDialog(ModelsRepository * modelsProvider, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickAddResistorDialog),
    _partsModel(modelsProvider->partsModel())
{
    ui->setupUi(this);
    ui->messageWidget->hide();

    _saveButtonHelper = new SaveButtonHelper(this);

    QSettings settings;
    _resistorNameTemplate = settings.value("resistor_template", tr("Resistor %1 %2 (%3)")).toString();

    _resistanceParam = PartParameterTableModel::findParameter("resistance");
    _powerRatingParam = PartParameterTableModel::findParameter("power_rating");
    _resistorToleranceParam = PartParameterTableModel::findParameter("resistance_tolerance");
    _partParams = new PartParameterTableModel(this);
    _partStockLogModel = PartStockLogTableModel::createNew(this);
    _partStockModel = PartStockTableModel2::createNew(this);


    QShortcut *sc = new QShortcut(QKeySequence("Ctrl+Space"), this);
    connect(sc, SIGNAL(activated()), this, SLOT(slotAttemptAutoComplete()));
    //XXX: Disabled the validator to allow "auto complete" (ctrl+space) to work
    //ui->resistorValueLineEdit->setValidator(new ParameterValueValidator(_resistanceParam.unitSymbol(), this));
    ui->resistorPowerRatingLineEdit->setValidator(new ParameterValueValidator(_powerRatingParam.unitSymbol(), true, this));
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

    QStandardItemModel * digitsBandsModel = new QStandardItemModel(11,1);
    digitsBandsModel->setItem(0,0, newColorItem(tr("None"), QColor(Qt::transparent), QVariant(), Qt::Key_Backspace));
    digitsBandsModel->setItem(1,0, newColorItem(tr("Black"), QColor("black"), QVariant(0), Qt::Key_0));
    digitsBandsModel->setItem(2,0, newColorItem(tr("Brown"), QColor("brown"), QVariant(1), Qt::Key_1));
    digitsBandsModel->setItem(3,0, newColorItem(tr("Red"), QColor("red"), QVariant(2), Qt::Key_2));
    digitsBandsModel->setItem(4,0, newColorItem(tr("Orange"), QColor("orange"), QVariant(3), Qt::Key_3));
    digitsBandsModel->setItem(5,0, newColorItem(tr("Yellow"), QColor("yellow"), QVariant(4), Qt::Key_4));
    digitsBandsModel->setItem(6,0, newColorItem(tr("Green"), QColor("green"), QVariant(5), Qt::Key_5));
    digitsBandsModel->setItem(7,0, newColorItem(tr("Blue"), QColor("blue"), QVariant(6), Qt::Key_6));
    digitsBandsModel->setItem(8,0, newColorItem(tr("Violet"), QColor("violet"), QVariant(7), Qt::Key_7));
    digitsBandsModel->setItem(9,0, newColorItem(tr("Grey"), QColor("grey"), QVariant(8), Qt::Key_8));
    digitsBandsModel->setItem(10,0, newColorItem(tr("White"), QColor("white"), QVariant(9), Qt::Key_9));

    QStandardItemModel * multiplierBandsModel = new QStandardItemModel(11,1);
    multiplierBandsModel->setItem(0,0, newColorItem(tr("None"), QColor(), QVariant(), Qt::Key_Backspace));
    multiplierBandsModel->setItem(1,0, newColorItem(tr("Black"), QColor("black"), QVariant(), Qt::Key_0));
    multiplierBandsModel->setItem(2,0, newColorItem(tr("Brown"), QColor("brown"), QVariant(10), Qt::Key_1));
    multiplierBandsModel->setItem(3,0, newColorItem(tr("Red"), QColor("red"), QVariant(100), Qt::Key_2));
    multiplierBandsModel->setItem(4,0, newColorItem(tr("Orange"), QColor("orange"), QVariant(1000), Qt::Key_3));
    multiplierBandsModel->setItem(5,0, newColorItem(tr("Yellow"), QColor("yellow"), QVariant(10000), Qt::Key_4));
    multiplierBandsModel->setItem(6,0, newColorItem(tr("Green"), QColor("green"), QVariant(100000), Qt::Key_5));
    multiplierBandsModel->setItem(7,0, newColorItem(tr("Blue"), QColor("blue"), QVariant(1000000), Qt::Key_6));
    multiplierBandsModel->setItem(8,0, newColorItem(tr("Violet"), QColor("violet"), QVariant(10000000), Qt::Key_7));
    multiplierBandsModel->setItem(9,0, newColorItem(tr("Gold"), QColor("gold"), QVariant(0.1), Qt::Key_Comma));
    multiplierBandsModel->setItem(10,0, newColorItem(tr("Silver"), QColor("silver"), QVariant(0.01), Qt::Key_Colon));

    QStandardItemModel * toleranceBandsModel = new QStandardItemModel(8,1);
    toleranceBandsModel->setItem(0,0, newColorItem(tr("None"), QColor(), QVariant(20), Qt::Key_Backspace));
    toleranceBandsModel->setItem(1,0, newColorItem(tr("Brown"), QColor("brown"), QVariant(1), Qt::Key_F));
    toleranceBandsModel->setItem(2,0, newColorItem(tr("Red"), QColor("red"), QVariant(2), Qt::Key_G));
    toleranceBandsModel->setItem(3,0, newColorItem(tr("Green"), QColor("green"), QVariant(0.5), Qt::Key_D));
    toleranceBandsModel->setItem(4,0, newColorItem(tr("Blue"), QColor("blue"), QVariant(0.25), Qt::Key_C));
    toleranceBandsModel->setItem(5,0, newColorItem(tr("Violet"), QColor("violet"), QVariant(0.1), Qt::Key_B));
    toleranceBandsModel->setItem(6,0, newColorItem(tr("Gold"), QColor("gold"), QVariant(5), Qt::Key_J));
    toleranceBandsModel->setItem(7,0, newColorItem(tr("Silver"), QColor("silver"), QVariant(10), Qt::Key_K));

    ui->band1ComboBox->setModel(digitsBandsModel);
    ui->band2ComboBox->setModel(digitsBandsModel);
    ui->band3ComboBox->setModel(digitsBandsModel);
    ui->multiplierBandComboBox->setModel(multiplierBandsModel);
    ui->toleranceBandComboBox->setModel(toleranceBandsModel);
    ui->fourBandRadioButton->setChecked(true);
    ui->band3ComboBox->setVisible(false);
    connect(ui->band1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandChanged(int)));
    connect(ui->band2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandChanged(int)));
    connect(ui->band3ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandChanged(int)));
    connect(ui->multiplierBandComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandChanged(int)));
    connect(ui->toleranceBandComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotToleranceBandChanged(int)));
    connect(ui->fiveBandRadioButton, SIGNAL(toggled(bool)), this, SLOT(slotFiveBandToggled(bool)));

    QPushButton * resetButton = ui->buttonBox->button(QDialogButtonBox::Reset);
    connect(resetButton, SIGNAL(clicked(bool)), this, SLOT(slotReset()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAddResistor()));
    connect(ui->messageWidget, SIGNAL(hideAnimationFinished()), ui->messageWidget, SLOT(hide()));

    _saveButtonHelper->monitor(ui->resistorValueLineEdit);
    _saveButtonHelper->monitor(ui->resistorPowerRatingLineEdit);
    _saveButtonHelper->monitor(ui->toleranceLineEdit);
    _saveButtonHelper->monitor(ui->partCategoryComboBox);
    _saveButtonHelper->monitor(ui->partPackageComboBox);
    _saveButtonHelper->monitor(ui->partStorageComboBox);
    _saveButtonHelper->monitor(ui->partConditionComboBox);
    _saveButtonHelper->monitor(ui->quantitySpinBox);
    _saveButtonHelper->setButtonBox(ui->buttonBox, QDialogButtonBox::Save);
    readSettings();

    QSqlQueryModel *tolerancesModel = new QSqlQueryModel;
    tolerancesModel->setQuery("SELECT DISTINCT value from part_parameter INNER JOIN parameter param WHERE param.key='resistance_tolerance' ORDER BY value ASC");
    QCompleter *completer = new QCompleter(tolerancesModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->toleranceLineEdit->setCompleter(completer);
}

QuickAddResistorDialog::~QuickAddResistorDialog()
{
    delete ui;
}

void QuickAddResistorDialog::closeEvent(QCloseEvent *)
{
     writeSettings();
}

void QuickAddResistorDialog::writeSettings()
{
    int bands = ui->fiveBandRadioButton->isChecked() ? 5 : 4;
    QSettings settings;
    settings.beginGroup("QuickAddResistorDialog");
    settings.setValue("size", this->size());
    settings.setValue("bands", bands);
    settings.endGroup();

}

void QuickAddResistorDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("QuickAddResistorDialog");
    resize(settings.value("size", QSize(400, 400)).toSize());
    int bands = settings.value("bands", 4).toInt();
    settings.endGroup();
    ui->fiveBandRadioButton->setChecked(bands>4);
}

void QuickAddResistorDialog::slotFiveBandToggled(bool checked)
{
    ui->band3ComboBox->setVisible(checked);
    ui->band3ComboBox->setCurrentIndex(0);
}

static void resetCombo(QComboBox * combo)
{
    bool oldState = combo->blockSignals(true);
    combo->setCurrentIndex(0);
    combo->blockSignals(oldState);
}

void QuickAddResistorDialog::slotReset()
{
    resetCombo(ui->band1ComboBox);
    resetCombo(ui->band2ComboBox);
    resetCombo(ui->band3ComboBox);
    resetCombo(ui->multiplierBandComboBox);
    resetCombo(ui->toleranceBandComboBox);    
    ui->resistorValueLineEdit->clear();
    ui->resistorPowerRatingLineEdit->clear();
    ui->toleranceLineEdit->clear();
    ui->quantitySpinBox->clear();
    _saveButtonHelper->reset();
    ui->resistorValueLineEdit->setFocus();
}

void QuickAddResistorDialog::slotAddResistor()
{   bool resistanceValid=false;
    bool powerValid=false;
    double resistance;
    double power;

    QString resistanceStr = ui->resistorValueLineEdit->text();
    QString powerStr = ui->resistorPowerRatingLineEdit->text();
    QString toleranceStr = ui->toleranceLineEdit->text();

    if(resistanceStr.isEmpty()){
        showError(tr("The resistance value is required."));
        ui->resistorValueLineEdit->setFocus();
        return;
    }

    resistance = UnitParser::parseUnit(resistanceStr, _resistanceParam.unitSymbol(), &resistanceValid);
    if(resistanceValid){
        resistanceStr = UnitFormatter::format(resistance, _resistanceParam.unitSymbol());        
    }
    else {
        qWarning("Resistance value is invalid %s", qPrintable(resistanceStr));
        showError(tr("MyParts failed to recognize the resistance value."));
        ui->resistorValueLineEdit->setFocus();
        return;
    }    

    if(!powerStr.isEmpty()){
        power = UnitParser::parseUnit(powerStr, _powerRatingParam.unitSymbol(), &powerValid);
        if(powerValid){
            powerStr = UnitFormatter::format(power, _powerRatingParam.unitSymbol());
        }
        else {
            qWarning("Power rating value is invalid %s", qPrintable(powerStr));
            showError(tr("MyParts failed to recognize the power rating value."));
            ui->resistorPowerRatingLineEdit->setFocus();
            return;
        }
    }

    if(resistanceValid){
        _partParams->appendParameter(_resistanceParam.id(), resistance);
    }
    if(powerValid){
        _partParams->appendParameter(_powerRatingParam.id(), power);
    }
    if(!toleranceStr.isEmpty()){
        _partParams->appendParameter(_resistorToleranceParam.id(), toleranceStr);
    }

    QVariant createDate = QVariant(QDateTime::currentDateTimeUtc().toTime_t());
    QString partName = _resistorNameTemplate.arg(resistanceStr, powerStr, toleranceStr);
    QVariant category = selectedCategory();
    QVariant storage = selectedStorage();
    QVariant condition = selectedCondition();
    QVariant package = selectedPackage();
    int quantity = ui->quantitySpinBox->value();
    //Creates a new empty record;
    QSqlRecord initialData = _partsModel->record();

    initialData.setValue(PartsSqlTableModel::ColumnName, partName);
    //initialData.setValue(PartsSqlTableModel::ColumnConditionId, condition);
    initialData.setValue(PartsSqlTableModel::ColumnPartUnitId, 1);//XXX
    initialData.setValue(PartsSqlTableModel::ColumnCategoryId, category);
    //initialData.setValue(PartsSqlTableModel::ColumnStorageId, storage);
    initialData.setValue(PartsSqlTableModel::ColumnCreateDate, createDate);
    initialData.setValue(PartsSqlTableModel::ColumnTotalStock, quantity);
    initialData.setValue(PartsSqlTableModel::ColumnPackageId, package);

    int newRow = _partsModel->rowCount();
    bool success = _partsModel->insertRecord(newRow, initialData);
    if(success) {
        _partsModel->database().transaction();
        if(_partsModel->submitAll()){            
            _partParams->setCurrentPartId(_partsModel->lastInsertedId());            
            _partParams->submitAll();
            if(quantity != 0){
                QVariant partId = _partsModel->lastInsertedId();
                _partStockModel->rawInsert(partId, condition, storage, quantity);
                _partStockLogModel->rawInsert(partId, quantity, QVariant(), QString());
            }
            _partsModel->database().commit();
            //showSuccess(tr("Resistor saved!"));
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

QVariant QuickAddResistorDialog::selectedCategory() const
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

void QuickAddResistorDialog::setSelectedCategory(const QVariant & category)
{
    QModelIndex categoryIndex;
    if(category.isValid()){
        TreeItemModel * viewModel = static_cast<TreeItemModel*>(ui->partCategoryComboBox->model());
        categoryIndex = viewModel->findIndex(category.toInt());
    }
    ui->partCategoryComboBox->setCurrentIndex(categoryIndex);
}


QVariant QuickAddResistorDialog::selectedStorage() const
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

void QuickAddResistorDialog::setSelectedStorage(const QVariant & storage)
{
    QModelIndex storageIndex;
    if(storage.isValid()){
        TreeItemModel * viewModel = static_cast<TreeItemModel*>(ui->partStorageComboBox->model());
        storageIndex = viewModel->findIndex(storage.toInt());
    }
    ui->partStorageComboBox->setCurrentIndex(storageIndex);
}

QVariant QuickAddResistorDialog::selectedCondition() const
{
    return ui->partConditionComboBox->currentKey();
}

QVariant QuickAddResistorDialog::selectedPackage() const
{
    return ui->partPackageComboBox->currentKey();
}

static QVariant getBandValue(QComboBox * combo)
{
    int idx =combo->currentIndex();
    if(idx<0)
        return QVariant();
    QAbstractItemModel * model = combo->model();
    return model->index(idx, 0).data(ColorBandComboBox::ValueRole);
}

void QuickAddResistorDialog::slotBandChanged(int)
{
    QVariant band3Value;
    QVariant band1Value = getBandValue(ui->band1ComboBox);
    QVariant band2Value = getBandValue(ui->band2ComboBox);
    QVariant multiplierBandValue = getBandValue(ui->multiplierBandComboBox);

    if(ui->band3ComboBox->isVisible()){
        band3Value = getBandValue(ui->band3ComboBox);
    }
    double value = 0;
    if(band1Value.isValid()){
        value = band1Value.toInt();
    }
    if(band2Value.isValid()){
        value = value * 10 + band2Value.toInt();
    }
    if(band3Value.isValid()){
        value = value * 10 + band3Value.toInt();
    }
    if(multiplierBandValue.isValid()){
        value = value * multiplierBandValue.toDouble();
    }
    const QString resistance = UnitFormatter::format(value, _resistanceParam.unitSymbol());
    ui->resistorValueLineEdit->setText(resistance);
}

void QuickAddResistorDialog::slotToleranceBandChanged(int)
{
    QVariant toleranceBandValue = getBandValue(ui->toleranceBandComboBox);
    if(toleranceBandValue.isValid()){
        const QString tolerance = UnitFormatter::formatPercentage(toleranceBandValue.toDouble(), "%");
        ui->toleranceLineEdit->setText(tolerance);
    }
}

void QuickAddResistorDialog::showError(const QString & errorMessage)
{
    ui->messageWidget->setText(errorMessage);
    ui->messageWidget->setMessageType(KMessageWidget::Error);
    ui->messageWidget->animatedShow();
}

void QuickAddResistorDialog::showSuccess(const QString& successMessage)
{
    ui->messageWidget->setText(successMessage);
    ui->messageWidget->setMessageType(KMessageWidget::Positive);
    ui->messageWidget->animatedShow();
    QTimer::singleShot(2000, ui->messageWidget, SLOT(animatedHide()));
}

void QuickAddResistorDialog::slotAttemptAutoComplete()
{
    QString text = ui->resistorValueLineEdit->text().trimmed();
    bool ok;
    double resistance = _smdResistorCalc.parse(text, &ok);
    if(ok){
        QString formatted = UnitFormatter::format(resistance, _resistanceParam.unitSymbol());
        ui->resistorValueLineEdit->setText(formatted);
    }

}
