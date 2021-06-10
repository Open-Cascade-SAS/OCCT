// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef MessageModel_ItemBase_H
#define MessageModel_ItemBase_H

#include <Standard.hxx>
#include <TopoDS_Shape.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

class MessageModel_ItemBase;
typedef QExplicitlySharedDataPointer<MessageModel_ItemBase> MessageModel_ItemBasePtr;

//! \class MessageModel_ItemBase
// \brief Declaration of the tree model base item.
class MessageModel_ItemBase : public TreeModel_ItemBase
{
public:

  //! Resets cached values
  virtual void Reset() Standard_OVERRIDE { TreeModel_ItemBase::Reset(); }

protected:

  //! Initialize the current item. It creates a backup of the specific item information
  virtual void initItem() const Standard_OVERRIDE {};

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  MessageModel_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : TreeModel_ItemBase (theParent, theRow, theColumn) {}

  //! Return root item
  //! \return an item instance
  TreeModel_ItemBasePtr GetRootItem() const;
};

#endif