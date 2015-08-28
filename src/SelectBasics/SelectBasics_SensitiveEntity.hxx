// Created on: 1995-01-23
// Created by: Mister rmi
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

#ifndef _SelectBasics_SensitiveEntity_HeaderFile
#define _SelectBasics_SensitiveEntity_HeaderFile

#include <gp_Trsf.hxx>

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <SelectBasics_SelectingVolumeManager.hxx>
#include <SelectBasics_PickResult.hxx>
#include <Standard_Integer.hxx>
#include <Select3D_BndBox3d.hxx>

class SelectBasics_EntityOwner;


class SelectBasics_SensitiveEntity;
DEFINE_STANDARD_HANDLE(SelectBasics_SensitiveEntity, MMgt_TShared)

//! root class; the inheriting classes will be able to give
//! sensitive Areas for the dynamic selection algorithms
class SelectBasics_SensitiveEntity : public MMgt_TShared
{

public:

  
  //! Sets owner of the entity
  Standard_EXPORT virtual void Set (const Handle(SelectBasics_EntityOwner)& theOwnerId);
  
  //! Returns pointer to owner of the entity
  Standard_EXPORT const Handle(SelectBasics_EntityOwner)& OwnerId() const;
  
  //! Checks whether the sensitive entity is overlapped by
  //! current selecting volume
  virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr, SelectBasics_PickResult& thePickResult) = 0;
  
  //! allows a better sensitivity for
  //! a specific entity in selection algorithms
  //! useful for small sized entities.
  Standard_EXPORT Standard_Integer SensitivityFactor() const;

  //! Allows to manage sensitivity of a particular sensitive entity
  Standard_EXPORT void SetSensitivityFactor (const Standard_Integer theNewSens);
  
  //! Returns the number of sub-entities or elements in
  //! sensitive entity. Is used to determine if entity is
  //! complex and needs to pre-build BVH at the creation of
  //! sensitive entity step or is light-weighted so the tree
  //! can be build on demand with unnoticeable delay
  virtual Standard_Integer NbSubElements() = 0;
  
  //! Returns bounding box of sensitive entity
  virtual Select3D_BndBox3d BoundingBox() = 0;
  
  //! Builds BVH tree for sensitive if it is needed
  virtual void BVH() = 0;
  
  //! Clears up all the resources and memory allocated
  virtual void Clear() = 0;

  //! Returns true if the shape corresponding to the entity has init location
  virtual Standard_Boolean HasInitLocation() const = 0;

  //! Returns inversed location transformation matrix if the shape corresponding
  //! to this entity has init location set. Otherwise, returns identity matrix.
  virtual gp_Trsf InvInitLocation() const = 0;

  DEFINE_STANDARD_RTTI(SelectBasics_SensitiveEntity,MMgt_TShared)

protected:

  Standard_EXPORT SelectBasics_SensitiveEntity (const Handle(SelectBasics_EntityOwner)& theOwnerId);

  Handle(SelectBasics_EntityOwner) myOwnerId;


private:

  Standard_Integer mySFactor;
};


#include <SelectBasics_SensitiveEntity.lxx>





#endif // _SelectBasics_SensitiveEntity_HeaderFile
