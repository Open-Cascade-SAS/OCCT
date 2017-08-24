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

#ifndef DFBrowser_SearchView_H
#define DFBrowser_SearchView_H

#include <inspector/DFBrowser_SearchLine.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>

#include <QItemSelection>
#include <QObject>

class DFBrowser_SearchLine;
class QTableView;
class QWidget;

//! \class DFBrowser_SearchView
//! Container of search result. It has a table of values
class DFBrowser_SearchView : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_SearchView (QWidget* theParent);

  //! Destructor
  virtual ~DFBrowser_SearchView() Standard_OVERRIDE {}

  //! Returns search parent control
  QWidget* GetControl() const { return myMainWindow; }

  //! Sets search line to connect to the search line completion model
  void SetSearchLine (DFBrowser_SearchLine* theSearchLine) { mySearchLine = theSearchLine; }

  //! Fills the table by values of search line completion model. The zero column of the table is hidden,
  //! so it visualizes values of 1st and 2nd columns of this model. It creates selection model and connects
  //! to selectionChanged and doubleClicked signals
  Standard_EXPORT void InitModels();

  //! Resets search line
  void Reset() { mySearchLine->SetText (""); }

signals:

  //! Signal about selecting of an item in the view.
  //! \param thePath path to the selected item (e.g. 0, 0:1, 0:1:1)
  //! \param theValue value of the selected item (e.g. TDataStd_Name)
  void pathSelected (const QStringList& thePath, const QString& theValue);

  //! Signal about double click on an item in the view.
  //! \param thePath path to the selected item (e.g. 0, 0:1, 0:1:1)
  //! \param theValue value of the selected item (e.g. TDataStd_Name)
  void pathDoubleClicked (const QStringList& thePath, const QString& theValue);

protected slots:

  //! Listens selection change and emits the pathSelected signal
  //! \param theSelected selected items
  //! \param theDeselected deselected items
  void onTableSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Listens double click signal on table view
  //! \theIndex a model index of double clicked item
  void onTableDoubleClicked (const QModelIndex& theIndex);

private:

  QWidget* myMainWindow; //!< control where table view is placed
  QTableView* myTableView; //!< table view to visualize search values
  DFBrowser_SearchLine* mySearchLine; //!< search line to have access to search model completor
};
#endif
