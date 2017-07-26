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

#ifndef ShapeView_OpenFileDialog_H
#define ShapeView_OpenFileDialog_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QDialog>
#include <QItemSelection>
#include <QStringList>

#include <map>

class QAbstractItemModel;
class QLineEdit;
class QPushButton;
class QTableView;
class QToolButton;
class QWidget;

//! \class ShapeView_OpenButton
//! Class that contains push button and the button processing. It obtains a file name from the default or current
//! directory and gives the name into TInspector communicator
//! Object name of the button is the name of the plugin to get the default directory, or the current directory is used.
class ShapeView_OpenButton : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  ShapeView_OpenButton (QObject* theParent) : QObject (theParent), myStartButton (0) {}

  //! Destructor
  virtual ~ShapeView_OpenButton() {}

  //! Returns the start button, if this is the first call, it creates the button and connect it to the slot
  Standard_EXPORT QPushButton* StartButton();

  //! Sets the default directory of plugin.
  void SetPluginDir (const TCollection_AsciiString& thePluginName, const TCollection_AsciiString& theDefaultDir)
  { myDefaultDirs[thePluginName] = theDefaultDir; }

private slots:

  //! Processes the button click, open default/current directory to select open file, calls OpenFile of communicator
  void onStartButtonClicked();

signals:

  //! Signals about opening file clicked
  //! \param theFileName an output file name
  void OpenFile (const QString& theFileName);

private:

  QPushButton* myStartButton; //!< processed button
  std::map<TCollection_AsciiString, TCollection_AsciiString> myDefaultDirs; //!< plugins default directories
};

//! \class ShapeView_OpenFileDialog
//! Control that contains table view of samples and line to select a file name from other directory.
//! Click on element of samples table view calls this sample opening else after entering(or opening) file name
//! the import becomes active. Click on the button will open selected file if it is possible
class ShapeView_OpenFileDialog : public QDialog
{
  Q_OBJECT
private:

  //! Constructor
  ShapeView_OpenFileDialog (QWidget* theParent, const QString& theDataDirName);

public:

  //! Destructor
  virtual ~ShapeView_OpenFileDialog() Standard_OVERRIDE {}

  //! Opens this file dialog using for samples view the given directory and try to open new file
  //! \param theParent a parent for the new dialog
  //! \param theDataDirName path to default samples directory
  //! \returns a file name from the open file dialog
  static QString OpenFile (QWidget* theParent, const QString& theDataDirName);

  //! Returns selection name from the dialog
  QString GetFileName() const { return myFileName; }

private slots:

  //! Stores name of selected sample file
  void onSampleSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Updates enabling state of Open file button, it is enabled if the file by the entered path exists
  //! \param theText a file name text in line edit
  void onNameChanged (const QString& theText);

  //! Open file dialog to select a file name. Fills file name line, enable import button
  void onSelectClicked();

  //! Accepts open file dialog
  void onApplySelectClicked();

private:

  //! Creates view of file names in samples directory
  //! \param theFileNames a container of names
  //! \return table view
  QTableView* createTableView (const QStringList& theFileNames);

  //! Creates view model and fills it by the file names
  //! \param theFileNames a container of names
  //! \return model
  QAbstractItemModel* createModel (const QStringList& theFileNames);

  //! Generates container of file names in samples directory
  //! \return container of names
  QStringList readSampleNames();

private:

  QString myDataDir; //!< samples directory
  QString myFileName; //!< result file name
  QTableView* mySamplesView; //! <view of sample file names
  QLineEdit* mySelectedName; //!< alternative control to open file
  QToolButton* myFolderApplyOpen; //! button to open file
};

#endif
