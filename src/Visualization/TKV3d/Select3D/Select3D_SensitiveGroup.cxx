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

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveGroup, Select3D_SensitiveSet)

//=================================================================================================

Select3D_SensitiveGroup::Select3D_SensitiveGroup(
  const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
  const bool                                theIsMustMatchAll)
    : Select3D_SensitiveSet(theOwnerId),
      myMustMatchAll(theIsMustMatchAll),
      myToCheckOverlapAll(false),
      myCenter(0.0, 0.0, 0.0)
{
}

//=================================================================================================

Select3D_SensitiveGroup::Select3D_SensitiveGroup(
  const occ::handle<SelectMgr_EntityOwner>&                    theOwnerId,
  NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>>& theEntities,
  const bool                                                   theIsMustMatchAll)
    : Select3D_SensitiveSet(theOwnerId),
      myEntities(std::max(1, theEntities.Size())),
      myMustMatchAll(theIsMustMatchAll),
      myToCheckOverlapAll(false),
      myCenter(0.0, 0.0, 0.0)
{
  for (NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>>::Iterator anIter(theEntities);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Select3D_SensitiveEntity>& anEntity    = anIter.Value();
    const int                                    aPrevExtent = myEntities.Extent();
    if (myEntities.Add(anEntity) <= aPrevExtent)
    {
      continue;
    }

    myBndBox.Combine(anEntity->BoundingBox());
    myBVHPrimIndexes.Append(myEntities.Extent());
    myCenter.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
  }

  myCenter.ChangeCoord().Divide(static_cast<double>(myEntities.Extent()));

  MarkDirty();
}

//=======================================================================
// function : Add
// purpose  : No control of entities inside
//=======================================================================
void Select3D_SensitiveGroup::Add(
  NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>>& theEntities)
{
  if (theEntities.IsEmpty())
  {
    return;
  }

  gp_Pnt aCent(0.0, 0.0, 0.0);
  myEntities.ReSize(myEntities.Extent() + theEntities.Size());
  for (NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>>::Iterator anIter(theEntities);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Select3D_SensitiveEntity>& anEntity    = anIter.Value();
    const int                                    aPrevExtent = myEntities.Extent();
    if (myEntities.Add(anEntity) <= aPrevExtent)
    {
      continue;
    }

    myBndBox.Combine(anEntity->BoundingBox());
    myBVHPrimIndexes.Append(myEntities.Extent());
    aCent.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
  }
  aCent.ChangeCoord().Divide(myEntities.Extent());
  myCenter = (myCenter.XYZ() + aCent.XYZ()).Multiplied(0.5);
}

//=================================================================================================

void Select3D_SensitiveGroup::Add(const occ::handle<Select3D_SensitiveEntity>& theSensitive)
{
  const int aPrevExtent = myEntities.Extent();
  if (myEntities.Add(theSensitive) <= aPrevExtent)
  {
    return;
  }

  myBVHPrimIndexes.Append(myEntities.Extent());
  myBndBox.Combine(theSensitive->BoundingBox());
  myCenter.ChangeCoord() += theSensitive->CenterOfGeometry().XYZ();
  if (myEntities.Extent() >= 2)
  {
    myCenter.ChangeCoord().Multiply(0.5);
  }
}

//=================================================================================================

void Select3D_SensitiveGroup::Remove(const occ::handle<Select3D_SensitiveEntity>& theSensitive)
{
  if (!myEntities.RemoveKey(theSensitive))
  {
    return;
  }

  myBndBox.Clear();
  myCenter = gp_Pnt(0.0, 0.0, 0.0);
  myBVHPrimIndexes.Clear();
  for (int anIdx = 1; anIdx <= myEntities.Size(); ++anIdx)
  {
    const occ::handle<Select3D_SensitiveEntity>& anEntity = myEntities.FindKey(anIdx);
    myBndBox.Combine(anEntity->BoundingBox());
    myCenter.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
    myBVHPrimIndexes.Append(anIdx);
  }
  myCenter.ChangeCoord().Divide(static_cast<double>(myEntities.Extent()));
}

//=================================================================================================

bool Select3D_SensitiveGroup::IsIn(const occ::handle<Select3D_SensitiveEntity>& theSensitive) const
{
  return myEntities.Contains(theSensitive);
}

//=================================================================================================

