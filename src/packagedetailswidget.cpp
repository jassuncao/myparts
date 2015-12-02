#include "packagedetailswidget.h"
#include "dialogs/attachmentselectiondialog.h"
#include "models/customtablemodel.h"
#include "models/packagetablemodel.h"
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

PackageDetailsDelegate::PackageDetailsDelegate(QObject * parent)
    : QStyledItemDelegate(parent)
{
}

void PackageDetailsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QToolButton* view = dynamic_cast<QToolButton*>(editor);
    if(index.column() == PackageTableModel::ColumnFilename && view) {
        QString filename;
        QVariant data = index.data(Qt::EditRole);
        if(data.isValid() && data.canConvert(QVariant::String)){
            filename = data.toString();
        }
        view->setText(filename);
        if(filename.isEmpty())
        {
            filename = QLatin1String(":/icons/images/packageplaceholder.png");
        }
        QImage img;
        qDebug()<<"Loading package image "<< filename;
        if(img.load(filename)){
             //Perform rescaling if necessary
             if(img.width()> view->width() || img.height()> view->height()){
                  img = img.scaled( view->width(), view->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
             }
         }
         else{
             qWarning()<<"Failed to load package image "<<filename;
         }

        view->setIcon(QIcon(QPixmap::fromImage(img)));
        return;
    }
    QStyledItemDelegate::setEditorData(editor, index);
}
void PackageDetailsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QToolButton* view = dynamic_cast<QToolButton*>(editor);
    if(index.column() == PackageTableModel::ColumnFilename && view){
        model->setData(index, view->text());
        return;
    }
    return QStyledItemDelegate::setModelData(editor, model, index);
}


PackageDetailsWidget::PackageDetailsWidget(QWidget *parent)
    : QWidget(parent),
      _dirty(false)
{
    //Info groupbox START
    _nameLineEdit = new QLineEdit;   
    _descriptionLineEdit = new QLineEdit;
    QFormLayout * layout1 = new QFormLayout;
    layout1->addRow(tr("Name:"), _nameLineEdit);
    layout1->addRow(tr("Description:"), _descriptionLineEdit);
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
    hboxLayout1->addLayout(layout1,1);
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

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Discard);
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    //connect(_buttonBox, SIGNAL(rejected()), this, SLOT(slotReject()));
    QPushButton* btnNoKey = _buttonBox->button(QDialogButtonBox::Discard);
    connect(btnNoKey, SIGNAL(clicked()), SLOT(slotReject()));

    QVBoxLayout * mainLayout = new QVBoxLayout;   
    mainLayout->addLayout(hboxLayout1);
    mainLayout->addWidget(groupBox3,1);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    _mapper = new QDataWidgetMapper(this);
    _mapper->setItemDelegate(new PackageDetailsDelegate(this));
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_descriptionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_attachmentsTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentAttachmentRowChanged(QModelIndex,QModelIndex)));        
    connect(_attachmentModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotSetDirty()));
    connect(_attachmentsTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));
}

PackageDetailsWidget::~PackageDetailsWidget()
{
    qDebug("Deleting PackageDetailsWidget");
}

QModelIndex PackageDetailsWidget::currentModelIndex() const
{
    return _currentIndex;
}

void PackageDetailsWidget::setCurrentModelIndex(const QModelIndex & modelIndex, bool newRecord)
{    
    _currentIndex = modelIndex;
    _newRecord = newRecord;
    _mapper->setCurrentModelIndex(modelIndex);
    qDebug()<<"Curr idx "<< _currentIndex;
    setEnabled(_currentIndex.isValid());
    QVariant packageId = modelIndex.data(Qt::EditRole);
    qDebug()<<"packageId is "<< packageId;
    _attachmentModel->setCurrentForeignKey(packageId);
    _attachmentModel->select();
    if(newRecord){
        _nameLineEdit->setFocus();        
        slotSetDirty();
    }
    else{        
        slotResetDirty();
    }
}

