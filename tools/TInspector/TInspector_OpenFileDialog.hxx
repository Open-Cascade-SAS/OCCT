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

#ifndef TInspectorEXE_OpenFileDialog_H
#define TInspectorEXE_OpenFileDialog_H

#include <inspector/TInspectorAPI_PreferencesDataMap.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <TCollection_AsciiString.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QDialog>
#include <QItemSelection>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

class TInspector_Communicator;

class QAbstractItemModel;
class QLineEdit;
class QPushButton;
class QTableView;
class QToolButton;
class QWidget;

//! \class TInspector_OpenFileDialog
//! Control that contains table view of samples and line to select a file name from other directory.
//! Click on element of samples table view calls this sample opening else after entering(or opening) file name
//! the import becomes active. Click on the button will open selected file if it is possible
class TInspector_OpenFileDialog : public QDialog
{
  Q_OBJECT
private:

  //! Constructor
  Standard_EXPORT TInspector_OpenFileDialog (QWidget* theParent, const QStringList& theRecentlyOpenedFiles);

public:

  //! Destructor
  virtual ~TInspector_OpenFileDialog() Standard_OVERRIDE {}

  //! Opens this file dialog using for samples view the given directory and try to open new file
  //! \param theParent a parent for the new dialog
  //! \param theDataDirName path to default samples directory
  //! \returns a file name from the open file dialog
  Standard_EXPORT static QString OpenFile (QWidget* theParent, const QStringList& theRecentlyOpenedFiles);

  //! Returns selection name from the dialog
  QString GetFileName() const { return myFileName; }

  //! Returns communicator, if this is the first call, create a communicator instance
  Standard_EXPORT static TInspector_Communicator* Communicator();

  //! Returns preferences: previous opened documents.
  //! \param thePluginName name of the plugin
  //! \param theCommunicator source of preferences
  //! \param theFileNames [out] container of recently opened file names
  Standard_EXPORT static void GetPluginRecentlyOpenedFiles (const TCollection_AsciiString& thePluginName,
                                                            TInspector_Communicator* theCommunicator,
                                                            QStringList& theFileNames);

  //! Sets preferences: previous opened documents.
  //! \param thePluginName name of the plugin
  //! \param theCommunicator source of preferences
  //! \param theFileNames container of recently opened file names to be set into communicator preferences
  Standard_EXPORT static void SetPluginRecentlyOpenedFiles (const TCollection_AsciiString& thePluginName,
                                                            TInspector_Communicator* theCommunicator,
                                                            QStringList& theFileNames);

private slots:

  //! Stores name of selected sample file
  void onSampleSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Opens file dialog to select a file name. Fills file name line, enable import button
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

private:

  QStringList myRecentlyOpenedFiles; //!< recently opened files
  QString myFileName; //!< result file name
  QTableView* mySamplesView; //!< view of sample file names
  QLineEdit* mySelectedName; //!< alternative control to open file
};

#endif
