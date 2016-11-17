#include "tableexportdialog.h"
#include "ui_tableexportdialog.h"
#include "utils.h"
#include <QFileDialog>
#include <QDebug>
#include <QTextCodec>

TableExportDialog::TableExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableExportDialog)
{
    ui->setupUi(this);
    connect(ui->browsePushButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()));
    loadCharsets();
    loadDelimiter();
    /*
    //Test purposes
    for(int i=0; i<ui->charSetComboBox->count(); ++i){
        QTextCodec * codec = QTextCodec::codecForName(ui->charSetComboBox->itemData(i).toString().toAscii());
        if(!codec){
            qDebug()<<"Invalid name "<<ui->charSetComboBox->itemData(i).toString();
        }
    }
    */
}

TableExportDialog::~TableExportDialog()
{
    delete ui;
}

QString TableExportDialog::filename() const
{
    return ui->filenameEdit->text();
}

QString TableExportDialog::charsetName() const
{
    int index = ui->charSetComboBox->currentIndex();
    if(index < 0){
        index = 0;
    }
    return ui->charSetComboBox->itemData(index).toString();
}

QChar TableExportDialog::fieldDelimiter() const
{
    int index = ui->delimiterComboBox->currentIndex();
    if(index < 0){
        index = 0;
    }
    return ui->delimiterComboBox->itemData(index).toChar();
}

bool TableExportDialog::includeHeader() const
{
    return ui->includeHeaderCheckBox->isChecked();
}

void TableExportDialog::loadCharsets()
{
    ui->charSetComboBox->addItem(tr("Unicode"), "UTF-8");
    ui->charSetComboBox->addItem(tr("Western"), "Windows-1252");
    ui->charSetComboBox->addItem(tr("Western (Euro)"), "ISO-8859-15");
    ui->charSetComboBox->addItem(tr("Arabic (Windows)"), "windows-1256");
    ui->charSetComboBox->addItem(tr("Arabic (ISO)"), "ISO-8859-6");
    ui->charSetComboBox->addItem(tr("Baltic (Windows)"), "windows-1257");
    ui->charSetComboBox->addItem(tr("Baltic (ISO)"), "ISO-8859-4");
    ui->charSetComboBox->addItem(tr("Central European (Windows)"), "windows-1250");
    ui->charSetComboBox->addItem(tr("Central European (ISO)"), "ISO-8859-2");
    ui->charSetComboBox->addItem(tr("Chinese, Simplified"), "GBK");
    ui->charSetComboBox->addItem(tr("Chinese, Traditional"), "BIG5");
    ui->charSetComboBox->addItem(tr("Cyrillic (Windows)"), "windows-1251");
    ui->charSetComboBox->addItem(tr("Cyrillic (ISO)"), " ISO-8859-5");
    ui->charSetComboBox->addItem(tr("Cyrillic (KOI8-R)"), "KOI8-R");
    ui->charSetComboBox->addItem(tr("Cyrillic (KOI8-U)"), "KOI8-U");
    ui->charSetComboBox->addItem(tr("Cyrillic (DOS)"), "IBM866");
    ui->charSetComboBox->addItem(tr("Greek (Windows)"), "windows-1253");
    ui->charSetComboBox->addItem(tr("Greek (ISO)"), "ISO-8859-7");
    ui->charSetComboBox->addItem(tr("Hebrew"), "windows-1255");
    ui->charSetComboBox->addItem(tr("Hebrew, Visual"), "ISO-8859-8");
    ui->charSetComboBox->addItem(tr("Japanese (Shift_JIS)"), "Shift_JIS");
    ui->charSetComboBox->addItem(tr("Japanese (EUC-JP)"), "EUC-JP");
    ui->charSetComboBox->addItem(tr("Japanese (ISO-2022-JP)"), "ISO-2022-JP");
    ui->charSetComboBox->addItem(tr("Korean"), "EUC-KR");
    ui->charSetComboBox->addItem(tr("Thai"), "IBM874");
    ui->charSetComboBox->addItem(tr("Turkish"), "windows-1254");
    ui->charSetComboBox->addItem(tr("Vietnamese"), "windows-1258");

    ui->charSetComboBox->setCurrentIndex(0);
}

void TableExportDialog::loadDelimiter()
{
    ui->delimiterComboBox->addItem(",",QChar(','));
    ui->delimiterComboBox->addItem(";",QChar(';'));
    ui->delimiterComboBox->addItem(":",QChar(':'));
    ui->delimiterComboBox->addItem(tr("Tab"),QChar('\t'));
    ui->delimiterComboBox->addItem(tr("Space"),QChar(' '));

    ui->delimiterComboBox->setCurrentIndex(0);
}

void TableExportDialog::onBrowse()
{
    QString docsDir = Utils::getDocumentsDirectory();
    if(docsDir.isEmpty()){
        docsDir = QDir::currentPath();
    }
    QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), docsDir, tr("CSV (*.csv)"));
    if(!filename.isEmpty()){
        ui->filenameEdit->setText(filename);
    }
}
