// Created on: 2014-08-15
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_SensitiveEntitySet_HeaderFile
#define _SelectMgr_SensitiveEntitySet_HeaderFile

#include <BVH_PrimitiveSet3d.hxx>
#include <Select3D_BVHBuilder3d.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SensitiveEntity.hxx>
#include <SelectMgr_Selection.hxx>

typedef NCollection_IndexedMap<Handle(SelectMgr_SensitiveEntity)> SelectMgr_IndexedMapOfHSensitive;
typedef NCollection_DataMap<Handle(SelectMgr_EntityOwner), Standard_Integer> SelectMgr_MapOfOwners;

//! This class is used to store all calculated sensitive entities of one selectable object.
//! It provides an interface for building BVH tree which is used to speed-up
//! the performance of searching for overlap among sensitives of one selectable object
class SelectMgr_SensitiveEntitySet : public BVH_PrimitiveSet3d
{
  DEFINE_STANDARD_RTTIEXT(SelectMgr_SensitiveEntitySet, BVH_PrimitiveSet3d)
public:

  //! Empty constructor.
  Standard_EXPORT SelectMgr_SensitiveEntitySet (const Handle(Select3D_BVHBuilder3d)& theBuilder);

  virtual ~SelectMgr_SensitiveEntitySet() {}

  //! Adds new entity to the set and marks BVH tree for rebuild
  Standard_EXPORT void Append (const Handle(SelectMgr_SensitiveEntity)& theEntity);

  //! Adds every entity of selection theSelection to the set and marks
  //! BVH tree for rebuild
  Standard_EXPORT void Append (const Handle(SelectMgr_Selection)& theSelection);

  //! Removes every entity of selection theSelection from the set
  //! and marks BVH tree for rebuild
  Standard_EXPORT void Remove (const Handle(SelectMgr_Selection)& theSelection);

  //! Returns bounding box of entity with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIndex) const Standard_OVERRIDE;

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_PrimitiveSet3d::Box;

  //! Returns geometry center of sensitive entity index theIdx
  //! along the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIndex,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2
  Standard_EXPORT virtual void Swap (const Standard_Integer theIndex1,
                                     const Standard_Integer theIndex2) Standard_OVERRIDE;

  //! Returns the amount of entities
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns the entity with index theIndex in the set
  Standard_EXPORT const Handle(SelectMgr_SensitiveEntity)& GetSensitiveById (const Standard_Integer theIndex) const;

  //! Returns map of entities.
  const SelectMgr_IndexedMapOfHSensitive& Sensitives() const { return mySensitives; }

  //! Returns map of owners.
  const SelectMgr_MapOfOwners& Owners() const { return myOwnersMap; }

  //! Returns map of entities.
  Standard_Boolean HasEntityWithPersistence() const { return myHasEntityWithPersistence; }

protected:

  //! Adds entity owner to the map of owners (or increases its counter if it is already there).
  Standard_EXPORT void addOwner (const Handle(SelectMgr_EntityOwner)& theOwner);

  //! Decreases counter of owner in the map of owners (or removes it from the map if counter == 0).
  Standard_EXPORT void removeOwner (const Handle(SelectMgr_EntityOwner)& theOwner);

private:

  SelectMgr_IndexedMapOfHSensitive mySensitives; //!< Map of entities and its corresponding index in BVH
  SelectMgr_MapOfOwners            myOwnersMap;  //!< Map of entity owners and its corresponding number of sensitives
  Standard_Boolean  myHasEntityWithPersistence;  //!< flag if some of sensitive entity has own transform persistence
};

#endif // _SelectMgr_SensitiveEntitySet_HeaderFile
