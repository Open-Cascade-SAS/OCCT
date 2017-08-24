// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#include <inspector/ShapeView_OpenFileDialog.hxx>
#include <inspector/ShapeView_OpenFileViewModel.hxx>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include <QApplication>
#include <QCompleter>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QPushButton>
#include <QToolButton>

const int FONT_POINT_SIZE = 18;
const int ICON_SIZE = 40;

const int OPEN_DIALOG_WIDTH = 550;
const int OPEN_DIALOG_HEIGHT = 200;

const int MARGIN_DIALOG = 4;
const int SPACING_DIALOG = 2;

// =======================================================================
// function : StartButton
// purpose :
// =======================================================================
QPushButton* ShapeView_OpenButton::StartButton()
{
  if (!myStartButton)
  {
    myStartButton = new QPushButton();
    myStartButton->setIcon (QIcon (":/icons/folder_open.png"));
    connect (myStartButton, SIGNAL (clicked()), this, SLOT (onStartButtonClicked()));
  }
  return myStartButton;
}

// =======================================================================
// function : onStartButtonClicked
// purpose :
// =======================================================================
void ShapeView_OpenButton::onStartButtonClicked()
{
  QString aDataDirName = QDir::currentPath();

  QString aFileName = ShapeView_OpenFileDialog::OpenFile (0, aDataDirName);
  aFileName = QDir().toNativeSeparators (aFileName);
  if (!aFileName.isEmpty())
  {
    QApplication::setOverrideCursor (Qt::WaitCursor);
    emit OpenFile (aFileName);
    QApplication::restoreOverrideCursor();
  }
}

