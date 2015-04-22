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

#include <Bnd_Box.hxx>
#include <BVH_BinnedBuilder.hxx>

#include <SelectMgr_SelectableObjectSet.hxx>

//=======================================================================
// function : SelectMgr_SelectableObjectSet
// purpose  : Creates new empty objects set and initializes BVH tree
//            builder to Binned builder with 1 element per list
//=======================================================================
SelectMgr_SelectableObjectSet::SelectMgr_SelectableObjectSet()
{
  myBuilder = new BVH_BinnedBuilder<Standard_Real, 3, 32> (1, 32, Standard_False);
}

//=======================================================================
// function : Append
// purpose  : Adds new object to the set and marks BVH tree for rebuild
//=======================================================================
void SelectMgr_SelectableObjectSet::Append (const Handle(SelectMgr_SelectableObject)& theObject)
{
  myObjects.Append (theObject);
  myObjectIdxs.Append (myObjects.Size());

  MarkDirty();
}

//=======================================================================
// function : Remove
// purpose  : Removes object theObject from set and marks BVH tree for
//            rebuild
//=======================================================================
void SelectMgr_SelectableObjectSet::Remove (const Handle(SelectMgr_SelectableObject)& theObject)
{
  for (Standard_Integer anObjectIdx = 1; anObjectIdx <= myObjects.Size(); ++anObjectIdx)
  {
    if (myObjects.Value (anObjectIdx) == theObject)
    {
      myObjects.Remove (anObjectIdx);
      myObjectIdxs.Clear();
      for (Standard_Integer anObjIdxsIter = 1; anObjIdxsIter <= myObjects.Size(); ++anObjIdxsIter)
      {
        myObjectIdxs.Append (anObjIdxsIter);
      }
      MarkDirty();
      break;
    }
  }
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of object with index theIndex
//=======================================================================
Select3D_BndBox3d SelectMgr_SelectableObjectSet::Box (const Standard_Integer theIndex) const
{
  const Handle(SelectMgr_SelectableObject)& anObject = GetObjectById (theIndex);
  Bnd_Box aBox;
  anObject->BoundingBox (aBox);
  if (aBox.IsVoid())
    return Select3D_BndBox3d();

  return Select3D_BndBox3d (SelectMgr_Vec3 (aBox.CornerMin().X(), aBox.CornerMin().Y(), aBox.CornerMin().Z()),
                            SelectMgr_Vec3 (aBox.CornerMax().X(), aBox.CornerMax().Y(), aBox.CornerMax().Z()));
}

//=======================================================================
// function : Center
// purpose  : Returns center of object with index theIndex in the set
//            along the given axis theAxis
//=======================================================================
Standard_Real SelectMgr_SelectableObjectSet::Center (const Standard_Integer theIndex,
                                                     const Standard_Integer theAxis) const
{
  Select3D_BndBox3d aBndBox = Box (theIndex);
  Standard_Real aCenter = theAxis == 0 ? (aBndBox.CornerMin().x() + aBndBox.CornerMax().x()) * 0.5 :
    (theAxis == 1 ? (aBndBox.CornerMin().y() + aBndBox.CornerMax().y()) * 0.5 :
                    (aBndBox.CornerMin().z() + aBndBox.CornerMax().z()) * 0.5);

  return aCenter;
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIndex1 and theIndex2 in the set
//=======================================================================
void SelectMgr_SelectableObjectSet::Swap (const Standard_Integer theIndex1,
                                          const Standard_Integer theIndex2)
{
  Standard_Integer anObjectIdx1 = myObjectIdxs.Value (theIndex1 + 1);
  Standard_Integer anObjectIdx2 = myObjectIdxs.Value (theIndex2 + 1);
  myObjectIdxs.ChangeValue (theIndex1 + 1) = anObjectIdx2;
  myObjectIdxs.ChangeValue (theIndex2 + 1) = anObjectIdx1;
}

//=======================================================================
// function : Size
// purpose  : Returns size of objects set
//=======================================================================
Standard_Integer SelectMgr_SelectableObjectSet::Size() const
{
  return myObjectIdxs.Size();
}

//=======================================================================
// function : GetObjectById
// purpose  : Returns object from set by theIndex given
//=======================================================================
const Handle(SelectMgr_SelectableObject)& SelectMgr_SelectableObjectSet::GetObjectById
                                                                         (const Standard_Integer theIndex) const
{
  Standard_Integer anIdx = myObjectIdxs.Value (theIndex + 1);
  return myObjects.Value (anIdx);
}

//=======================================================================
// function : Contains
// purpose  : Returns true if this objects set contains theObject given
//=======================================================================
Standard_Boolean SelectMgr_SelectableObjectSet::Contains (const Handle(SelectMgr_SelectableObject)& theObject) const
{
  for (Standard_Integer anObjectIdx = 1; anObjectIdx <= myObjects.Size(); ++anObjectIdx)
  {
    if (myObjects.Value (anObjectIdx) == theObject)
      return Standard_True;
  }

  return Standard_False;
}
