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

//! This class is used to store all calculated sensitive entities of one selectable object.
//! It provides an interface for building BVH tree which is used to speed-up
//! the performance of searching for overlap among sensitives of one selectable object
class SelectMgr_SensitiveEntitySet : public BVH_PrimitiveSet3d
{
  DEFINE_STANDARD_RTTIEXT(SelectMgr_SensitiveEntitySet, BVH_PrimitiveSet3d)
public:
  //! Empty constructor.
  Standard_EXPORT SelectMgr_SensitiveEntitySet(const occ::handle<Select3D_BVHBuilder3d>& theBuilder);

  virtual ~SelectMgr_SensitiveEntitySet() {}

  //! Adds new entity to the set and marks BVH tree for rebuild
  Standard_EXPORT void Append(const occ::handle<SelectMgr_SensitiveEntity>& theEntity);

  //! Adds every entity of selection theSelection to the set and marks
  //! BVH tree for rebuild
  Standard_EXPORT void Append(const occ::handle<SelectMgr_Selection>& theSelection);

  //! Removes every entity of selection theSelection from the set
  //! and marks BVH tree for rebuild
  Standard_EXPORT void Remove(const occ::handle<SelectMgr_Selection>& theSelection);

  //! Returns bounding box of entity with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box(const int theIndex) const
    override;

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_PrimitiveSet3d::Box;

  //! Returns geometry center of sensitive entity index theIdx
  //! along the given axis theAxis
  Standard_EXPORT virtual double Center(const int theIndex,
                                               const int theAxis) const
    override;

  //! Swaps items with indexes theIdx1 and theIdx2
  Standard_EXPORT virtual void Swap(const int theIndex1,
                                    const int theIndex2) override;

  //! Returns the amount of entities
  Standard_EXPORT virtual int Size() const override;

  //! Returns the entity with index theIndex in the set
  Standard_EXPORT const occ::handle<SelectMgr_SensitiveEntity>& GetSensitiveById(
    const int theIndex) const;

  //! Returns map of entities.
  const NCollection_IndexedMap<occ::handle<SelectMgr_SensitiveEntity>>& Sensitives() const { return mySensitives; }

  //! Returns map of owners.
  const NCollection_DataMap<occ::handle<SelectMgr_EntityOwner>, int>& Owners() const { return myOwnersMap; }

  //! Returns map of entities.
  bool HasEntityWithPersistence() const { return myNbEntityWithPersistence > 0; }

protected:
  //! Adds entity owner to the map of owners (or increases its counter if it is already there).
  Standard_EXPORT void addOwner(const occ::handle<SelectMgr_EntityOwner>& theOwner);

  //! Decreases counter of owner in the map of owners (or removes it from the map if counter == 0).
  Standard_EXPORT void removeOwner(const occ::handle<SelectMgr_EntityOwner>& theOwner);

private:
  // clang-format off
  NCollection_IndexedMap<occ::handle<SelectMgr_SensitiveEntity>> mySensitives;              //!< Map of entities and its corresponding index in BVH
  NCollection_DataMap<occ::handle<SelectMgr_EntityOwner>, int>            myOwnersMap;               //!< Map of entity owners and its corresponding number of sensitives
  int                 myNbEntityWithPersistence; //!< number of sensitive entities that have own transform persistence
  // clang-format on
};

#endif // _SelectMgr_SensitiveEntitySet_HeaderFile
