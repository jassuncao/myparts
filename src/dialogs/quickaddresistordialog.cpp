#include "quickaddresistordialog.h"
#include "ui_quickaddresistordialog.h"
#include "widgets/parametervaluevalidator.h"
#include "models/modelsprovider.h"
#include "models/categorytreemodel.h"
#include "models/storagetreemodel.h"
#include "models/partconditionmodel.h"
#include "models/partssqltablemodel.h"
#include "widgets/unitformatter.h"
#include "widgets/unitparser.h"
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


static QStandardItem * newColorItem(const QString & name, const QColor & color, const QVariant & value, const int shortcutKey)
{
    QStandardItem * item = new QStandardItem(name);  
    item->setData(color, Qt::DecorationRole);
    item->setData(shortcutKey, ColorBandComboBox::ShortcutRole);
    item->setData(value, ColorBandComboBox::ValueRole);
    return item;
}

QuickAddResistorDialog::QuickAddResistorDialog(ModelsProvider * modelsProvider, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickAddResistorDialog),
    _partsModel(modelsProvider->partsModel())
{
    ui->setupUi(this);

    QSettings settings;
    _resistorNameTemplate = settings.value("resistor_template", tr("Resistor %1 %2 (%3)")).toString();
/*
    _resistanceUnit = Utils::getStandardUnit(Entities::RESISTANCE);
    _powerUnit = Utils::getStandardUnit(Entities::POWER);
    _toleranceUnit = Utils::getStandardUnit(Entities::TOLERANCE);

    _resistanceSymbol = Utils::getStandardUnitSymbol(Entities::RESISTANCE);
    _powerSymbol = Utils::getStandardUnitSymbol(Entities::POWER);
    _toleranceSymbol = Utils::getStandardUnitSymbol(Entities::TOLERANCE);
*/
    _partParams = PartParametersTableModel3::createNew(this);

    ui->resistorValueLineEdit->setValidator(new ParameterValueValidator(QString("R"), this));
    ui->resistorPowerComboBox->setValidator(new ParameterValueValidator(QString("W"), this));
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
    //if(defaultConditionRow>=0)

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
}

QuickAddResistorDialog::~QuickAddResistorDialog()
{
    delete ui;
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
    ui->resistorPowerComboBox->clearEditText();
    ui->toleranceLineEdit->clear();
    ui->quantitySpinBox->clear();
}

void QuickAddResistorDialog::slotAddResistor()
{
    bool resistanceValid;
    QString resistanceStr = ui->resistorValueLineEdit->text();

    double resistance = UnitParser::parseUnit(resistanceStr, _resistanceUnit.symbol(), &resistanceValid);
    if(resistanceValid){
        resistanceStr = UnitFormatter::format(resistance, _resistanceUnit.symbol());
        //_partParams->appendParameter(_resistanceUnit.name(),resistance,Entities::RESISTANCE);
    }
    else {
        qWarning("Resistance value is invalid %s", qPrintable(resistanceStr));
        //TODO: Show warning
    }

    QString powerStr = ui->resistorPowerComboBox->currentText();
    bool powerValid;
    double power = UnitParser::parseUnit(powerStr, _powerUnit.symbol(), &powerValid);
    if(powerValid){
        powerStr = UnitFormatter::format(power, _powerUnit.symbol());
        //_partParams->appendParameter(_powerUnit.name(), power, Entities::POWER);
    }
    else {
        qWarning("Power value is invalid %s", qPrintable(powerStr));
        //TODO: Show warning
    }

    QString toleranceStr = ui->toleranceLineEdit->text();
    bool toleranceValid;
    double tolerance = UnitParser::parseUnit(toleranceStr, _toleranceUnit.symbol(), &toleranceValid);
    if(toleranceValid){
        toleranceStr = UnitFormatter::format(tolerance, _toleranceUnit.symbol());
        //_partParams->appendParameter(_toleranceUnit.name(), tolerance, Entities::TOLERANCE);
    }
    else {
        qWarning("Tolerance value is invalid %s", qPrintable(toleranceStr));
        //TODO: Show warning
    }

    QVariant createDate = QVariant(QDateTime::currentDateTimeUtc().toTime_t());
    QString partName = _resistorNameTemplate.arg(resistanceStr, powerStr, toleranceStr);
    QVariant category = getSelectedCategory();
    QVariant storage = getSelectedStorage();
    QVariant condition = getSelectedCondition();
    QVariant quantity = ui->quantitySpinBox->value();
    QSqlRecord initialData = _partsModel->record();
    qDebug()<<"Part cat "<<category;
    qDebug()<<"Part storage "<<storage;
    initialData.setValue(PartsSqlTableModel::ColumnName, partName);
    initialData.setValue(PartsSqlTableModel::ColumnConditionId, condition);
    initialData.setValue(PartsSqlTableModel::ColumnPartUnitId, 1);//XXX
    initialData.setValue(PartsSqlTableModel::ColumnCategoryId, category);
    initialData.setValue(PartsSqlTableModel::ColumnStorageId, storage);
    initialData.setValue(PartsSqlTableModel::ColumnCreateDate, createDate);
    initialData.setValue(PartsSqlTableModel::ColumnActualStock, quantity);

    int newRow = _partsModel->rowCount();
    bool success = _partsModel->insertRecord(newRow, initialData);
    if(success) {
        _partsModel->database().transaction();
        if(_partsModel->submitAll()){
            _partParams->setCurrentPartId(_partsModel->lastInsertedId());
            _partParams->submitAll();
            _partsModel->database().commit();
        }
        else{
            qWarning()<<"Failed to add part: Reason: " << _partsModel->lastError();
            _partsModel->database().rollback();
        }
        _partParams->setCurrentPartId(QVariant());
        _partParams->select();
    }
    else {
        qWarning("Failed to insert new part");
    }

}
QVariant QuickAddResistorDialog::getSelectedCategory() const
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
    qDebug()<<"Storage id is "<<itemId;
    return QVariant(itemId);
}

QVariant QuickAddResistorDialog::getSelectedStorage() const
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
    qDebug()<<"Storage id is "<<itemId;
    return QVariant(itemId);
}

QVariant QuickAddResistorDialog::getSelectedCondition() const
{
    return ui->partConditionComboBox->currentKey();
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
    }\
    if(multiplierBandValue.isValid()){
        value = value * multiplierBandValue.toDouble();
    }
    const QString resistance = UnitFormatter::format(value, _resistanceUnit.symbol());
    ui->resistorValueLineEdit->setText(resistance);
}

void QuickAddResistorDialog::slotToleranceBandChanged(int)
{
    QVariant toleranceBandValue = getBandValue(ui->toleranceBandComboBox);
    if(toleranceBandValue.isValid()){
        const QString tolerance = UnitFormatter::formatPercentage(toleranceBandValue.toDouble(), _toleranceUnit.symbol());
        ui->toleranceLineEdit->setText(tolerance);
    }
}
