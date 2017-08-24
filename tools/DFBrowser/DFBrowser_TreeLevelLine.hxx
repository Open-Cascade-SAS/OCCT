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

#ifndef DFBrowser_TreeLevelLine_H
#define DFBrowser_TreeLevelLine_H

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>

#include <QItemSelection>
#include <QObject>
#include <QMap>
#include <QModelIndex>

class DFBrowser_SearchLine;

class QAbstractItemModel;
class QToolButton;
class QTableView;
class QWidget;

//! \class DFBrowser_TreeLevelLine
//! This is a control to visualize the current selected item of OCAF tree view.
//! It contains history of previous selected items.
//! The structure of this control is the next:
//! <back button> <forward button> <selected item line> <update button> <search line>
//! - <back button> allows moving to previously selected item (if it exists)
//! - <forward button> allows moving to next selected item (if it exists)
//! - <selected item line> path to currently selected item in OCAF tree view. Click on any level will
//! select the clicked item in tree view.
//! - <update button> will update content of OCAF tree model
//! - <search line> allows type label entry or attribute name and see in search view the available elements
class DFBrowser_TreeLevelLine : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_TreeLevelLine (QWidget* theParent);

  //! Destructor
  virtual ~DFBrowser_TreeLevelLine() Standard_OVERRIDE {}

  //! Clears history of selected items
  Standard_EXPORT void ClearHistory();

  //! Returns parent control
  QWidget* GetControl() const { return myMainWindow; }

  //! Returns current search line
  DFBrowser_SearchLine* GetSearchLine() const { return mySearchLine; };

signals:

  //! Signal about selection of an item in tree level line
  //! \param theIndex a tree view model index of selected item
  void indexSelected (const QModelIndex& theIndex);

  //! Signal about necessity to update OCAF tree model
  void updateClicked();

public slots:

  //! Listens tree view selection model. Update tree level line by selected item. Stores
  //! \param theSelected a list of selected items
  //! \param theDeselected a list of deselected items
  Standard_EXPORT void OnTreeViewSelectionChanged (const QItemSelection& theSelected,
                                                   const QItemSelection& theDeselected);

private slots:

  //! Listens table view selection model. Gets selected tree view model index and emit indexSelected signal.
  //! \param theSelected a list of selected items
  //! \param theDeselected a list of deselected items
  void onTableSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Listens actions and do the following:
  //! - <backward>: moves history to the previous selection item, emits indexSelected signal
  //! - <forward>: moves history to the next selection item, emits indexSelected signal
  //! - <update>: emits updateClicked signal to update OCAF tree model
  void onActionClicked();

private:

  //! Returns the previous index of history selection
  QModelIndex getBackwardIndex();

  //! Returns the next index of history selection
  QModelIndex getForwardIndex();

  //! Appends new history index, set it active, remove the first history index if the cache is out of range
  //! \param theIndex a selected OCAF tree model index
  void setForwardIndex (const QModelIndex& theIndex);

  //! Updates enable state of backward/forward actions depending on the current item index
  void updateActionsState();

  //! Sets the current index and update actions state
  //! \param theIndexId an item index, should be in range of history of indices
  void setCurrentHistoryIndex (const int theIndexId);

private:

  bool mySelectionProcessingBlocked; //!< if true, table view selection is started but has not been finished yet
  QList<QModelIndex> myHistoryIndices; //!< cached selected OCAF tree model indices
  int myCurrentHistoryIndex; //!< an index of the current history selected index from myHistoryIndices

  QWidget* myMainWindow; //!< parent widget for controls
  QToolButton* myBackwardButton; //!< backward button, to select previous selected item
  QToolButton* myForwardButton; //!< forward button, to select next selected item
  QToolButton* myUpdateButton; //!< update button, like F5, to update OCAF tree model content
  QTableView* myTableView; //!< container of path values to selected item, a path value is a label entry

  DFBrowser_SearchLine* mySearchLine; //!< line of search
};
#endif
