// Created on: 1998-04-16
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#include <Select3D_SensitiveGroup.hxx>

#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveGroup,Select3D_SensitiveSet)

//=======================================================================
//function : Creation
//purpose  :
//=======================================================================
Select3D_SensitiveGroup::Select3D_SensitiveGroup (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                  const Standard_Boolean theIsMustMatchAll)
: Select3D_SensitiveSet (theOwnerId),
  myMustMatchAll (theIsMustMatchAll),
  myToCheckOverlapAll (Standard_False),
  myCenter (0.0, 0.0, 0.0) {}

//=======================================================================
//function : Creation
//purpose  :
//=======================================================================
Select3D_SensitiveGroup::Select3D_SensitiveGroup (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                  Select3D_EntitySequence& theEntities,
                                                  const Standard_Boolean theIsMustMatchAll)
: Select3D_SensitiveSet (theOwnerId),
  myEntities (Max (1, theEntities.Size())),
  myMustMatchAll (theIsMustMatchAll),
  myToCheckOverlapAll (Standard_False),
  myCenter (0.0, 0.0, 0.0)
{
  for (Select3D_EntitySequenceIter anIter (theEntities); anIter.More(); anIter.Next())
  {
    const Handle(Select3D_SensitiveEntity)& anEntity = anIter.Value();
    const Standard_Integer aPrevExtent = myEntities.Extent();
    if (myEntities.Add (anEntity) <= aPrevExtent)
    {
      continue;
    }

    myBndBox.Combine (anEntity->BoundingBox());
    myBVHPrimIndexes.Append (myEntities.Extent());
    myCenter.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
  }

  myCenter.ChangeCoord().Divide (static_cast<Standard_Real> (myEntities.Extent()));

  MarkDirty();
}