// =======================================================================
// function : changeMargins
// purpose :
// =======================================================================
void changeMargins (QBoxLayout* theLayout)
{
  theLayout->setContentsMargins (MARGIN_DIALOG, MARGIN_DIALOG, MARGIN_DIALOG, MARGIN_DIALOG);
  theLayout->setSpacing (SPACING_DIALOG);
}

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ShapeView_OpenFileDialog::ShapeView_OpenFileDialog (QWidget* theParent, const QString& theDataDirName)
: QDialog (theParent)
{
  setWindowTitle (theDataDirName);
  myDataDir = theDataDirName;

  QVBoxLayout* aDialogLay = new QVBoxLayout (this);
  changeMargins (aDialogLay);

  // Title label
  QLabel* aTitleLabel = new QLabel (this);
  aTitleLabel->setText (tr ("Open File"));
  aDialogLay->addWidget (aTitleLabel);

  // Samples View
  QGroupBox* aSamplesBox = new QGroupBox (this);
  aSamplesBox->setTitle (tr ("Samples"));
  aDialogLay->addWidget (aSamplesBox);
  QVBoxLayout* aSampleLay = new QVBoxLayout (aSamplesBox);
  changeMargins (aSampleLay);
  mySamplesView = createTableView (readSampleNames());
  aSampleLay->addWidget (mySamplesView);

  // Select file
  QGroupBox* aSelectFileBox = new QGroupBox (this);
  aSelectFileBox->setTitle (tr ("Select file"));
  aDialogLay->addWidget (aSelectFileBox);
  QGridLayout* aSelectFileLay = new QGridLayout (aSelectFileBox);
  aSelectFileLay->setContentsMargins (MARGIN_DIALOG, MARGIN_DIALOG, MARGIN_DIALOG, MARGIN_DIALOG);

  mySelectedName = new QLineEdit (aSelectFileBox);
  QCompleter* aCompleter = new QCompleter();
  QFileSystemModel* aFileSystemModel = new QFileSystemModel;
  aFileSystemModel->setRootPath (QDir::rootPath());
  aCompleter->setModel (aFileSystemModel);
  mySelectedName->setCompleter (aCompleter);
  aSelectFileLay->addWidget (mySelectedName, 1, 0);

  QToolButton* aSelectFileBtn = new QToolButton (aSelectFileBox);
  aSelectFileBtn->setIcon (QIcon (":/icons/folder_open.png"));
  aSelectFileLay->addWidget (aSelectFileBtn, 1, 1);

  myFolderApplyOpen = new QToolButton (aSelectFileBox);
  myFolderApplyOpen->setIcon (QIcon (":/icons/folder_import.png"));
  myFolderApplyOpen->setIconSize (QSize (ICON_SIZE, ICON_SIZE));
  myFolderApplyOpen->setEnabled (false);
  aSelectFileLay->addWidget (myFolderApplyOpen, 0, 2, 2, 1);

  connect (mySelectedName, SIGNAL (textChanged (const QString&)),
           this, SLOT (onNameChanged (const QString&)));
  connect (aSelectFileBtn, SIGNAL (clicked()), this, SLOT (onSelectClicked()));
  connect (myFolderApplyOpen, SIGNAL (clicked()), this, SLOT (onApplySelectClicked()));

  resize (OPEN_DIALOG_WIDTH, OPEN_DIALOG_HEIGHT);
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
QString ShapeView_OpenFileDialog::OpenFile (QWidget* theParent, const QString& theDataDirName)
{
  QString aFileName;
  ShapeView_OpenFileDialog* aDialog = new ShapeView_OpenFileDialog(theParent, theDataDirName);
  if (aDialog->exec() == QDialog::Accepted)
    aFileName = aDialog->GetFileName();

  return aFileName;
}

// =======================================================================
// function : onSampleSelectionChanged
// purpose :
// =======================================================================
void ShapeView_OpenFileDialog::onSampleSelectionChanged (const QItemSelection& theSelected,
                                                           const QItemSelection&)
{
  QItemSelectionModel* aSelectionModel = (QItemSelectionModel*)sender();
  if (!aSelectionModel)
    return;
  if (theSelected.isEmpty())
    return;

  QModelIndex anIndex = theSelected.first().indexes().first();
  if (!anIndex.isValid())
    return;

  myFileName = aSelectionModel->model()->data (anIndex, Qt::ToolTipRole).toString();
  accept();
}

// =======================================================================
// function : onNameChanged
// purpose :
// =======================================================================
void ShapeView_OpenFileDialog::onNameChanged (const QString& theText)
{
  QFileInfo aFileInfo (theText);
  bool anExists = aFileInfo.exists() && aFileInfo.isFile();
  myFolderApplyOpen->setEnabled (anExists);
}

// =======================================================================
// function : onSelectClicked
// purpose :
// =======================================================================
void ShapeView_OpenFileDialog::onSelectClicked()
{
  QString anEnteredPath;
  QString aDirName = mySelectedName->text();
  if (!aDirName.isEmpty())
  {
    QDir aDir (aDirName);
    if (aDir.exists())
      anEnteredPath = aDirName;
  }

  QString aFilter (tr ("BREP file (*.brep*)"));
  QString aFileName = QFileDialog::getOpenFileName (0, "Open document", anEnteredPath, aFilter);

  if (aFileName.isEmpty())
    return; // do nothing, left the previous value

  mySelectedName->setText (aFileName);
  onNameChanged (aFileName);
}

// =======================================================================
// function : onApplySelectClicked
// purpose :
// =======================================================================
void ShapeView_OpenFileDialog::onApplySelectClicked()
{
  myFileName = mySelectedName->text();
  accept();
}

// =======================================================================
// function : createTableView
// purpose :
// =======================================================================
QTableView* ShapeView_OpenFileDialog::createTableView (const QStringList& theFileNames)
{
  QTableView* aTableView = new QTableView (this);
  aTableView->setFrameStyle (QFrame::NoFrame);
  QPalette aPalette = aTableView->viewport()->palette();
  QColor aWindowColor = aPalette.color (QPalette::Window);
  aPalette.setBrush (QPalette::Base, aWindowColor);
  aTableView->viewport()->setPalette (aPalette);

  aTableView->horizontalHeader()->setVisible (false);
  aTableView->verticalHeader()->setVisible (false);
  aTableView->setGridStyle (Qt::NoPen);
  aTableView->setModel (createModel (theFileNames));
  aTableView->setItemDelegateForRow (0, new ShapeView_OpenFileItemDelegate (aTableView,
                                                          aPalette.color (QPalette::Highlight)));
  aTableView->viewport()->setAttribute (Qt::WA_Hover);
  int aCellHeight = ICON_SIZE + aTableView->verticalHeader()->defaultSectionSize();
  aTableView->setRowHeight (0, aCellHeight);
  int aScrollHeight = aTableView->horizontalScrollBar()->sizeHint().height();
  aTableView->setMinimumHeight (aCellHeight + aScrollHeight);
  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aTableView->model());
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onSampleSelectionChanged (const QItemSelection&, const QItemSelection&)));
  aTableView->setSelectionModel (aSelectionModel);

  return aTableView;
}

// =======================================================================
// function : createModel
// purpose :
// =======================================================================
QAbstractItemModel* ShapeView_OpenFileDialog::createModel (const QStringList& theFileNames)
{
  ShapeView_OpenFileViewModel* aModel = new ShapeView_OpenFileViewModel(this);
  aModel->Init (theFileNames);
  return aModel;
}

// =======================================================================
// function : readSampleNames
// purpose :
// =======================================================================
QStringList ShapeView_OpenFileDialog::readSampleNames()
{
  QStringList aNames;

  QDir aDir (myDataDir);
  aDir.setSorting(QDir::Name);

  QFileInfoList aDirEntries = aDir.entryInfoList();
  for (int aDirId = 0; aDirId < aDirEntries.size(); ++aDirId)
  {
    QFileInfo aFileInfo = aDirEntries.at (aDirId);
    if (aFileInfo.isFile())
      aNames.append (aFileInfo.absoluteFilePath());
  }
  return aNames;
}
