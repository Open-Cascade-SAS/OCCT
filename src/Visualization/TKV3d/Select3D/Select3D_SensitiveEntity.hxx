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

#include <Standard_Transient.hxx>
#include <Select3D_BndBox3d.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <TopLoc_Location.hxx>

class Graphic3d_TransformPers;
class SelectMgr_EntityOwner;

//! Abstract framework to define 3D sensitive entities.
class Select3D_SensitiveEntity : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveEntity, Standard_Transient)
public:
  //! Returns pointer to owner of the entity
  const Handle(SelectMgr_EntityOwner)& OwnerId() const { return myOwnerId; }

  //! Sets owner of the entity
  virtual void Set(const Handle(SelectMgr_EntityOwner)& theOwnerId) { myOwnerId = theOwnerId; }

  //! allows a better sensitivity for a specific entity in selection algorithms useful for small
  //! sized entities.
  Standard_Integer SensitivityFactor() const { return mySFactor; }

  //! Allows to manage sensitivity of a particular sensitive entity
  void SetSensitivityFactor(const Standard_Integer theNewSens)
  {
    Standard_ASSERT_RAISE(theNewSens >= 0,
                          "Error! Selection sensitivity should not be negative value.");
    mySFactor = theNewSens;
  }

  //! Originally this method intended to return sensitive entity with new location aLocation,
  //! but currently sensitive entities do not hold a location,
  //! instead HasLocation() and Location() methods call corresponding entity owner's methods.
  //! Thus all entities returned by GetConnected() share the same location propagated from
  //! corresponding selectable object. You must redefine this function for any type of sensitive
  //! entity which can accept another connected sensitive entity.
  virtual Handle(Select3D_SensitiveEntity) GetConnected()
  {
    return Handle(Select3D_SensitiveEntity)();
  }

  //! Checks whether sensitive overlaps current selecting volume.
  //! Stores minimum depth, distance to center of geometry and closest point detected into
  //! thePickResult
  virtual Standard_Boolean Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                   SelectBasics_PickResult&             thePickResult) = 0;

  //! Returns the number of sub-entities or elements in sensitive entity.
  //! Is used to determine if entity is complex and needs to pre-build BVH at the creation of
  //! sensitive entity step or is light-weighted so the tree can be build on demand with
  //! unnoticeable delay.
  virtual Standard_Integer NbSubElements() const = 0;

  //! Returns bounding box of a sensitive with transformation applied
  virtual Select3D_BndBox3d BoundingBox() = 0;

  //! Returns center of a sensitive with transformation applied
  virtual gp_Pnt CenterOfGeometry() const = 0;

  //! Builds BVH tree for a sensitive if needed
  virtual void BVH() {}

  //! Returns TRUE if BVH tree is in invalidated state
  virtual Standard_Boolean ToBuildBVH() const { return Standard_True; }

  //! Clears up all resources and memory
  virtual void Clear() { Set(Handle(SelectMgr_EntityOwner)()); }

  //! Returns true if the shape corresponding to the entity has init location
  virtual Standard_Boolean HasInitLocation() const { return Standard_False; }

  //! Returns inversed location transformation matrix if the shape corresponding to this entity has
  //! init location set. Otherwise, returns identity matrix.
  virtual gp_GTrsf InvInitLocation() const { return gp_GTrsf(); }

  //! Return transformation persistence.
  const Handle(Graphic3d_TransformPers)& TransformPersistence() const { return myTrsfPers; }

  //! Set transformation persistence.
  virtual void SetTransformPersistence(const Handle(Graphic3d_TransformPers)& theTrsfPers)
  {
    myTrsfPers = theTrsfPers;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const;

protected:
  Standard_EXPORT Select3D_SensitiveEntity(const Handle(SelectMgr_EntityOwner)& theOwnerId);

protected:
  Handle(SelectMgr_EntityOwner)   myOwnerId;
  Handle(Graphic3d_TransformPers) myTrsfPers;
  Standard_Integer                mySFactor;
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveEntity, Standard_Transient)

Standard_DEPRECATED("Deprecated alias - Select3D_SensitiveEntity should be used "
                    "instead")
typedef Select3D_SensitiveEntity SelectBasics_SensitiveEntity;

#endif // _Select3D_SensitiveEntity_HeaderFile
