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

#include <inspector/TInspector_OpenFileDialog.hxx>

#include <inspector/TInspector_OpenButton.hxx>
#include <inspector/TInspector_OpenFileViewModel.hxx>

#include <inspector/TInspector_Communicator.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QCompleter>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QTableView>
#include <QToolButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

const int FONT_POINT_SIZE = 18;
const int ICON_SIZE = 40;

const int OPEN_DIALOG_WIDTH = 550;
const int OPEN_DIALOG_HEIGHT = 200;

const int MARGIN_DIALOG = 4;
const int SPACING_DIALOG = 2;

const int RECENT_FILES_CACHE_SIZE = 10;

TInspector_Communicator* MyCommunicator;

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
TInspector_OpenFileDialog::TInspector_OpenFileDialog (QWidget* theParent, const QStringList& theRecentlyOpenedFiles)
: QDialog (theParent), myRecentlyOpenedFiles (theRecentlyOpenedFiles)
{
  setWindowTitle (tr ("Open File"));

  QVBoxLayout* aDialogLay = new QVBoxLayout (this);
  changeMargins (aDialogLay);

  // Samples View
  QGroupBox* aSamplesBox = new QGroupBox (this);
  aSamplesBox->setTitle (tr ("Recent files"));
  aDialogLay->addWidget (aSamplesBox);
  QVBoxLayout* aSampleLay = new QVBoxLayout (aSamplesBox);
  changeMargins (aSampleLay);
  mySamplesView = createTableView (theRecentlyOpenedFiles);
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
  aSelectFileBtn->setIcon (QIcon (":folder_open.png"));
  aSelectFileLay->addWidget (aSelectFileBtn, 1, 1);

  connect (aSelectFileBtn, SIGNAL (clicked()), this, SLOT (onSelectClicked()));
  connect (mySelectedName, SIGNAL (returnPressed()), this, SLOT (onApplySelectClicked()));

  resize (OPEN_DIALOG_WIDTH, OPEN_DIALOG_HEIGHT);
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
QString TInspector_OpenFileDialog::OpenFile (QWidget* theParent, const QStringList& theRecentlyOpenedFiles)
{
  QString aFileName;
  TInspector_OpenFileDialog* aDialog = new TInspector_OpenFileDialog (theParent, theRecentlyOpenedFiles);
  if (aDialog->exec() == QDialog::Accepted)
    aFileName = aDialog->GetFileName();

  return aFileName;
}

// =======================================================================
// function : Communicator
// purpose :
// =======================================================================
TInspector_Communicator* TInspector_OpenFileDialog::Communicator()
{
  if (!MyCommunicator)
    MyCommunicator = new TInspector_Communicator();
  return MyCommunicator;
}

// =======================================================================
// function : GetPluginRecentlyOpenedFiles
// purpose :
// =======================================================================
void TInspector_OpenFileDialog::GetPluginRecentlyOpenedFiles (const TCollection_AsciiString& thePluginName,
                                                                 TInspector_Communicator* theCommunicator,
                                                                 QStringList& theFileNames)
{
  Handle(TInspectorAPI_PluginParameters) aParameters = theCommunicator->GetPluginParameters();
  TInspectorAPI_PreferencesDataMap aPreferencesItem;
  aParameters->GetPreferences (thePluginName, aPreferencesItem);

  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt (aPreferencesItem); anItemIt.More(); anItemIt.Next())
  {
    if (!anItemIt.Key().IsEqual("recently_opened_files"))
      continue;
    theFileNames = QString(anItemIt.Value().ToCString()).split(";", QString::SkipEmptyParts);
    if (theFileNames.size() > RECENT_FILES_CACHE_SIZE)
      for (int i = 0; i < theFileNames.size() - RECENT_FILES_CACHE_SIZE; i++)
        theFileNames.removeFirst();
    break;
  }
}

// =======================================================================
// function : SetPluginRecentlyOpenedFiles
// purpose :
// =======================================================================
void TInspector_OpenFileDialog::SetPluginRecentlyOpenedFiles (const TCollection_AsciiString& thePluginName,
                                                                 TInspector_Communicator* theCommunicator,
                                                                 QStringList& theFileNames)
{
  Handle(TInspectorAPI_PluginParameters) aParameters = theCommunicator->GetPluginParameters();

  TInspectorAPI_PreferencesDataMap aPreferencesItem;
  aParameters->GetPreferences (thePluginName, aPreferencesItem);
  aPreferencesItem.Bind ("recently_opened_files", TCollection_AsciiString (theFileNames.join (";").toUtf8().data()));

  aParameters->SetPreferences (thePluginName, aPreferencesItem);
}

// =======================================================================
// function : onSampleSelectionChanged
// purpose :
// =======================================================================
void TInspector_OpenFileDialog::onSampleSelectionChanged (const QItemSelection& theSelected,
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
// function : onSelectClicked
// purpose :
// =======================================================================
void TInspector_OpenFileDialog::onSelectClicked()
{
  QString anEnteredPath;
  QString aDirName = mySelectedName->text();
  if (!aDirName.isEmpty())
  {
    QDir aDir (aDirName);
    if (aDir.exists())
      anEnteredPath = aDirName;
  }

  QString aFileName = QFileDialog::getOpenFileName (0, "Open document", anEnteredPath);

  if (aFileName.isEmpty())
    return; // do nothing, left the previous value


  myFileName = aFileName;
  accept();
}

// =======================================================================
// function : onApplySelectClicked
// purpose :
// =======================================================================
void TInspector_OpenFileDialog::onApplySelectClicked()
{
  QString aFileName = mySelectedName->text();

  QFileInfo aFileInfo (aFileName);
  if (!aFileInfo.exists() || !aFileInfo.isFile())
    return;

  myFileName = mySelectedName->text();
  accept();
}

// =======================================================================
// function : createTableView
// purpose :
// =======================================================================
QTableView* TInspector_OpenFileDialog::createTableView (const QStringList& theFileNames)
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
  aTableView->setItemDelegateForRow (0, new TInspectorEXE_OpenFileItemDelegate (aTableView,
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
QAbstractItemModel* TInspector_OpenFileDialog::createModel (const QStringList& theFileNames)
{
  TInspector_OpenFileViewModel* aModel = new TInspector_OpenFileViewModel (this);
  aModel->Init (theFileNames);
  return aModel;
}
