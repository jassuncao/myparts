/****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include <QScrollArea>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QMenu>

#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent) : QDialog(parent)
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setContextMenuPolicy(Qt::CustomContextMenu);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    createActions();
    createMenus();

    connect(imageLabel, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ctxMenu(QPoint)));
    setWindowTitle(tr("Image Viewer"));
    resize(500, 400);
    setModal(true);
}

void ImageViewer::open(const QString & fileName)
{
     if (!fileName.isEmpty()) {
         QImage image(fileName);
         if (image.isNull()) {
             QMessageBox::information(this, tr("Image Viewer"),
                                      tr("Cannot load %1.").arg(fileName));
             return;
         }
         imageLabel->setPixmap(QPixmap::fromImage(image));
         scaleFactor = 1.0;

         fitToWindowAct->setEnabled(true);
         updateActions();

         if (!fitToWindowAct->isChecked())
             imageLabel->adjustSize();
     }
     exec();
}

void ImageViewer::zoomIn()
 {
     scaleImage(1.25);
 }

 void ImageViewer::zoomOut()
 {
     scaleImage(0.8);
 }

 void ImageViewer::normalSize()
 {
     imageLabel->adjustSize();
     scaleFactor = 1.0;
 }

 void ImageViewer::fitToWindow()
 {
     bool fitToWindow = fitToWindowAct->isChecked();
     scrollArea->setWidgetResizable(fitToWindow);
     if (!fitToWindow) {
         normalSize();
     }
     updateActions();
 }

void ImageViewer::createActions()
{


      zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
      zoomInAct->setShortcut(tr("Ctrl++"));
      zoomInAct->setEnabled(false);
      connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

      zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
      zoomOutAct->setShortcut(tr("Ctrl+-"));
      zoomOutAct->setEnabled(false);
      connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

      normalSizeAct = new QAction(tr("&Normal Size"), this);
      normalSizeAct->setShortcut(tr("Ctrl+S"));
      normalSizeAct->setEnabled(false);
      connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

      fitToWindowAct = new QAction(tr("&Fit to Window"), this);
      fitToWindowAct->setEnabled(false);
      fitToWindowAct->setCheckable(true);
      fitToWindowAct->setShortcut(tr("Ctrl+F"));
      connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));
  }

  void ImageViewer::createMenus()
  {
      viewMenu = new QMenu(tr("&View"), this);
      viewMenu->addAction(zoomInAct);
      viewMenu->addAction(zoomOutAct);
      viewMenu->addAction(normalSizeAct);
      viewMenu->addSeparator();
      viewMenu->addAction(fitToWindowAct);
  }

  void ImageViewer::ctxMenu(const QPoint &pos)
  {
      viewMenu->exec(imageLabel->mapToGlobal(pos));
  }

  void ImageViewer::updateActions()
  {
      zoomInAct->setEnabled(!fitToWindowAct->isChecked());
      zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
      normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
  }

  void ImageViewer::scaleImage(double factor)
  {
      Q_ASSERT(imageLabel->pixmap());
      scaleFactor *= factor;
      imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

      adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
      adjustScrollBar(scrollArea->verticalScrollBar(), factor);

      zoomInAct->setEnabled(scaleFactor < 3.0);
      zoomOutAct->setEnabled(scaleFactor > 0.333);
  }

  void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
  {
      scrollBar->setValue(int(factor * scrollBar->value()
                              + ((factor - 1) * scrollBar->pageStep()/2)));
  }
