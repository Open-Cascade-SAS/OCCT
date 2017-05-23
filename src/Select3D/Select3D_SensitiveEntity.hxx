// Created on: 1995-03-13
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Select3D_SensitiveEntity_HeaderFile
#define _Select3D_SensitiveEntity_HeaderFile

#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <TopLoc_Location.hxx>

class SelectBasics_EntityOwner;

//!  Abstract framework to define 3D sensitive entities.
//! As the selection process uses the principle of a
//! projection of 3D shapes onto a 2D view where
//! nearness to a rectangle determines whether a shape
//! is picked or not, all 3D shapes need to be converted
//! into 2D ones in order to be selected.
class Select3D_SensitiveEntity : public SelectBasics_SensitiveEntity
{
public:

  //! Originally this method intended to return sensitive
  //! entity with new location aLocation, but currently sensitive
  //! entities do not hold a location, instead HasLocation() and
  //! Location() methods call corresponding entity owner's methods.
  //! Thus all entities returned by GetConnected() share the same
  //! location propagated from corresponding selectable object.
  //! You must redefine this function for any type of
  //! sensitive entity which can accept another connected
  //! sensitive entity.//can be connected to another sensitive entity.
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected();

  //! Checks whether sensitive overlaps current selecting volume.
  //! Stores minimum depth, distance to center of geometry and
  //! closest point detected into thePickResult
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Returns the number of sub-entities or elements in
  //! sensitive entity. Is used to determine if entity is
  //! complex and needs to pre-build BVH at the creation of
  //! sensitive entity step or is light-weighted so the tree
  //! can be build on demand with unnoticeable delay
  virtual Standard_Integer NbSubElements() Standard_OVERRIDE = 0;

  //! Returns bounding box of a sensitive with transformation applied
  virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE = 0;

  //! Returns center of a sensitive with transformation applied
  virtual gp_Pnt CenterOfGeometry() const = 0;

  //! Builds BVH tree for a sensitive if needed
  Standard_EXPORT virtual void BVH() Standard_OVERRIDE;

  //! Clears up all resources and memory
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Returns true if the shape corresponding to the entity has init location
  Standard_EXPORT virtual Standard_Boolean HasInitLocation() const Standard_OVERRIDE;

  //! Returns inversed location transformation matrix if the shape corresponding
  //! to this entity has init location set. Otherwise, returns identity matrix.
  Standard_EXPORT virtual gp_GTrsf InvInitLocation() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveEntity,SelectBasics_SensitiveEntity)

protected:

  Standard_EXPORT Select3D_SensitiveEntity (const Handle(SelectBasics_EntityOwner)& theOwnerId);
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveEntity, SelectBasics_SensitiveEntity)

#endif // _Select3D_SensitiveEntity_HeaderFile
