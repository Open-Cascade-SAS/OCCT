// Created on: 1995-01-25
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

#include <Select3D_SensitiveSegment.hxx>
#include <gp_Vec.hxx>
#include <TopLoc_Location.hxx>
#include <Precision.hxx>


IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveSegment,Select3D_SensitiveEntity)

//=====================================================
// Function : Create
// Purpose  : Constructor
//=====================================================
Select3D_SensitiveSegment::Select3D_SensitiveSegment (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                      const gp_Pnt& theFirstPnt,
                                                      const gp_Pnt& theLastPnt)
: Select3D_SensitiveEntity (theOwnerId)
{
  myStart = theFirstPnt;
  myEnd = theLastPnt;
}

// =======================================================================
// function : Matches
// purpose  : Checks whether the segment overlaps current selecting volume
// =======================================================================
Standard_Boolean Select3D_SensitiveSegment::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                     SelectBasics_PickResult& thePickResult)
{
  thePickResult = SelectBasics_PickResult (RealLast(), RealLast());

  Standard_Real aDepth;
  if (!theMgr.IsOverlapAllowed()) // check for inclusion
  {
    return theMgr.Overlaps (myStart, aDepth) && theMgr.Overlaps (myEnd, aDepth);
  }

  if (!theMgr.Overlaps (myStart, myEnd, aDepth)) // check for overlap
  {
    return Standard_False;
  }

  thePickResult = SelectBasics_PickResult (aDepth,
    theMgr.DistToGeometryCenter (CenterOfGeometry()));

  return Standard_True;
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveSegment::GetConnected()
{
  Handle(Select3D_SensitiveSegment) aNewEntity =
    new Select3D_SensitiveSegment (myOwnerId, myStart, myEnd);

  return aNewEntity;
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of the segment. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitiveSegment::CenterOfGeometry() const
{
  return (myStart.XYZ() + myEnd.XYZ()) * 0.5;
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the segment. If location
//            transformation is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveSegment::BoundingBox()
{
  const SelectMgr_Vec3 aMinPnt (Min (myStart.X(), myEnd.X()),
                                Min (myStart.Y(), myEnd.Y()),
                                Min (myStart.Z(), myEnd.Z()));
  const SelectMgr_Vec3 aMaxPnt (Max (myStart.X(), myEnd.X()),
                                Max (myStart.Y(), myEnd.Y()),
                                Max (myStart.Z(), myEnd.Z()));
  return Select3D_BndBox3d (aMinPnt, aMaxPnt);
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of points
//=======================================================================
Standard_Integer Select3D_SensitiveSegment::NbSubElements()
{
  return 2;
}
