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

#ifndef DFBrowser_TreeLevelViewModel_H
#define DFBrowser_TreeLevelViewModel_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QAbstractTableModel>
#include <QVariant>

class QObject;

//! \class DFBrowser_TreeLevelViewModel
//! Tree Model of one level of OCAF tree view model. It is initialized by tree view index and
//! contains children and attributes of this label.
class DFBrowser_TreeLevelViewModel : public QAbstractTableModel
{
public:

  //! Constructor
  DFBrowser_TreeLevelViewModel (QObject* theParent) : QAbstractTableModel (theParent), myRowCount (0) {}

  //! Destructor
  virtual ~DFBrowser_TreeLevelViewModel() Standard_OVERRIDE {}

  //! Reset OCAF tree model index
  void Reset() { myIndex = QModelIndex(); }

  //! Fills OCAF tree model index
  //! \param theTreeIndex an index
  Standard_EXPORT void Init (const QModelIndex& theTreeIndex);

  //! Returns true if the index is filled
  bool IsInitialized() const { return myIndex.isValid(); }

  //! Return OCAF tree view model index on level defined by column of the parameter index
  //! \param theIndex a tree level view model index
  //! \return model index
  Standard_EXPORT QModelIndex GetTreeViewIndex (const QModelIndex& theIndex) const;

  //! Emits the layoutChanged signal from outside of this class
  void EmitLayoutChanged() { emit layoutChanged(); }

  //! It returns value only for DisplayRole for column = 1
  //! \param theSection an index of value in the container 
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation,
                                               int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Creates new model index
  //! \param theRow the index row position
  //! \param theColummn the index column position
  //! \param theParent the parent index
  //! \return the model index
  Standard_EXPORT virtual QModelIndex index (int theRow, int theColumn,
                                             const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns item information(short) for display role.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex,
                                         int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns Enabled and Selectable item for any index
  //! \param theIndex a model index
  //! \return flags
  Standard_EXPORT virtual Qt::ItemFlags flags (const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Returns number of rows
  virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return myRowCount; }

  //! Returns 2 columns
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 2; }

private:

  QModelIndex myIndex; //!< OCAF tree view model index
  int myRowCount; //!< number of rows of item of treeview model index
};
#endif
