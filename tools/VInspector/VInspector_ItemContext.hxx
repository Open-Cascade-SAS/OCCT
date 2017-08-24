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

#ifndef VInspector_ItemContext_H
#define VInspector_ItemContext_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

class VInspector_ItemContext;
typedef QExplicitlySharedDataPointer<VInspector_ItemContext> VInspector_ItemContextPtr;

//! \class VInspector_ItemContext
//! Parent item, that corresponds to AIS_InteractiveContext
class VInspector_ItemContext : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemContextPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemContextPtr (new VInspector_ItemContext (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemContext() Standard_OVERRIDE {};

  //! Returns number of displayed presentations
  //! \return rows count
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

protected:

  //! Initialize the current item. It creates a backup of the specific item information
  //! Do nothing as context has been already set into item
  virtual void initItem() const Standard_OVERRIDE {}

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemContext(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}
};

#endif
