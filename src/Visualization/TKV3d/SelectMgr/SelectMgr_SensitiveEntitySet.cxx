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

#include <SelectMgr_SensitiveEntitySet.hxx>

#include <Graphic3d_TransformPers.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_SensitiveEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_SensitiveEntitySet, BVH_PrimitiveSet3d)

//=================================================================================================

SelectMgr_SensitiveEntitySet::SelectMgr_SensitiveEntitySet(
  const Handle(Select3D_BVHBuilder3d)& theBuilder)
    : BVH_PrimitiveSet3d(theBuilder)
{
  myNbEntityWithPersistence = 0;
}

//=======================================================================
// function : Append
// purpose  : Adds new entity to the set and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SensitiveEntitySet::Append(const Handle(SelectMgr_SensitiveEntity)& theEntity)
{
  if (!theEntity->BaseSensitive()->IsKind(STANDARD_TYPE(Select3D_SensitiveEntity)))
  {
    theEntity->ResetSelectionActiveStatus();
    return;
  }
  const Standard_Integer anExtent = mySensitives.Extent();
  if (mySensitives.Add(theEntity) > anExtent)
  {
    addOwner(theEntity->BaseSensitive()->OwnerId());
  }
  if (!theEntity->BaseSensitive()->TransformPersistence().IsNull())
  {
    ++myNbEntityWithPersistence;
  }
  MarkDirty();
}

//=======================================================================
// function : Append
// purpose  : Adds every entity of selection theSelection to the set
//            and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SensitiveEntitySet::Append(const Handle(SelectMgr_Selection)& theSelection)
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    const Handle(SelectMgr_SensitiveEntity)& aSensEnt = aSelEntIter.Value();
    if (!aSensEnt->BaseSensitive()->IsKind(STANDARD_TYPE(Select3D_SensitiveEntity)))
    {
      aSensEnt->ResetSelectionActiveStatus();
      continue;
    }

    const Standard_Integer anExtent = mySensitives.Extent();
    if (mySensitives.Add(aSensEnt) > anExtent)
    {
      addOwner(aSensEnt->BaseSensitive()->OwnerId());
    }
    if (!aSensEnt->BaseSensitive()->TransformPersistence().IsNull())
    {
      ++myNbEntityWithPersistence;
    }
  }
  MarkDirty();
}

//=======================================================================
// function : Remove
// purpose  : Removes every entity of selection theSelection from the set
//            and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SensitiveEntitySet::Remove(const Handle(SelectMgr_Selection)& theSelection)
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    const Handle(SelectMgr_SensitiveEntity)& aSensEnt = aSelEntIter.Value();
    const Standard_Integer                   anEntIdx = mySensitives.FindIndex(aSensEnt);
    if (anEntIdx == 0)
    {
      continue;
    }

    if (anEntIdx != mySensitives.Size())
    {
      Swap(anEntIdx - 1, mySensitives.Size() - 1);
    }
    if (!aSensEnt->BaseSensitive()->TransformPersistence().IsNull())
    {
      --myNbEntityWithPersistence;
    }

    mySensitives.RemoveLast();
    removeOwner(aSensEnt->BaseSensitive()->OwnerId());
  }

  MarkDirty();
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of entity with index theIdx
//=======================================================================
Select3D_BndBox3d SelectMgr_SensitiveEntitySet::Box(const Standard_Integer theIndex) const
{
  const Handle(Select3D_SensitiveEntity)& aSensitive = GetSensitiveById(theIndex)->BaseSensitive();
  if (!aSensitive->TransformPersistence().IsNull())
  {
    return Select3D_BndBox3d();
  }

  return aSensitive->BoundingBox();
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of sensitive entity index theIdx
//            along the given axis theAxis
//=======================================================================
Standard_Real SelectMgr_SensitiveEntitySet::Center(const Standard_Integer theIndex,
                                                   const Standard_Integer theAxis) const
{
  const Handle(Select3D_SensitiveEntity)& aSensitive = GetSensitiveById(theIndex)->BaseSensitive();
  const gp_Pnt                            aCenter    = aSensitive->CenterOfGeometry();
  Standard_Real                           aCenterCoord = 0.0;
  aCenterCoord = theAxis == 0 ? aCenter.X() : (theAxis == 1 ? aCenter.Y() : aCenter.Z());

  return aCenterCoord;
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2
//=======================================================================
void SelectMgr_SensitiveEntitySet::Swap(const Standard_Integer theIndex1,
                                        const Standard_Integer theIndex2)
{
  mySensitives.Swap(theIndex1 + 1, theIndex2 + 1);
}

//=======================================================================
// function : Size
// purpose  : Returns the amount of entities
//=======================================================================
Standard_Integer SelectMgr_SensitiveEntitySet::Size() const
{
  return mySensitives.Size();
}

//=======================================================================
// function : GetSensitiveById
// purpose  : Returns the entity with index theIndex in the set
//=======================================================================
const Handle(SelectMgr_SensitiveEntity)& SelectMgr_SensitiveEntitySet::GetSensitiveById(
  const Standard_Integer theIndex) const
{
  return mySensitives.FindKey(theIndex + 1);
}

//=================================================================================================

void SelectMgr_SensitiveEntitySet::addOwner(const Handle(SelectMgr_EntityOwner)& theOwner)
{
  if (!theOwner.IsNull())
  {
    if (Standard_Integer* aNumber = myOwnersMap.ChangeSeek(theOwner))
    {
      ++(*aNumber);
    }
    else
    {
      myOwnersMap.Bind(theOwner, 1);
    }
  }
}

//=================================================================================================

void SelectMgr_SensitiveEntitySet::removeOwner(const Handle(SelectMgr_EntityOwner)& theOwner)
{
  if (Standard_Integer* aNumber = !theOwner.IsNull() ? myOwnersMap.ChangeSeek(theOwner) : NULL)
  {
    if (--(*aNumber) == 0)
    {
      myOwnersMap.UnBind(theOwner);
    }
  }
}
