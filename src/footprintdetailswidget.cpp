#include "footprintdetailswidget.h"
#include "attachmentselectiondialog.h"
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

FootprintDetailsDelegate::FootprintDetailsDelegate(QObject * parent)
    : QStyledItemDelegate(parent)
{
}

void FootprintDetailsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QToolButton* view = dynamic_cast<QToolButton*>(editor);
    if(index.column() == FootprintTableModel::ColumnFilename && view) {
        QString filename;
        QVariant data = index.data(Qt::EditRole);
        if(data.isValid() && data.canConvert(QVariant::String)){
            filename = data.toString();
        }
        view->setText(filename);
        if(filename.isEmpty())
        {
            filename = QLatin1String(":/icons/images/footprintplaceholder.png");
        }
        QImage img;
        qDebug()<<"Loading footprint image "<< filename;
        if(img.load(filename)){
             //Perform rescaling if necessary
             if(img.width()> view->width() || img.height()> view->height()){
                  img = img.scaled( view->width(), view->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
             }
         }
         else{
             qWarning()<<"Failed to load footprint image "<<filename;
         }

        view->setIcon(QIcon(QPixmap::fromImage(img)));
        return;
    }
    QStyledItemDelegate::setEditorData(editor, index);
}
void FootprintDetailsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QToolButton* view = dynamic_cast<QToolButton*>(editor);
    if(index.column() == FootprintTableModel::ColumnFilename && view){
        model->setData(index, view->text());
        return;
    }
    return QStyledItemDelegate::setModelData(editor, model, index);
}


FootprintDetailsWidget::FootprintDetailsWidget(QWidget *parent)
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
    _mapper->setItemDelegate(new FootprintDetailsDelegate(this));
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_descriptionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_attachmentsTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentAttachmentRowChanged(QModelIndex,QModelIndex)));        
    connect(_attachmentModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotSetDirty()));
    connect(_attachmentsTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotAttachmentDoubleClicked(QModelIndex)));
}

FootprintDetailsWidget::~FootprintDetailsWidget()
{
    qDebug("Deleting FootprintDetailsWidget");
}

QModelIndex FootprintDetailsWidget::currentModelIndex() const
{
    return _currentIndex;
}

void FootprintDetailsWidget::setCurrentModelIndex(const QModelIndex & modelIndex, bool newRecord)
{    
    _currentIndex = modelIndex;
    _newRecord = newRecord;
    _mapper->setCurrentModelIndex(modelIndex);
    qDebug()<<"Curr idx "<< _currentIndex;
    setEnabled(_currentIndex.isValid());
    QVariant footprintId = modelIndex.data(Qt::EditRole);
    qDebug()<<"footprintId is "<< footprintId;
    _attachmentModel->setCurrentForeignKey(footprintId);
    _attachmentModel->select();
    if(newRecord){
        _nameLineEdit->setFocus();        
        slotSetDirty();
    }
    else{        
        slotResetDirty();
    }
}

bool FootprintDetailsWidget::setFootprintImage(const QString & filePath)
{
    _imageButton->setText(filePath);

    QImage pix;
    QString aux = filePath.isEmpty() ? QLatin1String(":/icons/images/footprintplaceholder.png") : filePath;
    qDebug()<<"Loading footprint image "<< aux;
    if(pix.load(aux)){
        qDebug()<<"Footprint image loaded" << aux;
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

void FootprintDetailsWidget::setModel(QAbstractItemModel * model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, FootprintTableModel::ColumnName);
    _mapper->addMapping(_descriptionLineEdit, FootprintTableModel::ColumnDescription);
    _mapper->addMapping(_imageButton, FootprintTableModel::ColumnFilename);
}

void FootprintDetailsWidget::slotSetDirty()
{
   _dirty = true;
   _buttonBox->setEnabled(true);
}

void FootprintDetailsWidget::slotResetDirty()
{
   _dirty = false;
   _buttonBox->setEnabled(false);
}

void FootprintDetailsWidget::slotImageContextMenu(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(_actionSetImageLocal);
    menu.addAction(_actionSetImageRemote);
    menu.addSeparator();
    menu.addAction(_actionRemoveImage);
    menu.exec(_imageButton->mapToGlobal(pos));
}


void FootprintDetailsWidget::slotAccept()
{
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("The footprint must have a name"), QMessageBox::Close);
        return;
    }
    QVariant footprintId = _currentIndex.data(Qt::EditRole);
    qDebug()<<"Footprint ID is "<<footprintId;

    qDebug()<<"Saving changes to footprint with model index" << _currentIndex ;
    if(!_mapper->submit()){
        qWarning()<<"Failed to submit changes ";
        //TODO:SHow error message
    }
    if(_newRecord){
        footprintId = _currentIndex.data(Qt::EditRole);
        qDebug()<<"New footprint ID is "<<footprintId;
        _attachmentModel->setCurrentForeignKey(footprintId);
    }
    _attachmentModel->submitAll();
    //_attachmentModel->save(footprintId);
    slotResetDirty();
    emit accepted();
}

void FootprintDetailsWidget::slotReject()
{
    qDebug()<<"Reject changes to footprint with model index" << _currentIndex ;
    if(!isNew()){
        _mapper->revert();
    }    
    QVariant footprintId = _currentIndex.data(Qt::EditRole);
    //_attachmentModel->load(footprintId);
    _attachmentModel->select();
    slotResetDirty();
    emit rejected();

}

void FootprintDetailsWidget::slotImageShow()
{
    if(_imageButton->icon().isNull()){
        slotAddImage();
    }
    else{
        //TODO: Show full image using image viewer
    }
}

void FootprintDetailsWidget::slotAddImage()
{
    QFileDialog dlg(this);
    dlg.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFile);
    if(dlg.exec()){
        QStringList fileNames = dlg.selectedFiles();
        if(fileNames.size()>0){
            QString selectedFile = fileNames.first();
            if(setFootprintImage(selectedFile)){
                slotSetDirty();
            }
        }        
    }   
}

void FootprintDetailsWidget::slotAddImageRemote()
{
    slotSetDirty();
}

void FootprintDetailsWidget::slotRemoveImage()
{  
    setFootprintImage(QString::null);
    slotSetDirty();
}

void FootprintDetailsWidget::slotAddAttachment()
{
    AttachmentSelectionDialog dlg(this);
    if(dlg.exec()){
        QUrl resourceUrl = dlg.value();
        _attachmentModel->appendRow(resourceUrl.toString(), QString());        
        slotSetDirty();
    }
}

void FootprintDetailsWidget::slotRemoveAttachment()
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

void FootprintDetailsWidget::slotCurrentAttachmentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    _removeAttachmentButton->setEnabled(current.isValid());
}

void FootprintDetailsWidget::slotAttachmentDoubleClicked(const QModelIndex &index)
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

