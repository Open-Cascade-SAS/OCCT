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

#ifndef VInspector_ItemEntityOwner_H
#define VInspector_ItemEntityOwner_H

#include <AIS_InteractiveObject.hxx>
#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

class QItemSelectionModel;

class VInspector_ItemEntityOwner;
typedef QExplicitlySharedDataPointer<VInspector_ItemEntityOwner> VInspector_ItemEntityOwnerPtr;

//! \class VInspector_ItemPresentableObject
//! Item for selection entity owner. The parent is sensitive entity item, there are no children
class VInspector_ItemEntityOwner : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemEntityOwnerPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemEntityOwnerPtr (new VInspector_ItemEntityOwner (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemEntityOwner() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns the current entity owner
  Handle(SelectBasics_EntityOwner) EntityOwner() const { return myOwner; }

protected:
  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE{ return 0; }

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE
  { (void)theRow; (void)theColumn; return TreeModel_ItemBasePtr(); }

private:

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemEntityOwner(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase(theParent, theRow, theColumn) {}

private:

  //! Returns the current entity owner. Initializes the item if it was not initialized yet
  Handle(SelectBasics_EntityOwner) getEntityOwner() const;

private:

  Handle(SelectBasics_EntityOwner) myOwner; //!< the current entity owner
};

#endif