void Select3D_SensitiveGroup::Clear()
{
  myEntities.Clear();
  myBndBox.Clear();
  myCenter = gp_Pnt(0.0, 0.0, 0.0);
  myBVHPrimIndexes.Clear();
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of sub-entities
//=======================================================================
int Select3D_SensitiveGroup::NbSubElements() const
{
  return myEntities.Size();
}

//=================================================================================================

occ::handle<Select3D_SensitiveEntity> Select3D_SensitiveGroup::GetConnected()
{
  occ::handle<Select3D_SensitiveGroup> aNewEntity =
    new Select3D_SensitiveGroup(myOwnerId, myMustMatchAll);
  NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>> aConnectedEnt;
  for (NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>>::Iterator anEntityIter(
         myEntities);
       anEntityIter.More();
       anEntityIter.Next())
  {
    aConnectedEnt.Append(anEntityIter.Value()->GetConnected());
  }
  aNewEntity->Add(aConnectedEnt);
  return aNewEntity;
}

//=================================================================================================

bool Select3D_SensitiveGroup::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                      SelectBasics_PickResult&             thePickResult)
{
  const bool toMatchAll =
    theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point && myMustMatchAll;
  const bool toCheckAll =
    theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point && myToCheckOverlapAll;
  if (!toMatchAll && !toCheckAll)
  {
    return Select3D_SensitiveSet::Matches(theMgr, thePickResult);
  }

  SelectBasics_PickResult aPickResult;
  bool                    isFailed = false;
  for (NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>>::Iterator anEntityIter(
         myEntities);
       anEntityIter.More();
       anEntityIter.Next())
  {
    const occ::handle<Select3D_SensitiveEntity>& aChild = anEntityIter.Value();
    if (!aChild->Matches(theMgr, aPickResult))
    {
      if (toMatchAll)
      {
        isFailed = true;
        if (!toCheckAll)
        {
          break;
        }
      }
    }
    else
    {
      thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
    }
  }
  if (isFailed)
  {
    return false;
  }

  thePickResult.SetDistToGeomCenter(distanceToCOG(theMgr));
  return true;
}

//=================================================================================================

void Select3D_SensitiveGroup::Set(const occ::handle<SelectMgr_EntityOwner>& theOwnerId)
{
  Select3D_SensitiveEntity::Set(theOwnerId);
  for (NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>>::Iterator anEntityIter(
         myEntities);
       anEntityIter.More();
       anEntityIter.Next())
  {
    anEntityIter.Value()->Set(theOwnerId);
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
  for (NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>>::Iterator anEntityIter(
         myEntities);
       anEntityIter.More();
       anEntityIter.Next())
  {
    myBndBox.Combine(anEntityIter.Value()->BoundingBox());
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
Select3D_BndBox3d Select3D_SensitiveGroup::Box(const int theIdx) const
{
  const int anElemIdx = myBVHPrimIndexes.Value(theIdx);
  return myEntities.FindKey(anElemIdx)->BoundingBox();
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of sensitive entity with index
//            theIdx in the vector along the given axis theAxis
//=======================================================================
double Select3D_SensitiveGroup::Center(const int theIdx, const int theAxis) const
{
  const int    anElemIdx = myBVHPrimIndexes.Value(theIdx);
  const gp_Pnt aCenter   = myEntities.FindKey(anElemIdx)->CenterOfGeometry();
  return theAxis == 0 ? aCenter.X() : (theAxis == 1 ? aCenter.Y() : aCenter.Z());
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in the vector
//=======================================================================
void Select3D_SensitiveGroup::Swap(const int theIdx1, const int theIdx2)
{
  const int anEntIdx1 = myBVHPrimIndexes.Value(theIdx1);
  const int anEntIdx2 = myBVHPrimIndexes.Value(theIdx2);

  myBVHPrimIndexes.ChangeValue(theIdx1) = anEntIdx2;
  myBVHPrimIndexes.ChangeValue(theIdx2) = anEntIdx1;
}

//=======================================================================
// function : Size
// purpose  : Returns the length of vector of sensitive entities
//=======================================================================
int Select3D_SensitiveGroup::Size() const
{
  return myBVHPrimIndexes.Size();
}

// =======================================================================
// function : overlapsElement
// purpose  : Checks whether the entity with index theIdx overlaps the
//            current selecting volume
// =======================================================================
bool Select3D_SensitiveGroup::overlapsElement(SelectBasics_PickResult&             thePickResult,
                                              SelectBasics_SelectingVolumeManager& theMgr,
                                              int                                  theElemIdx,
                                              bool)
{
  const int aSensitiveIdx = myBVHPrimIndexes.Value(theElemIdx);
  return myEntities.FindKey(aSensitiveIdx)->Matches(theMgr, thePickResult);
}

//=================================================================================================

bool Select3D_SensitiveGroup::elementIsInside(SelectBasics_SelectingVolumeManager& theMgr,
                                              int                                  theElemIdx,
                                              bool                                 theIsFullInside)
{
  SelectBasics_PickResult aDummy;
  return overlapsElement(aDummy, theMgr, theElemIdx, theIsFullInside);
}

// =======================================================================
// function : distanceToCOG
// purpose  : Calculates distance from the 3d projection of used-picked
//            screen point to center of the geometry
// =======================================================================
double Select3D_SensitiveGroup::distanceToCOG(SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter(CenterOfGeometry());
}

//=================================================================================================

void Select3D_SensitiveGroup::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveSet)

  for (NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>>::Iterator anIterator(
         myEntities);
       anIterator.More();
       anIterator.Next())
  {
    const occ::handle<Select3D_SensitiveEntity>& anEntity = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anEntity.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMustMatchAll)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToCheckOverlapAll)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBndBox)
}
