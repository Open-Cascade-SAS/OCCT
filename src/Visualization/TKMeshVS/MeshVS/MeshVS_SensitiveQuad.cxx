// Created on: 2016-03-02
// Created by: Varvara POSKONINA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <MeshVS_SensitiveQuad.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_SensitiveQuad, Select3D_SensitiveEntity)

//=================================================================================================

MeshVS_SensitiveQuad::MeshVS_SensitiveQuad(const Handle(SelectMgr_EntityOwner)& theOwner,
                                           const TColgp_Array1OfPnt&            theQuadVerts)
    : Select3D_SensitiveEntity(theOwner)
{
  const Standard_Integer aLowerIdx = theQuadVerts.Lower();
  for (Standard_Integer aVertIdx = 0; aVertIdx < 4; ++aVertIdx)
  {
    myVertices[aVertIdx] = theQuadVerts.Value(aLowerIdx + aVertIdx);
  }
}

//=================================================================================================

MeshVS_SensitiveQuad::MeshVS_SensitiveQuad(const Handle(SelectMgr_EntityOwner)& theOwner,
                                           const gp_Pnt&                        thePnt1,
                                           const gp_Pnt&                        thePnt2,
                                           const gp_Pnt&                        thePnt3,
                                           const gp_Pnt&                        thePnt4)
    : Select3D_SensitiveEntity(theOwner)
{
  myVertices[0] = thePnt1;
  myVertices[1] = thePnt2;
  myVertices[2] = thePnt3;
  myVertices[3] = thePnt4;
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) MeshVS_SensitiveQuad::GetConnected()
{
  return new MeshVS_SensitiveQuad(myOwnerId,
                                  myVertices[0],
                                  myVertices[1],
                                  myVertices[2],
                                  myVertices[3]);
}

//=======================================================================
// function : Matches
// purpose  : Checks whether the box overlaps current selecting volume
//=======================================================================
Standard_Boolean MeshVS_SensitiveQuad::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                               SelectBasics_PickResult&             thePickResult)
{
  if (!theMgr.IsOverlapAllowed()) // check for inclusion
  {
    if (theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Polyline)
    {
      SelectBasics_PickResult aDummy;
      return theMgr.OverlapsTriangle(myVertices[0],
                                     myVertices[1],
                                     myVertices[2],
                                     Select3D_TOS_INTERIOR,
                                     aDummy)
             && theMgr.OverlapsTriangle(myVertices[0],
                                        myVertices[2],
                                        myVertices[3],
                                        Select3D_TOS_INTERIOR,
                                        aDummy);
    }
    for (Standard_Integer aPntIdx = 0; aPntIdx < 4; ++aPntIdx)
    {
      if (!theMgr.OverlapsPoint(myVertices[aPntIdx]))
        return Standard_False;
    }

    return Standard_True;
  }

  // check for overlap
  SelectBasics_PickResult aPickResult1, aPickResult2;
  if (!theMgr.OverlapsTriangle(myVertices[0],
                               myVertices[1],
                               myVertices[2],
                               Select3D_TOS_INTERIOR,
                               aPickResult1)
      && !theMgr.OverlapsTriangle(myVertices[0],
                                  myVertices[2],
                                  myVertices[3],
                                  Select3D_TOS_INTERIOR,
                                  aPickResult2))
  {
    return Standard_False;
  }

  thePickResult = SelectBasics_PickResult::Min(aPickResult1, aPickResult2);
  thePickResult.SetDistToGeomCenter(theMgr.DistToGeometryCenter(CenterOfGeometry()));
  return Standard_True;
}

//=================================================================================================

gp_Pnt MeshVS_SensitiveQuad::CenterOfGeometry() const
{
  gp_XYZ aSum(0.0, 0.0, 0.0);
  for (Standard_Integer aPntIdx = 0; aPntIdx < 4; ++aPntIdx)
  {
    aSum += myVertices[aPntIdx].XYZ();
  }

  return aSum / 4.0;
}

//=================================================================================================

Select3D_BndBox3d MeshVS_SensitiveQuad::BoundingBox()
{
  Select3D_BndBox3d aBox;
  for (Standard_Integer aPntIdx = 0; aPntIdx < 4; ++aPntIdx)
  {
    aBox.Add(
      SelectMgr_Vec3(myVertices[aPntIdx].X(), myVertices[aPntIdx].Y(), myVertices[aPntIdx].Z()));
  }

  return aBox;
}
