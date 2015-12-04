// Created on: 1996-10-17
// Created by: Odile OLIVIER
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Select3D_SensitiveWire.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>

#include <Select3D_SensitiveSegment.hxx>


IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveWire,Select3D_SensitiveSet)

//=====================================================
// Function : Select3D_SensitiveWire
// Purpose  :
//=====================================================
Select3D_SensitiveWire::Select3D_SensitiveWire (const Handle(SelectBasics_EntityOwner)& theOwnerId)
: Select3D_SensitiveSet (theOwnerId),
  myCenter (0.0, 0.0, 0.0)
{}

//=====================================================
// Function : Add
// Purpose  :
//=====================================================
void Select3D_SensitiveWire::Add (const Handle(Select3D_SensitiveEntity)& theSensitive)
{
  if (!theSensitive.IsNull())
    myEntities.Append (theSensitive);

  Select3D_BndBox3d aBndBox = theSensitive->BoundingBox();
  myBndBox.Combine (aBndBox);
  myCenter.ChangeCoord() += theSensitive->CenterOfGeometry().XYZ();
  if (myEntities.Length() != 1)
    myCenter.ChangeCoord().Divide (2.0);
  myEntityIndexes.Append (myEntities.Length() - 1);
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of sub-entities
//=======================================================================
Standard_Integer Select3D_SensitiveWire::NbSubElements()
{
  return myEntities.Length();
}

//=======================================================================
// function : Size
// purpose  : Returns the length of vector of sensitive entities
//=======================================================================
Standard_Integer Select3D_SensitiveWire::Size() const
{
  return myEntities.Length();
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of sensitive entity with index theIdx
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveWire::Box (const Standard_Integer theIdx) const
{
  const Standard_Integer aSensitiveIdx = myEntityIndexes.Value (theIdx);
  return myEntities.Value (aSensitiveIdx)->BoundingBox();
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of sensitive entity with index
//            theIdx in the vector along the given axis theAxis
//=======================================================================
Standard_Real Select3D_SensitiveWire::Center (const Standard_Integer theIdx,
                                              const Standard_Integer theAxis) const
{
  const Standard_Integer aSensitiveIdx = myEntityIndexes.Value (theIdx);
  const gp_Pnt& aCenter = myEntities.Value (aSensitiveIdx)->CenterOfGeometry();
  Standard_Real aCenterCoord = 0.0;
  aCenterCoord = theAxis == 0 ? aCenter.X() : (theAxis == 1 ? aCenter.Y() : aCenter.Z());

  return aCenterCoord;
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in the vector
//=======================================================================
void Select3D_SensitiveWire::Swap (const Standard_Integer theIdx1,
                                   const Standard_Integer theIdx2)
{
  const Standard_Integer aSensitiveIdx1 = myEntityIndexes.Value (theIdx1);
  const Standard_Integer aSensitiveIdx2 = myEntityIndexes.Value (theIdx2);
  myEntityIndexes.ChangeValue (theIdx1) = aSensitiveIdx2;
  myEntityIndexes.ChangeValue (theIdx2) = aSensitiveIdx1;
}

// =======================================================================
// function : overlapsElement
// purpose  : Checks whether the entity with index theIdx overlaps the
//            current selecting volume
// =======================================================================
Standard_Boolean Select3D_SensitiveWire::overlapsElement (SelectBasics_SelectingVolumeManager& theMgr,
                                                          Standard_Integer theElemIdx,
                                                          Standard_Real& theMatchDepth)
{
  const Standard_Integer aSensitiveIdx = myEntityIndexes.Value (theElemIdx);
  const Handle(SelectBasics_SensitiveEntity)& aSeg = myEntities.Value (aSensitiveIdx);
  SelectBasics_PickResult aMatchResult;
  if (aSeg->Matches (theMgr, aMatchResult))
  {
    theMatchDepth = aMatchResult.Depth();
    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : elementIsInside
// purpose  :
// =======================================================================
Standard_Boolean Select3D_SensitiveWire::elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                          const Standard_Integer               theElemIdx)
{
  SelectBasics_PickResult aMatchResult;
  return myEntities.Value (myEntityIndexes.Value (theElemIdx))->Matches (theMgr, aMatchResult);
}

// =======================================================================
// function : distanceToCOG
// purpose  : Calculates distance from the 3d projection of used-picked
//            screen point to center of the geometry
// =======================================================================
Standard_Real Select3D_SensitiveWire::distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter (myCenter);
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveWire::GetConnected()
{
  Handle(Select3D_SensitiveWire) aNewEntity = new Select3D_SensitiveWire (myOwnerId);
  for (Standard_Integer anEntityIdx = 0; anEntityIdx < myEntities.Length(); anEntityIdx++)
    aNewEntity->Add (myEntities(anEntityIdx)->GetConnected());

  return aNewEntity;
}

//=======================================================================
//function : GetEdges
//purpose  : returns the sensitive edges stored in this wire
//=======================================================================
const NCollection_Vector<Handle(Select3D_SensitiveEntity)>& Select3D_SensitiveWire::GetEdges()
{
  return myEntities;
}

//=============================================================================
// Function : GetLastDetected
// Purpose  :
//=============================================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveWire::GetLastDetected() const
{
  Handle(Select3D_SensitiveEntity) aRes;

  if (myDetectedIdx >= 0 && myDetectedIdx < myEntities.Length())
  {
    const Standard_Integer aSensitiveIdx = myEntityIndexes.Value (myDetectedIdx);
    aRes = myEntities.Value (aSensitiveIdx);
  }

  return aRes;
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
void Select3D_SensitiveWire::Set (const Handle(SelectBasics_EntityOwner)& theOwnerId)
{
  Select3D_SensitiveEntity::Set (theOwnerId);

  // Set TheOwnerId for each element of sensitive wire
  for (Standard_Integer anEntityIdx = 0; anEntityIdx < myEntities.Length(); ++anEntityIdx)
  {
    myEntities.Value (anEntityIdx)->Set (theOwnerId);
  }
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the wire. If location
//            transformation is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveWire::BoundingBox()
{
  if (myBndBox.IsValid())
    return myBndBox;

  for (Standard_Integer aSensitiveIdx = 0; aSensitiveIdx < myEntities.Length(); ++aSensitiveIdx)
  {
    myBndBox.Combine (myEntities.Value (aSensitiveIdx)->BoundingBox());
  }

  return myBndBox;
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of the wire. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitiveWire::CenterOfGeometry() const
{
  return myCenter;
}
