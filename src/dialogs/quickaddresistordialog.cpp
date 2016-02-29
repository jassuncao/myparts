#include "quickaddresistordialog.h"
#include "ui_quickaddresistordialog.h"
#include "widgets/parametervaluevalidator.h"
#include "models/modelsprovider.h"
#include "models/categorytreemodel.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStandardItemModel>
#include <QStylePainter>
#include <QKeyEvent>
#include <QStyle>

static QStandardItem * newColorItem(const QString & name, const QColor & color, const int shortcutKey)
{
    QStandardItem * item = new QStandardItem(name);  
    item->setData(color, Qt::DecorationRole);
    item->setData(shortcutKey, ColorBandComboBox::ShortcutRole);
    return item;
}

QuickAddResistorDialog::QuickAddResistorDialog(ModelsProvider * modelsProvider, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickAddResistorDialog)
{

    ui->partCategoryComboBox->setMinimumContentsLength(22);
    ui->partCategoryComboBox->setMaxVisibleItems(40);
    ui->resistorValueLineEdit->setValidator(new ParameterValueValidator(this));
    ui->partCategoryComboBox->setModel(modelsProvider->partCategoryModel());

    ui->partStorageComboBox->setModel(storageModel);

    ui->partConditionComboBox->setModel(modelsProvider->partConditionModel());


    _partConditionModel = new QSqlQueryModel();
    _partConditionModel->setQuery("SELECT id, value, defaultCondition FROM condition ORDER BY value ASC");

    int row = findDefaultValueRow(_partConditionModel, 2);
    _defaultCondition = _partConditionModel->index(row,0).data(Qt::EditRole);

    ui->partConditionComboBox->setModel(_partConditionModel);
    ui->partConditionComboBox->setModelKeyColumn(0);
    ui->partConditionComboBox->setModelColumn(1);

    QStandardItemModel * digitsBandsModel = new QStandardItemModel(11,1);
    digitsBandsModel->setItem(0,0, newColorItem(tr("None"), QColor(Qt::transparent), Qt::Key_Backspace));
    digitsBandsModel->setItem(1,0, newColorItem(tr("Black"), QColor("black"), Qt::Key_0));
    digitsBandsModel->setItem(2,0, newColorItem(tr("Brown"), QColor("brown"), Qt::Key_1));
    digitsBandsModel->setItem(3,0, newColorItem(tr("Red"), QColor("red"), Qt::Key_2));
    digitsBandsModel->setItem(4,0, newColorItem(tr("Orange"), QColor("orange"), Qt::Key_3));
    digitsBandsModel->setItem(5,0, newColorItem(tr("Yellow"), QColor("yellow"), Qt::Key_4));
    digitsBandsModel->setItem(6,0, newColorItem(tr("Green"), QColor("green"), Qt::Key_5));
    digitsBandsModel->setItem(7,0, newColorItem(tr("Blue"), QColor("blue"), Qt::Key_6));
    digitsBandsModel->setItem(8,0, newColorItem(tr("Violet"), QColor("violet"), Qt::Key_7));
    digitsBandsModel->setItem(9,0, newColorItem(tr("Grey"), QColor("grey"), Qt::Key_8));
    digitsBandsModel->setItem(10,0, newColorItem(tr("White"), QColor("white"), Qt::Key_9));

    QStandardItemModel * multiplierBandsModel = new QStandardItemModel(11,1);
    multiplierBandsModel->setItem(0,0, newColorItem(tr("None"), QColor(), Qt::Key_Backspace));
    multiplierBandsModel->setItem(1,0, newColorItem(tr("Black"), QColor("black"), Qt::Key_0));
    multiplierBandsModel->setItem(2,0, newColorItem(tr("Brown"), QColor("brown"), Qt::Key_1));
    multiplierBandsModel->setItem(3,0, newColorItem(tr("Red"), QColor("red"), Qt::Key_2));
    multiplierBandsModel->setItem(4,0, newColorItem(tr("Orange"), QColor("orange"), Qt::Key_3));
    multiplierBandsModel->setItem(5,0, newColorItem(tr("Yellow"), QColor("yellow"), Qt::Key_4));
    multiplierBandsModel->setItem(6,0, newColorItem(tr("Green"), QColor("green"), Qt::Key_5));
    multiplierBandsModel->setItem(7,0, newColorItem(tr("Blue"), QColor("blue"), Qt::Key_6));
    multiplierBandsModel->setItem(8,0, newColorItem(tr("Violet"), QColor("violet"), Qt::Key_7));
    multiplierBandsModel->setItem(9,0, newColorItem(tr("Gold"), QColor("gold"), Qt::Key_Comma));
    multiplierBandsModel->setItem(10,0, newColorItem(tr("Silver"), QColor("silver"), Qt::Key_Colon));

    QStandardItemModel * toleranceBandsModel = new QStandardItemModel(8,1);
    toleranceBandsModel->setItem(0,0, newColorItem(tr("None"), QColor(), Qt::Key_Backspace));
    toleranceBandsModel->setItem(1,0, newColorItem(tr("Brown"), QColor("brown"), Qt::Key_F));
    toleranceBandsModel->setItem(2,0, newColorItem(tr("Red"), QColor("red"), Qt::Key_G));
    toleranceBandsModel->setItem(3,0, newColorItem(tr("Green"), QColor("green"), Qt::Key_D));
    toleranceBandsModel->setItem(4,0, newColorItem(tr("Blue"), QColor("blue"), Qt::Key_C));
    toleranceBandsModel->setItem(5,0, newColorItem(tr("Violet"), QColor("violet"), Qt::Key_B));
    toleranceBandsModel->setItem(6,0, newColorItem(tr("Gold"), QColor("gold"), Qt::Key_J));
    toleranceBandsModel->setItem(7,0, newColorItem(tr("Silver"), QColor("silver"), Qt::Key_K));

    ui->setupUi(this);
    ui->band1ComboBox->setModel(digitsBandsModel);
    ui->band2ComboBox->setModel(digitsBandsModel);
    ui->band3ComboBox->setModel(digitsBandsModel);
    ui->multiplierBandComboBox->setModel(multiplierBandsModel);
    ui->toleranceBandComboBox->setModel(toleranceBandsModel);
    ui->fourBandRadioButton->setChecked(true);
    ui->band3ComboBox->setVisible(false);
    //connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeColor(int)));
}

QuickAddResistorDialog::~QuickAddResistorDialog()
{
    delete ui;
}

void QuickAddResistorDialog::slotChangeColor(int index)
{
    QComboBox * cb = dynamic_cast<QComboBox*>(sender());    
}
