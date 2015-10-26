#include "footprinteditorwidget.h"
#include "../attachmentselectiondialog.h"
#include "models/customtablemodel.h"
#include "models/footprinttablemodel.h"
#include <QDebug>
#include <QToolButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QAction>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QDataWidgetMapper>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QIdentityProxyModel>
#include <QSqlError>
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QDesktopServices>

FootprintEditorWidget::FootprintEditorWidget(QWidget *parent) :
    QWidget(parent)
{
    //Info groupbox START
    _nameLineEdit = new QLineEdit;
    _descriptionLineEdit = new QLineEdit;
    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), _nameLineEdit);
    formLayout->addRow(tr("Description:"), _descriptionLineEdit);
    //Info groupbox END

    //Image groupbox START

    _imageButton = new QToolButton;
    _imageButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _imageButton->setMinimumSize(128,128);
    _imageButton->setMaximumSize(256,256);
    _imageButton->setIconSize(QSize(128,128));
    _imageButton->setContextMenuPolicy(Qt::CustomContextMenu);
    _imageButton->setToolTip(tr("Right click to change image"));

    connect(_imageButton, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotImageContextMenu(QPoint)));
    connect(_imageButton, SIGNAL(clicked()), this, SLOT(slotImageShow()));

    _actionViewImage = new QAction(tr("View full image"),this);
    _actionSetImageLocal = new QAction(tr("Use local image"),this);
    _actionSetImageRemote = new QAction(tr("Use remote image"),this);
    _actionRemoveImage = new QAction(tr("Remove image"),this);

    connect(_actionSetImageLocal, SIGNAL(triggered()), this, SLOT(slotAddImage()));
    connect(_actionSetImageRemote, SIGNAL(triggered()), this, SLOT(slotAddImageRemote()));
    connect(_actionRemoveImage, SIGNAL(triggered()), this, SLOT(slotRemoveImage()));

    QHBoxLayout * hboxLayout1 = new QHBoxLayout;
    hboxLayout1->addWidget(_imageButton);
    hboxLayout1->addLayout(formLayout,1);
    //Image groupbox END

    //Attachments groupbox START
    _attachmentModel = AttachmentTableModel3::createNewFootprintAttachmentModel(this);

    _attachmentsTable = new QTableView;
    _attachmentsTable->setSelectionMode(QTableView::SingleSelection);
    _attachmentsTable->setSelectionBehavior(QTableView::SelectRows);
    _attachmentsTable->setModel(_attachmentModel);
    _attachmentsTable->verticalHeader()->setVisible(false);
    _attachmentsTable->horizontalHeader()->setStretchLastSection(true);
    _attachmentsTable->setColumnWidth(0, 512);

    QPushButton* addAttachmentButton = new QPushButton(QIcon(":/icons/link_add"), tr("Add..."));
    _removeAttachmentButton = new QPushButton(QIcon(":/icons/link_delete"), tr("Remove"));
    QVBoxLayout * vboxLayout2 = new QVBoxLayout;
    vboxLayout2->addWidget(addAttachmentButton);
    vboxLayout2->addWidget(_removeAttachmentButton);
    vboxLayout2->addStretch(1);

    QHBoxLayout * hboxLayout2 = new QHBoxLayout;
    hboxLayout2->addWidget(_attachmentsTable);
    hboxLayout2->addLayout(vboxLayout2);

    QGroupBox * groupBox3= new QGroupBox(tr("Attachments"));
    groupBox3->setLayout(hboxLayout2);

    connect(addAttachmentButton, SIGNAL(clicked()), this, SLOT(slotAddAttachment()));
    connect(_removeAttachmentButton, SIGNAL(clicked()), this, SLOT(slotRemoveAttachment()));
    //Attachments groupbox END

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(groupBox3,1);
    mainLayout->addStretch();
    setLayout(mainLayout);

    _mapper = new QDataWidgetMapper(this);
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_descriptionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_attachmentsTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentAttachmentRowChanged(QModelIndex,QModelIndex)));
    connect(_attachmentModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotSetDirty()));
    connect(_attachmentsTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));
}

QModelIndex FootprintEditorWidget::currentModelIndex() const
{
    return _currentIndex;
}

void FootprintEditorWidget::setCurrentModelIndex(const QModelIndex & modelIndex)
{
    _currentIndex = modelIndex;
    _mapper->setCurrentModelIndex(modelIndex);
    setImageEditorData(_mapper->currentIndex());
    setEnabled(_currentIndex.isValid());
    QVariant footprintId = modelIndex.data(Qt::EditRole);
    qDebug()<<"footprintId is "<< footprintId;
    _attachmentModel->setCurrentForeignKey(footprintId);
    _attachmentModel->select();
}

void FootprintEditorWidget::setModel(QAbstractItemModel * model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, FootprintTableModel::ColumnName);
    _mapper->addMapping(_descriptionLineEdit, FootprintTableModel::ColumnDescription);
}

bool FootprintEditorWidget::validate()
{
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("Please fill the name field"), QMessageBox::Close);
        return false;
    }
    return true;
}

void FootprintEditorWidget::submit()
{
    setImageModelData(_mapper->currentIndex());
    _mapper->submit();
}

void FootprintEditorWidget::submitChilds(const QVariant & id)
{
   _attachmentModel->setCurrentForeignKey(id);
   _attachmentModel->submitAll();
}

void FootprintEditorWidget::revert()
{
    _mapper->revert();
    _attachmentModel->revert();
}

void FootprintEditorWidget::slotContentChanged(){
    emit contentChanged();
}

void FootprintEditorWidget::slotImageContextMenu(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(_actionSetImageLocal);
    menu.addAction(_actionSetImageRemote);
    menu.addSeparator();
    menu.addAction(_actionRemoveImage);
    menu.exec(_imageButton->mapToGlobal(pos));
}

void FootprintEditorWidget::setImageEditorData(int row)
{
    QString filename;

    QModelIndex filenameIndex = _model->index(row, FootprintTableModel::ColumnFilename);
    QVariant data = _model->data(filenameIndex, Qt::EditRole);
    if(data.isValid() && data.canConvert(QVariant::String)){
        filename = data.toString();
    }
    _imageButton->setText(filename);
    if(filename.isEmpty())
    {
        filename = QLatin1String(":/icons/images/footprintplaceholder.png");
    }
    QImage img;
    qDebug()<<"Loading footprint image "<< filename;
    if(img.load(filename)){
         //Perform rescaling if necessary
         if(img.width()> _imageButton->width() || img.height()> _imageButton->height()){
              img = img.scaled( _imageButton->width(), _imageButton->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
         }
     }
     else{
         qWarning()<<"Failed to load footprint image "<<filename;
     }

    _imageButton->setIcon(QIcon(QPixmap::fromImage(img)));
}

void FootprintEditorWidget::setImageModelData(int row)
{
    QModelIndex filenameIndex = _model->index(row, FootprintTableModel::ColumnFilename);
    _model->setData(filenameIndex, _imageButton->text());

}
