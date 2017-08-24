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

#ifndef VInspector_ItemHistoryType_H
#define VInspector_ItemHistoryType_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

class VInspector_ItemHistoryType;
typedef QExplicitlySharedDataPointer<VInspector_ItemHistoryType> VInspector_ItemHistoryTypePtr;

//! \class VInspector_ItemHistoryType
//! Item to show history type. Also it shows the element information of the first sub-item
//! Parent is item history root item, children are item history elements
class VInspector_ItemHistoryType : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemHistoryTypePtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemHistoryTypePtr (new VInspector_ItemHistoryType (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemHistoryType() Standard_OVERRIDE {};

  //! Returns the pointer info of the first child item, initialize item if it has not been initialized yet.
  Standard_EXPORT QString PointerInfo() const;

  //! Returns the owner info of the first child item, initialize item if it has not been initialized yet.
  Standard_EXPORT QString OwnerInfo() const;

protected:

  //! Return data value for the role.
  //! \param theRole a value role
  //! \return the value
  virtual QVariant initValue(const int theRole) const;

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE;

    //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild(int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemHistoryType(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}

};

#endif
