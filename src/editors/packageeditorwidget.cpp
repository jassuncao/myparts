#include "packageeditorwidget.h"
#include "dialogs/attachmentselectiondialog.h"
#include "models/customtablemodel.h"
#include "models/basicentitytablemodel.h"
#include "utils.h"
#include "dialogs/imageviewer.h"
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


PackageEditorWidget::PackageEditorWidget(QWidget *parent) :
    AbstractEditor(parent), _model(0)
{
    //Info groupbox START
    _nameLineEdit = new QLineEdit;
    _descriptionLineEdit = new QLineEdit;
    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), _nameLineEdit);
    formLayout->addRow(tr("Description:"), _descriptionLineEdit);
    setFocusProxy(_nameLineEdit);
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
    _attachmentModel = AttachmentTableModel3::createNewPackageAttachmentModel(this);

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
    mainLayout->addLayout(hboxLayout1);
    mainLayout->addWidget(groupBox3);
    mainLayout->addStretch();
    setLayout(mainLayout);

    _mapper = new QDataWidgetMapper(this);
    connect(_nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(_descriptionLineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotContentChanged()));
    connect(_attachmentsTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentAttachmentRowChanged(QModelIndex,QModelIndex)));
    connect(_attachmentModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotContentChanged()));
    connect(_attachmentsTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));    
}

/*
QModelIndex PackageEditorWidget::currentModelIndex() const
{
    return _currentIndex;
}

void PackageEditorWidget::setCurrentModelIndex(const QModelIndex & modelIndex)
{
    _currentIndex = modelIndex;
    _mapper->setCurrentModelIndex(modelIndex);
    setImageEditorData(_mapper->currentIndex());
    setEnabled(_currentIndex.isValid());
    QVariant packageId = modelIndex.data(Qt::EditRole);
    qDebug()<<"packageId is "<< packageId;

    _attachmentModel->setCurrentForeignKey(packageId);
    _attachmentModel->select();
}
*/

void PackageEditorWidget::setModel(QAbstractItemModel * model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, PackageTableModel::ColumnName);
    _mapper->addMapping(_descriptionLineEdit, PackageTableModel::ColumnDescription);
}

void PackageEditorWidget::setCurrentIndex(int row)
{
    _mapper->setCurrentIndex(row);
    setImageEditorData(row);
    setEnabled(row >= 0);
    QVariant packageId = _model->index(row, PackageTableModel::ColumnId).data(Qt::EditRole);
    qDebug()<<"packageId is "<< packageId;
    _attachmentModel->setCurrentForeignKey(packageId);
    _attachmentModel->select();
}

int PackageEditorWidget::currentIndex() const {
    return _mapper->currentIndex();
}

bool PackageEditorWidget::validate()
{
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("Please fill the name field"), QMessageBox::Close);
        return false;
    }
    return true;
}

void PackageEditorWidget::submit()
{
    setImageModelData(_mapper->currentIndex());
    _mapper->submit();
}

void PackageEditorWidget::submitChilds(const QVariant & id)
{
   _attachmentModel->setCurrentForeignKey(id);
   _attachmentModel->submitAll();
}

void PackageEditorWidget::revert()
{
    _mapper->revert();
    _attachmentModel->select();
}

void PackageEditorWidget::slotImageContextMenu(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(_actionSetImageLocal);
    menu.addAction(_actionSetImageRemote);
    menu.addSeparator();
    menu.addAction(_actionRemoveImage);
    menu.exec(_imageButton->mapToGlobal(pos));
}

void PackageEditorWidget::setImageEditorData(int row)
{
    QString filename;
    QModelIndex filenameIndex = _model->index(row, PackageTableModel::ColumnFilename);
    QVariant data = _model->data(filenameIndex, Qt::EditRole);
    if(data.isValid() && data.canConvert(QVariant::String)){
        filename = data.toString();
    }
    setPackageImage(filename);
}

void PackageEditorWidget::setPackageImage(const QString & filePath)
{
    QImage pix;
    QString aux = filePath.isEmpty() ? QLatin1String(":/icons/images/packageplaceholder_128x128.png") : filePath;
    qDebug()<<"Loading package image "<< aux;
    if(pix.load(aux)){
        qDebug()<<"Package image loaded" << aux;
        //Perform rescaling if necessary
        if(pix.width()> _imageButton->width() || pix.height()> _imageButton->height()){
             pix = pix.scaled( _imageButton->width(), _imageButton->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
        }
    }
    else{
        qWarning()<<"Failed to load package image "<<filePath;
        //TODO: Show some warning
    }
    _imageButton->setText(filePath);
    _imageButton->setIcon(QIcon(QPixmap::fromImage(pix)));
}

void PackageEditorWidget::setImageModelData(int row)
{
    QModelIndex filenameIndex = _model->index(row, PackageTableModel::ColumnFilename);
    _model->setData(filenameIndex, _imageButton->text());
}

void PackageEditorWidget::slotImageShow()
{
    if(_imageButton->text().isEmpty()){
        slotAddImage();
    }
    else{
        ImageViewer viewer(this);
        viewer.open(_imageButton->text());
        //TODO: Show full image using image viewer
    }
}

void PackageEditorWidget::slotAddImage()
{
    QString docsDir = Utils::getDocumentsDirectory();
    QString selectedFile = QFileDialog::getOpenFileName(this, tr("Choose an image file"), docsDir, tr("Images (*.png *.xpm *.jpg)"));
    if(!selectedFile.isEmpty()){
        setPackageImage(selectedFile);
        slotContentChanged();
    }
}

void PackageEditorWidget::slotAddImageRemote()
{
    //TODO: Download image from URL and store it locally
}

void PackageEditorWidget::slotRemoveImage()
{
    setPackageImage(QString::null);
    slotContentChanged();
}

void PackageEditorWidget::slotAddAttachment()
{
    AttachmentSelectionDialog dlg(this);
    if(dlg.exec()){
        QUrl resourceUrl = dlg.value();
        int row = _attachmentModel->rowCount();
        _attachmentModel->insertRow(row);
        const QModelIndex & index = _attachmentModel->index(row, AttachmentTableModel3::ColumnURL);
        _attachmentModel->setData(index, resourceUrl.toString());
    }
}

void PackageEditorWidget::slotRemoveAttachment()
{
    QModelIndex index = _attachmentsTable->currentIndex();
    if(index.isValid()){
        qDebug()<<"Removing row";
        bool res = _attachmentModel->removeRow(index.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
    }
}

void PackageEditorWidget::slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    _removeAttachmentButton->setEnabled(current.isValid());
}

void PackageEditorWidget::slotAttachmentDoubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    if(index.column()==0){
        const QModelIndex & urlColIndex = _attachmentModel->index(index.row(), AttachmentTableModel3::ColumnURL);
        QString url = urlColIndex.data(Qt::EditRole).toString();
        QDesktopServices::openUrl(QUrl::fromUserInput(url));
    }
}
