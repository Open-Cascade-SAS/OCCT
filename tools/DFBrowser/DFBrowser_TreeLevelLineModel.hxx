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

#ifndef DFBrowser_TreeLevelLineModel_H
#define DFBrowser_TreeLevelLineModel_H

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>

#include <QAbstractTableModel>
#include <QObject>
#include <QList>

//! \class DFBrowser_TreeLevelLineModel
//! Tree Model of tree line items. It is initialized by OCAF tree model index. Each element of the current model
//! is an item of hierarchy of OCAF tree model index. So, on each level a label is presented, the last element may be
//! an attribute. Information, presented for the item has no additional information (not as in OCAF tree model)
class DFBrowser_TreeLevelLineModel : public QAbstractTableModel
{
public:

  //! Constructor
  DFBrowser_TreeLevelLineModel (QObject* theParent = 0) : QAbstractTableModel (theParent) {}

  //! Destructor
  virtual ~DFBrowser_TreeLevelLineModel() {}

  //!
  void Reset() { myLevelItems.clear(); }

  //! Inits the model by the index
  //! \param theTreeIndex an OCAF tree model index
  Standard_EXPORT void Init (const QModelIndex& theTreeIndex);

  //! Returns true if the tree model index is filled
  bool IsInitialized() const { return myTreeIndex.isValid(); }

  //! Return OCAF tree view model index on level defined by column of the parameter index
  //! \param theIndex a tree level view model index
  //! \return model index
  const QModelIndex& GetTreeViewIndex (const QModelIndex& theIndex) const
  { return myLevelItems[theIndex.column()]; }

  //! Returns item information(short) for display role.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex,
                                         int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns the header data for the given role and section in the header with the specified orientation.
  //! \param theSection the header section. For horizontal headers - column number, for vertical headers - row number.
  //! \param theOrientation a header orientation
  //! \param theRole a data role
  //! \return the header data
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation,
                                               int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns number of tree level line items = colums in table view
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return myLevelItems.size(); }

  //! Returns onlly one row in table view
  virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 1; }

private:

  QModelIndex myTreeIndex; //!< the current OCAF tree view model index
  QList<QModelIndex> myLevelItems; //! cached parent indices of myTreeIndex for quick access to item information
};

#endif
