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

#ifndef VInspector_ItemSensitiveEntity_H
#define VInspector_ItemSensitiveEntity_H

#include <SelectMgr_SensitiveEntity.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

class SelectBasics_EntityOwner;
class VInspector_ItemSensitiveEntity;

typedef QExplicitlySharedDataPointer<VInspector_ItemSensitiveEntity> VInspector_ItemSensitiveEntityPtr;

//! \class VInspector_ItemSensitiveEntity
//! The item shows information about SelectBasics_EntityOwner.
//! The parent is item selection, children are item entity owners
class VInspector_ItemSensitiveEntity : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSensitiveEntityPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemSensitiveEntityPtr (new VInspector_ItemSensitiveEntity (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemSensitiveEntity() Standard_OVERRIDE {};

  //! \return the current sensitive entity
  Standard_EXPORT Handle(SelectMgr_SensitiveEntity) GetSensitiveEntity() const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE { return !GetSensitiveEntity()->BaseSensitive().IsNull() ? 1 : 0; }

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

  //! Returns owner of the current sensitive entity
  //! \return owner
  Handle(SelectBasics_EntityOwner) getEntityOwner() const;

private:

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemSensitiveEntity(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase(theParent, theRow, theColumn) {}

private:

  Handle(SelectMgr_SensitiveEntity) myEntity; //!< the current entity owner
};

#endif