//=======================================================================
//function : Add
//purpose  : No control of entities inside
//=======================================================================
void Select3D_SensitiveGroup::Add (Select3D_EntitySequence& theEntities)
{
  if (theEntities.IsEmpty())
  {
    return;
  }

  gp_Pnt aCent (0.0, 0.0, 0.0);
  myEntities.ReSize (myEntities.Extent() + theEntities.Size());
  for (Select3D_EntitySequenceIter anIter (theEntities); anIter.More(); anIter.Next())
  {
    const Handle(Select3D_SensitiveEntity)& anEntity = anIter.Value();
    const Standard_Integer aPrevExtent = myEntities.Extent();
    if (myEntities.Add (anEntity) <= aPrevExtent)
    {
      continue;
    }

    myBndBox.Combine (anEntity->BoundingBox());
    myBVHPrimIndexes.Append (myEntities.Extent());
    aCent.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
  }
  aCent.ChangeCoord().Divide (myEntities.Extent());
  myCenter = (myCenter.XYZ() + aCent.XYZ()).Multiplied (0.5);
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
void Select3D_SensitiveGroup::Add (const Handle(Select3D_SensitiveEntity)& theSensitive)
{
  const Standard_Integer aPrevExtent = myEntities.Extent();
  if (myEntities.Add (theSensitive) <= aPrevExtent)
  {
    return;
  }

  myBVHPrimIndexes.Append (myEntities.Extent());
  myBndBox.Combine (theSensitive->BoundingBox());
  myCenter.ChangeCoord() += theSensitive->CenterOfGeometry().XYZ();
  if (myEntities.Extent() >= 2)
  {
    myCenter.ChangeCoord().Multiply (0.5);
  }
}

//=======================================================================
//function : Remove
//purpose  :
//=======================================================================
void Select3D_SensitiveGroup::Remove (const Handle(Select3D_SensitiveEntity)& theSensitive)
{
  if (!myEntities.RemoveKey (theSensitive))
  {
    return;
  }

  myBndBox.Clear();
  myCenter = gp_Pnt (0.0, 0.0, 0.0);
  myBVHPrimIndexes.Clear();
  for (Standard_Integer anIdx = 1; anIdx <= myEntities.Size(); ++anIdx)
  {
    const Handle(Select3D_SensitiveEntity)& anEntity = myEntities.FindKey (anIdx);
    myBndBox.Combine (anEntity->BoundingBox());
    myCenter.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
    myBVHPrimIndexes.Append (anIdx);
  }
  myCenter.ChangeCoord().Divide (static_cast<Standard_Real> (myEntities.Extent()));
}

//=======================================================================
//function : IsIn
//purpose  :
//=======================================================================
Standard_Boolean Select3D_SensitiveGroup::IsIn (const Handle(Select3D_SensitiveEntity)& theSensitive) const
{
  return myEntities.Contains (theSensitive);
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Select3D_SensitiveGroup::Clear()
{
  myEntities.Clear();
  myBndBox.Clear();
  myCenter = gp_Pnt (0.0, 0.0, 0.0);
  myEntities.Clear();
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of sub-entities
//=======================================================================
Standard_Integer Select3D_SensitiveGroup::NbSubElements()
{
  return myEntities.Size();
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveGroup::GetConnected()
{
  Handle(Select3D_SensitiveGroup) aNewEntity = new Select3D_SensitiveGroup (myOwnerId, myMustMatchAll);
  Select3D_EntitySequence aConnectedEnt;
  for (Select3D_IndexedMapOfEntity::Iterator anEntityIter (myEntities); anEntityIter.More(); anEntityIter.Next())
  {
    aConnectedEnt.Append (anEntityIter.Value()->GetConnected());
  }
  aNewEntity->Add (aConnectedEnt);
  return aNewEntity;
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================
Standard_Boolean Select3D_SensitiveGroup::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
{
  const Standard_Boolean toMatchAll = theMgr.GetActiveSelectionType() != SelectBasics_SelectingVolumeManager::Point
                                   && myMustMatchAll;
  const Standard_Boolean toCheckAll = theMgr.GetActiveSelectionType() != SelectBasics_SelectingVolumeManager::Point
                                   && myToCheckOverlapAll;
  if (!toMatchAll && !toCheckAll)
  {
    return Select3D_SensitiveSet::Matches (theMgr, thePickResult);
  }

  Standard_Real aDepth     = RealLast();
  Standard_Real aDistToCOG = RealLast();
  Standard_Boolean isFailed = Standard_False;
  for (Select3D_IndexedMapOfEntity::Iterator anEntityIter (myEntities); anEntityIter.More(); anEntityIter.Next())
  {
    SelectBasics_PickResult aMatchResult;
    const Handle(Select3D_SensitiveEntity)& aChild = anEntityIter.Value();
    if (!aChild->Matches (theMgr, aMatchResult))
    {
      if (toMatchAll)
      {
        isFailed = Standard_True;
        if (!toCheckAll)
        {
          break;
        }
      }
    }
    else
    {
      aDepth = Min (aMatchResult.Depth(), aDepth);
    }
  }
  if (isFailed)
  {
    thePickResult = SelectBasics_PickResult (RealLast(), RealLast());
    return Standard_False;
  }

  aDistToCOG = theMgr.DistToGeometryCenter (CenterOfGeometry());
  thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
  return Standard_True;
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
void Select3D_SensitiveGroup::Set (const Handle(SelectBasics_EntityOwner)& theOwnerId)
{ 
  Select3D_SensitiveEntity::Set (theOwnerId);
  for (Select3D_IndexedMapOfEntity::Iterator anEntityIter (myEntities); anEntityIter.More(); anEntityIter.Next())
  {
    anEntityIter.Value()->Set (theOwnerId);
  }
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the group. If location
//            transformation is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveGroup::BoundingBox()
{
  if (myBndBox.IsValid())
    return myBndBox;

  // do not apply the transformation because sensitives AABBs
  // are already transformed
  for (Select3D_IndexedMapOfEntity::Iterator anEntityIter (myEntities); anEntityIter.More(); anEntityIter.Next())
  {
    myBndBox.Combine (anEntityIter.Value()->BoundingBox());
  }

  return myBndBox;
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of group. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitiveGroup::CenterOfGeometry() const
{
  return myCenter;
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of sensitive entity with index theIdx
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveGroup::Box (const Standard_Integer theIdx) const
{
  const Standard_Integer anElemIdx = myBVHPrimIndexes.Value (theIdx);
  return myEntities.FindKey (anElemIdx)->BoundingBox();
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of sensitive entity with index
//            theIdx in the vector along the given axis theAxis
//=======================================================================
Standard_Real Select3D_SensitiveGroup::Center (const Standard_Integer theIdx,
                                               const Standard_Integer theAxis) const
{
  const Standard_Integer anElemIdx = myBVHPrimIndexes.Value (theIdx);
  const gp_Pnt aCenter = myEntities.FindKey (anElemIdx)->CenterOfGeometry();
  return theAxis == 0 ? aCenter.X() : (theAxis == 1 ? aCenter.Y() : aCenter.Z());
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in the vector
//=======================================================================
void Select3D_SensitiveGroup::Swap (const Standard_Integer theIdx1,
                                    const Standard_Integer theIdx2)
{
  const Standard_Integer anEntIdx1 = myBVHPrimIndexes.Value (theIdx1);
  const Standard_Integer anEntIdx2 = myBVHPrimIndexes.Value (theIdx2);

  myBVHPrimIndexes.ChangeValue (theIdx1) = anEntIdx2;
  myBVHPrimIndexes.ChangeValue (theIdx2) = anEntIdx1;
}

//=======================================================================
// function : Size
// purpose  : Returns the length of vector of sensitive entities
//=======================================================================
Standard_Integer Select3D_SensitiveGroup::Size() const
{
  return myBVHPrimIndexes.Size();
}

// =======================================================================
// function : overlapsElement
// purpose  : Checks whether the entity with index theIdx overlaps the
//            current selecting volume
// =======================================================================
Standard_Boolean Select3D_SensitiveGroup::overlapsElement (SelectBasics_SelectingVolumeManager& theMgr,
                                                           Standard_Integer theElemIdx,
                                                           Standard_Real& theMatchDepth)
{
  theMatchDepth = RealLast();
  const Standard_Integer aSensitiveIdx = myBVHPrimIndexes.Value (theElemIdx);
  SelectBasics_PickResult aResult;
  if (myEntities.FindKey (aSensitiveIdx)->Matches (theMgr, aResult))
  {
    theMatchDepth = aResult.Depth();
    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : elementIsInside
// purpose  :
// =======================================================================
Standard_Boolean Select3D_SensitiveGroup::elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                           const Standard_Integer               theElemIdx)
{
  Standard_Real aDummy;
  return overlapsElement(theMgr, theElemIdx, aDummy);
}

// =======================================================================
// function : distanceToCOG
// purpose  : Calculates distance from the 3d projection of used-picked
//            screen point to center of the geometry
// =======================================================================
Standard_Real Select3D_SensitiveGroup::distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter (CenterOfGeometry());
}
