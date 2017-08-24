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

#ifndef ShapeView_ItemBase_H
#define ShapeView_ItemBase_H

#include <Standard.hxx>
#include <TopoDS_Shape.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

class ShapeView_ItemBase;
typedef QExplicitlySharedDataPointer<ShapeView_ItemBase> ShapeView_ItemBasePtr;

//! \class ShapeView_ItemBase
// \brief Declaration of the tree model base item.
class ShapeView_ItemBase : public TreeModel_ItemBase
{
public:

  //! Resets cached values
  virtual void Reset() Standard_OVERRIDE { TreeModel_ItemBase::Reset(); }

protected:

  //! Initialize the current item. It creates a backup of the specific item information
  virtual void initItem() const {};

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  ShapeView_ItemBase(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : TreeModel_ItemBase (theParent, theRow, theColumn) {}
};

#endif