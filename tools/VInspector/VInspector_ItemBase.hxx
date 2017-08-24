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

#ifndef VInspector_ItemBase_H
#define VInspector_ItemBase_H

#include <AIS_InteractiveContext.hxx>
#include <Standard.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

class VInspector_ItemBase;
typedef QExplicitlySharedDataPointer<VInspector_ItemBase> VInspector_ItemBasePtr;

//! \class VInspector_ItemBase
//! Parent item for all ShapeView items
class VInspector_ItemBase : public TreeModel_ItemBase
{
public:
  //! Resets cached values
  virtual void Reset() Standard_OVERRIDE { TreeModel_ItemBase::Reset(); }

  //! Sets the context 
  //! \param theLabel an object where the child items structure is found
  void SetContext (const Handle(AIS_InteractiveContext)& theContext) { myContext = theContext; }

  //! Returns true if the current context is not null
  //! \return a boolean value
  bool HasContext() const { return !GetContext().IsNull(); }

  //! Returns the current contex. It iterates up by list of parents to found context item and return context
  //! \return a context
  Standard_EXPORT const Handle(AIS_InteractiveContext)& GetContext() const;

protected:

  //! Initialize the current item. It creates a backup of the specific item information
  virtual void initItem() const {};// = 0;

protected:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : TreeModel_ItemBase (theParent, theRow, theColumn), myContext (0) {}

protected:

  Handle(AIS_InteractiveContext) myContext; //!< the current context
};

#endif