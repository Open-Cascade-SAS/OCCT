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

#ifndef DFBrowser_SearchLine_H
#define DFBrowser_SearchLine_H

#include <inspector/DFBrowser_SearchLineModel.hxx> // to include DFBrowser_SearchItemInfo

#include <Standard.hxx>
#include <TDocStd_Application.hxx>
#include <TDF_Label.hxx>

#include <QCompleter>
#include <QFrame>
#include <QLineEdit>

class DFBrowser_Module;
class DFBrowser_SearchLineModel;

class QAbstractItemModel;
class QLineEdit;
class QToolButton;
class QStringList;

//! \class DFBrowser_SearchLine
//! It contains a line edit control with auto completion and search button
class DFBrowser_SearchLine : public QFrame
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_SearchLine (QWidget* theParent);

  //! Destructor
  virtual ~DFBrowser_SearchLine() Standard_OVERRIDE {}

  //! Creates search line model filled by the module. It is necessary for auto completion of line edit
  //! \param theModule a current module
  Standard_EXPORT void SetModule (DFBrowser_Module* theModule);

  //! Returns the current module
  Standard_EXPORT DFBrowser_Module* GetModule();

  //! Fills the search line model by OCAF document values
  //! \param theDocumentValues container of document index to container of entry/attribute name to item information
  //! \param theDocumentInfoValues container of a document index to entry/attribute name
  Standard_EXPORT void SetValues (const QMap<int, QMap<QString, DFBrowser_SearchItemInfo > >& theDocumentValues,
                                  const QMap<int, QStringList>& theDocumentInfoValues);

  //! Clears cache of values in search line model
  Standard_EXPORT void ClearValues();

  //! Returns completer model
  Standard_EXPORT QAbstractItemModel* GetModel() { return myLineControl->completer()->model(); }

  //! Returns completion completer model
  Standard_EXPORT QAbstractItemModel* GetCompletionModel() { return myLineControl->completer()->completionModel(); }

  //! Returns the current line edit text
  QString Text() const { return myLineControl->text(); }

  //! Sets the current text value
  //! \param theValue a string value
  void SetText (const QString& theValue) { myLineControl->setText (theValue); }

signals:

  //! Signals that is emitted by text changed in line edit control
  void searchActivated();

private slots:

  //! Updates icon of search button depending on text is empty and emits searchActivated signal
  void onTextChanged (const QString& theText);
  //! Set completion prefix in completer model
  void onReturnPressed() { myLineControl->completer()->setCompletionPrefix (myLineControl->text()); }
  //! Set empty text if the current text is not empty: new search is started
  void onSearchButtonClicked();

private:

  QLineEdit* myLineControl; //!< line editor control
  QToolButton* mySearchButton; //! < search button
};

#endif