bool PackageDetailsWidget::setPackageImage(const QString & filePath)
{
    _imageButton->setText(filePath);

    QImage pix;
    QString aux = filePath.isEmpty() ? QLatin1String(":/icons/images/packageplaceholder.png") : filePath;
    qDebug()<<"Loading package image "<< aux;
    if(pix.load(aux)){
        qDebug()<<"Package image loaded" << aux;
        if(pix.width()> _imageButton->width() || pix.height()> _imageButton->height()){
             pix = pix.scaled( _imageButton->width(), _imageButton->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
        }
    }
    else{
        //TODO: Show some warning
    }
    _imageButton->setIcon(QIcon(QPixmap::fromImage(pix)));
    return !pix.isNull();
}

void PackageDetailsWidget::setModel(QAbstractItemModel * model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, PackageTableModel::ColumnName);
    _mapper->addMapping(_descriptionLineEdit, PackageTableModel::ColumnDescription);
    _mapper->addMapping(_imageButton, PackageTableModel::ColumnFilename);
}

void PackageDetailsWidget::slotSetDirty()
{
   _dirty = true;
   _buttonBox->setEnabled(true);
}

void PackageDetailsWidget::slotResetDirty()
{
   _dirty = false;
   _buttonBox->setEnabled(false);
}

void PackageDetailsWidget::slotImageContextMenu(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(_actionSetImageLocal);
    menu.addAction(_actionSetImageRemote);
    menu.addSeparator();
    menu.addAction(_actionRemoveImage);
    menu.exec(_imageButton->mapToGlobal(pos));
}


void PackageDetailsWidget::slotAccept()
{
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("The package must have a name"), QMessageBox::Close);
        return;
    }
    QVariant packageId = _currentIndex.data(Qt::EditRole);
    qDebug()<<"Package ID is "<<packageId;

    qDebug()<<"Saving changes to package with model index" << _currentIndex ;
    if(!_mapper->submit()){
        qWarning()<<"Failed to submit changes ";
        //TODO:SHow error message
    }
    if(_newRecord){
        packageId = _currentIndex.data(Qt::EditRole);
        qDebug()<<"New package ID is "<<packageId;
        _attachmentModel->setCurrentForeignKey(packageId);
    }
    _attachmentModel->submitAll();
    //_attachmentModel->save(packageId);
    slotResetDirty();
    emit accepted();
}

void PackageDetailsWidget::slotReject()
{
    qDebug()<<"Reject changes to package with model index" << _currentIndex ;
    if(!isNew()){
        _mapper->revert();
    }    
    QVariant packageId = _currentIndex.data(Qt::EditRole);
    //_attachmentModel->load(packageId);
    _attachmentModel->select();
    slotResetDirty();
    emit rejected();

}

void PackageDetailsWidget::slotImageShow()
{
    if(_imageButton->icon().isNull()){
        slotAddImage();
    }
    else{
        //TODO: Show full image using image viewer
    }
}

void PackageDetailsWidget::slotAddImage()
{
    QFileDialog dlg(this);
    dlg.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFile);
    if(dlg.exec()){
        QStringList fileNames = dlg.selectedFiles();
        if(fileNames.size()>0){
            QString selectedFile = fileNames.first();
            if(setPackageImage(selectedFile)){
                slotSetDirty();
            }
        }        
    }   
}

void PackageDetailsWidget::slotAddImageRemote()
{
    slotSetDirty();
}

void PackageDetailsWidget::slotRemoveImage()
{  
    setPackageImage(QString::null);
    slotSetDirty();
}

void PackageDetailsWidget::slotAddAttachment()
{
    AttachmentSelectionDialog dlg(this);
    if(dlg.exec()){
        QUrl resourceUrl = dlg.value();
        _attachmentModel->appendRow(resourceUrl.toString(), QString());        
        slotSetDirty();
    }
}

void PackageDetailsWidget::slotRemoveAttachment()
{
    QModelIndex index = _attachmentsTable->currentIndex();
    if(index.isValid()){
        qDebug()<<"Removing row";
        bool res = _attachmentModel->removeRow(index.row());
        if(!res){
            qDebug()<<"Failed to remove";
        }
        slotSetDirty();
    }
}

void PackageDetailsWidget::slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    _removeAttachmentButton->setEnabled(current.isValid());
}

void PackageDetailsWidget::slotAttachmentDoubleClicked(const QModelIndex &index)
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

