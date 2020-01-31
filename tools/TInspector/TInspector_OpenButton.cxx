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

#include <inspector/TInspector_OpenButton.hxx>

#include <inspector/TInspector_Communicator.hxx>
#include <inspector/TInspector_OpenFileDialog.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QDir>
#include <QPushButton>
#include <Standard_WarningsRestore.hxx>

const int RECENT_FILES_CACHE_SIZE = 10;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TInspector_OpenButton::TInspector_OpenButton (QObject* theParent)
 : QObject (theParent), myStartButton (0)
{
}

// =======================================================================
// function : StartButton
// purpose :
// =======================================================================
QPushButton* TInspector_OpenButton::StartButton()
{
  if (!myStartButton)
  {
    myStartButton = new QPushButton();
    myStartButton->setIcon (QIcon (":folder_open.png"));
    connect (myStartButton, SIGNAL (clicked()), this, SLOT (onStartButtonClicked()));
  }
  return myStartButton;
}

// =======================================================================
// function : onStartButtonClicked
// purpose :
// =======================================================================
void TInspector_OpenButton::onStartButtonClicked()
{
  QPushButton* aButton = (QPushButton*)sender();
  TCollection_AsciiString aPluginName (aButton->objectName().toStdString().c_str());
  if (aPluginName.IsEmpty())
    return;
  
  QStringList aPluginRecentlyOpenedFiles;
  if (myRecentlyOpenedFiles.contains(aPluginName))
  {
    QStringList aFileNames = myRecentlyOpenedFiles[aPluginName];
    for (int i = 0; i < aFileNames.size(); i++)
    {
      QFileInfo aFileInfo (aFileNames[i]);
      if (aFileInfo.exists() && aFileInfo.isFile())
        aPluginRecentlyOpenedFiles.append(aFileInfo.absoluteFilePath());
    }
  }

  QString aFileName = TInspector_OpenFileDialog::OpenFile (0, aPluginRecentlyOpenedFiles);
  aFileName = QDir().toNativeSeparators (aFileName);
  if (!aFileName.isEmpty()) {
    QApplication::setOverrideCursor (Qt::WaitCursor);
    TInspector_OpenFileDialog::Communicator()->OpenFile (aPluginName, TCollection_AsciiString (aFileName.toUtf8().data()));

    QFileInfo aFileInfo (aFileName);
    if (!aPluginRecentlyOpenedFiles.contains (aFileInfo.absoluteFilePath()))
    {
      myRecentlyOpenedFiles[aPluginName].append (aFileInfo.absoluteFilePath());
      for (int i = 0; i < myRecentlyOpenedFiles[aPluginName].size() - RECENT_FILES_CACHE_SIZE; i++)
        myRecentlyOpenedFiles[aPluginName].removeFirst();
      TInspector_OpenFileDialog::SetPluginRecentlyOpenedFiles (aPluginName,
        TInspector_OpenFileDialog::Communicator(), myRecentlyOpenedFiles[aPluginName]);

      TInspector_OpenFileDialog::Communicator()->GetPluginParameters()->StorePreferences();
    }

    QApplication::restoreOverrideCursor();
  }
}
