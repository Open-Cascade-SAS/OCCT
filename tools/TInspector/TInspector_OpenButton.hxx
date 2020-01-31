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

#ifndef TInspectorEXE_OpenButton_H
#define TInspectorEXE_OpenButton_H

#include <TCollection_AsciiString.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QMap>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

class QPushButton;

//! \class TInspector_OpenButton
//! Class that contains push button and the button processing. It obtains a file name from the default or current
//! directory and gives the name into TInspector communicator
//! Object name of the button is the name of the plugin to get the default directory, or the current directory is used.
class TInspector_OpenButton : public QObject
{
  Q_OBJECT

public:

  //! Constructor
  Standard_EXPORT TInspector_OpenButton (QObject* theParent);

  //! Destructor
  virtual ~TInspector_OpenButton() {}

  //! Returns the start button, if this is the first call, it creates the button and connect it to the slot
  Standard_EXPORT QPushButton* StartButton();

  //! Sets the default directory of plugin.
  void SetPluginRecentlyOpenedFiles (const TCollection_AsciiString& thePluginName,
                                     const QStringList& theRecentlyOpenedFiles)
  { myRecentlyOpenedFiles[thePluginName] = theRecentlyOpenedFiles; }

private slots:

  //! Processes the button click, open default/current directory to select open file, calls OpenFile of communicator
  void onStartButtonClicked();

private:

  QPushButton* myStartButton; //!< processed button
  //!< plugins recently opened files
  QMap<TCollection_AsciiString, QStringList> myRecentlyOpenedFiles;
};

#endif
