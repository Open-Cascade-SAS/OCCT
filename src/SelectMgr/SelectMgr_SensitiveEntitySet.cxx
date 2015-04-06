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

#include <BVH_BinnedBuilder.hxx>

#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_SensitiveEntity.hxx>

//=======================================================================
// function : SelectMgr_SensitiveEntitySet
// purpose  :
//=======================================================================
SelectMgr_SensitiveEntitySet::SelectMgr_SensitiveEntitySet()
{
  myBuilder = new BVH_BinnedBuilder<Standard_Real, 3, 32> (1, 32, Standard_True);
}

//=======================================================================
// function : Append
// purpose  : Adds new entity to the set and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SensitiveEntitySet::Append (const SelectMgr_HSensitiveEntity& theEntity)
{
  if (!theEntity->BaseSensitive()->IsKind ("Select3D_SensitiveEntity"))
  {
    theEntity->ResetSelectionActiveStatus();
    return;
  }
  myEntities.Append (theEntity);
  myEntityIdxs.Append (myEntities.Size());
  MarkDirty();
}

//=======================================================================
// function : Append
// purpose  : Adds every entity of selection theSelection to the set
//            and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SensitiveEntitySet::Append (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    if (!theSelection->Sensitive()->BaseSensitive()->IsKind ("Select3D_SensitiveEntity"))
    {
      theSelection->Sensitive()->ResetSelectionActiveStatus();
      continue;
    }
    myEntities.Append (theSelection->Sensitive());
    myEntityIdxs.Append (myEntities.Size());
  }
  MarkDirty();
}

//=======================================================================
// function : Remove
// purpose  : Removes entity from the set and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SensitiveEntitySet::Remove (const SelectMgr_HSensitiveEntity& theEntity)
{
  for (Standard_Integer anEntityIdx = 1; anEntityIdx <= myEntities.Size(); ++anEntityIdx)
  {
    if (myEntities.Value (anEntityIdx) == theEntity)
    {
      myEntities.Remove (anEntityIdx);
      myEntityIdxs.Clear();
      for (Standard_Integer anEntityIndexesIter = 1; anEntityIndexesIter <= myEntities.Size(); ++anEntityIndexesIter)
      {
        myEntityIdxs.Append (anEntityIndexesIter);
      }
      MarkDirty();
      break;
    }
  }
}

//=======================================================================
// function : Remove
// purpose  : Removes every entity of selection theSelection from the set
//            and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SensitiveEntitySet::Remove (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    for (Standard_Integer anEntityIdx = 1; anEntityIdx <= myEntities.Size(); ++anEntityIdx)
    {
      if (myEntities.Value (anEntityIdx) == theSelection->Sensitive())
      {
        myEntities.Remove (anEntityIdx);
        MarkDirty();
      }
    }
  }

  if (BVH_Object<Standard_Real, 3>::myIsDirty)
  {
    myEntityIdxs.Clear();
    for (Standard_Integer anEntityIdxsIter = 1; anEntityIdxsIter <= myEntities.Size(); ++anEntityIdxsIter)
    {
      myEntityIdxs.Append (anEntityIdxsIter);
    }
  }
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of entity with index theIdx
//=======================================================================
Select3D_BndBox3d SelectMgr_SensitiveEntitySet::Box (const Standard_Integer theIndex) const
{
  Standard_Integer anEntityIdx = myEntityIdxs.Value (theIndex + 1);
  return myEntities.Value (anEntityIdx)->BaseSensitive()->BoundingBox();
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of sensitive entity index theIdx
//            along the given axis theAxis
//=======================================================================
Standard_Real SelectMgr_SensitiveEntitySet::Center (const Standard_Integer theIndex,
                                                    const Standard_Integer theAxis) const
{
  Standard_Integer anEntityIdx = myEntityIdxs.Value (theIndex + 1);
  const Handle(SelectBasics_SensitiveEntity)& aBasicEntity =
    myEntities.Value (anEntityIdx)->BaseSensitive();
  const Handle(Select3D_SensitiveEntity)& aSensitive =
    Handle(Select3D_SensitiveEntity)::DownCast (aBasicEntity);
  const gp_Pnt aCenter = aSensitive->CenterOfGeometry();
  Standard_Real aCenterCoord = 0.0;
  aCenterCoord = theAxis == 0 ? aCenter.X() :
    (theAxis == 1 ? aCenter.Y() : aCenter.Z());

  return aCenterCoord;
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2
//=======================================================================
void SelectMgr_SensitiveEntitySet::Swap (const Standard_Integer theIndex1,
                                         const Standard_Integer theIndex2)
{
  Standard_Integer anEntityIdx1 = myEntityIdxs.Value (theIndex1 + 1);
  Standard_Integer anEntityIdx2 = myEntityIdxs.Value (theIndex2 + 1);
  myEntityIdxs.ChangeValue (theIndex1 + 1) = anEntityIdx2;
  myEntityIdxs.ChangeValue (theIndex2 + 1) = anEntityIdx1;
}

//=======================================================================
// function : Size
// purpose  : Returns the amount of entities
//=======================================================================
Standard_Integer SelectMgr_SensitiveEntitySet::Size() const
{
  return myEntityIdxs.Size();
}

//=======================================================================
// function : GetSensitiveById
// purpose  : Returns the entity with index theIndex in the set
//=======================================================================
const SelectMgr_HSensitiveEntity& SelectMgr_SensitiveEntitySet::GetSensitiveById
  (const Standard_Integer theIndex) const
{
  Standard_Integer anIdx = myEntityIdxs.Value (theIndex + 1);
  return myEntities.Value (anIdx);
}
