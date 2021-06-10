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

#ifndef VInspector_ItemGraphic3dCLight_H
#define VInspector_ItemGraphic3dCLight_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <Graphic3d_CLight.hxx>

class VInspector_ItemGraphic3dCLight;
typedef QExplicitlySharedDataPointer<VInspector_ItemGraphic3dCLight> VInspector_ItemGraphic3dCLightPtr;

//! \class VInspector_ItemGraphic3dCLight
//! Parent item is context properties, that corresponds to AIS_InteractiveContext
class VInspector_ItemGraphic3dCLight : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemGraphic3dCLightPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemGraphic3dCLightPtr (new VInspector_ItemGraphic3dCLight (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemGraphic3dCLight() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual const Handle(Standard_Transient)& Object() const Standard_OVERRIDE { initItem(); return myLight; }

  //! Returns the current light, init item if it was not initialized yet
  //! \return interactive object
  Handle(Graphic3d_CLight) GetLight() const { return Handle(Graphic3d_CLight)::DownCast (Object()); }

  //! Updates item by the item properties value
  virtual void StoreItemProperties (const int theRow, const int theColumn, const QVariant& theValue) Standard_OVERRIDE;

protected:
  //! Initializes the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of displayed presentations
  //! \return rows count
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemGraphic3dCLight(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}

protected:

  Handle(Graphic3d_CLight) myLight; //!< the current light
};

#endif
