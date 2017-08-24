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

#ifndef DFBrowser_TreeLevelView_H
#define DFBrowser_TreeLevelView_H

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>

#include <QObject>
#include <QItemSelection>

class QWidget;
class QTableView;

//! \class DFBrowser_TreeLevelView
//! This is a control to visualize the current selected item of OCAF tree view in table view.
//! The table contains two colums: name and value. The information is similar the one OCAF tree view label
//! content: children and attributes for the current label. Selection or double click of item emits
//! signals about this event.
class DFBrowser_TreeLevelView : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_TreeLevelView (QWidget* theParent);

  //! Destructor
  virtual ~DFBrowser_TreeLevelView() Standard_OVERRIDE {}

  //! Returns parent control
  QWidget* GetControl() const { return myMainWindow; }

  //! Clear selection of the table view selection model
  Standard_EXPORT void ClearSelection();

  //! Returns true if this control may be filled by the index
  //! It is possible if an item of the index is application, document or label
  //! \param theIndex OCAF tree view model index
  //! \return boolean result
  Standard_EXPORT static bool ProcessItem (const QModelIndex& theIndex);

  //! Init view by the first selected item in OCAF tree view
  //! \param theSelected selected items
  //! \param theDeselected deselected items
  Standard_EXPORT void UpdateByTreeSelectionChanged (const QItemSelection& theSelected,
                                                     const QItemSelection& theDeselected);

signals:

  //! Signal about selection of an item in tree view
  //! \param theIndex a tree view model index of selected item
  void indexSelected (const QModelIndex& theIndex);

  //! Signal about double click on an item in tree view
  //! \param theIndex a tree view model index of selected item
  void indexDoubleClicked (const QModelIndex& theIndex);

private slots:

  //! Listens table view selection model. Gets the first selected tree view model index and emit indexSelected signal.
  //! \param theSelected a list of selected items
  //! \param theDeselected a list of deselected items
  void onTableSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Listens table view double click. Gets clicked index and emit indexDoubleClicked signal.
  //! \param theIndex a tree view model index of selected item
  void onTableDoubleClicked (const QModelIndex& theIndex);

private:

  QWidget* myMainWindow; //!< parent control
  QTableView* myTableView; //!< current view
};
#endif
