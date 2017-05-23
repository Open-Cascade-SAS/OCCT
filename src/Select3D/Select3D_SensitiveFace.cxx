// Created on: 1995-03-27
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

#include <Select3D_SensitiveFace.hxx>

#include <Select3D_SensitivePoly.hxx>
#include <Select3D_InteriorSensitivePointSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveFace,Select3D_SensitiveEntity)

//==================================================
// Function: Hide this constructor to the next version...
// Purpose : simply avoid interfering with the version update
//==================================================
Select3D_SensitiveFace::Select3D_SensitiveFace (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                const TColgp_Array1OfPnt& thePoints,
                                                const Select3D_TypeOfSensitivity theType)
: Select3D_SensitiveEntity (theOwnerId),
  mySensType (theType)
{
  if (mySensType == Select3D_TOS_INTERIOR)
  {
    myFacePoints = new Select3D_InteriorSensitivePointSet (theOwnerId, thePoints);
  }
  else
  {
    myFacePoints = new Select3D_SensitivePoly (theOwnerId, thePoints, Standard_True);
  }
}

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveFace::Select3D_SensitiveFace (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                const Handle(TColgp_HArray1OfPnt)& thePoints,
                                                const Select3D_TypeOfSensitivity theType)
: Select3D_SensitiveEntity (theOwnerId),
  mySensType (theType)
{
  if (mySensType == Select3D_TOS_INTERIOR)
  {
    myFacePoints = new Select3D_InteriorSensitivePointSet (theOwnerId, thePoints->Array1());
  }
  else
  {
    myFacePoints = new Select3D_SensitivePoly (theOwnerId, thePoints->Array1(), Standard_True);
  }
}

//=======================================================================
// function : GetPoints
// purpose  : Initializes the given array theHArrayOfPnt by 3d
//            coordinates of vertices of the face
//=======================================================================
void Select3D_SensitiveFace::GetPoints (Handle(TColgp_HArray1OfPnt)& theHArrayOfPnt)
{
  if (myFacePoints->IsKind(STANDARD_TYPE(Select3D_SensitivePoly)))
  {
    Handle(Select3D_SensitivePoly)::DownCast (myFacePoints)->Points3D (theHArrayOfPnt);
  }
  else
  {
    Handle(Select3D_InteriorSensitivePointSet)::DownCast (myFacePoints)->GetPoints (theHArrayOfPnt);
  }

}

//=======================================================================
// function : BVH
// purpose  : Builds BVH tree for the face
//=======================================================================
void Select3D_SensitiveFace::BVH()
{
  myFacePoints->BVH();
}

//=======================================================================
// function : Matches
// purpose  : Checks whether the face overlaps current selecting volume
//=======================================================================
Standard_Boolean Select3D_SensitiveFace::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                  SelectBasics_PickResult& thePickResult)
{
  return myFacePoints->Matches (theMgr, thePickResult);
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveFace::GetConnected()
{
  // Create a copy of this
  Handle(TColgp_HArray1OfPnt) aPoints;
  GetPoints (aPoints);

  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveFace (myOwnerId, aPoints, mySensType);

  return aNewEntity;
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the face. If location transformation
//            is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveFace::BoundingBox()
{
  return myFacePoints->BoundingBox();
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of the face. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitiveFace::CenterOfGeometry() const
{
  return myFacePoints->CenterOfGeometry();
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of sub-entities (points or planar convex
//            polygons)
//=======================================================================
Standard_Integer Select3D_SensitiveFace::NbSubElements()
{
  return myFacePoints->NbSubElements();
}
