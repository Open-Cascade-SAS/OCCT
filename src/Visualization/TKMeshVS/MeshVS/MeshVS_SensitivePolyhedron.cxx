// Created on: 2005-01-21
// Created by: Alexander SOLOVYOV
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

#include <MeshVS_SensitivePolyhedron.hxx>

#include <gp_Lin.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <MeshVS_Tool.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_SequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_SensitivePolyhedron, Select3D_SensitiveEntity)

//=================================================================================================

MeshVS_SensitivePolyhedron::MeshVS_SensitivePolyhedron(
  const Handle(SelectMgr_EntityOwner)&             theOwner,
  const TColgp_Array1OfPnt&                        theNodes,
  const Handle(MeshVS_HArray1OfSequenceOfInteger)& theTopo)
    : Select3D_SensitiveEntity(theOwner),
      myTopo(theTopo)
{
  Standard_Integer aPlaneLowIdx   = theTopo->Lower();
  Standard_Integer aPlaneUpIdx    = theTopo->Upper();
  Standard_Integer aNodesLowerIdx = theNodes.Lower();
  myNodes                         = new TColgp_HArray1OfPnt(aNodesLowerIdx, theNodes.Upper());
  myCenter                        = gp_XYZ(0.0, 0.0, 0.0);

  for (Standard_Integer aPlaneIdx = aPlaneLowIdx; aPlaneIdx <= aPlaneUpIdx; ++aPlaneIdx)
  {
    Standard_Integer            aVertNb    = theTopo->Value(aPlaneIdx).Length();
    Handle(TColgp_HArray1OfPnt) aVertArray = new TColgp_HArray1OfPnt(0, aVertNb - 1);
    for (Standard_Integer aVertIdx = 1; aVertIdx <= aVertNb; ++aVertIdx)
    {
      Standard_Integer aNodeIdx = theTopo->Value(aPlaneIdx).Value(aVertIdx);
      const gp_Pnt&    aVert    = theNodes.Value(aNodeIdx + aNodesLowerIdx);
      aVertArray->SetValue(aVertIdx - 1, aVert);
      myNodes->SetValue(aNodeIdx + aNodesLowerIdx, aVert);
      myBndBox.Add(SelectMgr_Vec3(aVert.X(), aVert.Y(), aVert.Z()));
      myCenter += aVert.XYZ();
    }

    myTopology.Append(aVertArray);
  }

  myCenter.Divide(theNodes.Length());
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) MeshVS_SensitivePolyhedron::GetConnected()
{
  Handle(MeshVS_SensitivePolyhedron) aNewEnt =
    new MeshVS_SensitivePolyhedron(myOwnerId, myNodes->Array1(), myTopo);

  return aNewEnt;
}

//=================================================================================================

Standard_Boolean MeshVS_SensitivePolyhedron::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                     SelectBasics_PickResult& thePickResult)
{
  SelectBasics_PickResult aPickResult;
  for (MeshVS_PolyhedronVertsIter aIter(myTopology); aIter.More(); aIter.Next())
  {
    if (theMgr.OverlapsPolygon(aIter.Value()->Array1(), Select3D_TOS_INTERIOR, aPickResult))
    {
      thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
    }
  }
  if (!thePickResult.IsValid())
  {
    return Standard_False;
  }

  thePickResult.SetDistToGeomCenter(theMgr.DistToGeometryCenter(CenterOfGeometry()));
  return Standard_True;
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of nodes of polyhedron
//=======================================================================
Standard_Integer MeshVS_SensitivePolyhedron::NbSubElements() const
{
  return myNodes->Length();
}

//=================================================================================================

Select3D_BndBox3d MeshVS_SensitivePolyhedron::BoundingBox()
{
  return myBndBox;
}

//=================================================================================================

gp_Pnt MeshVS_SensitivePolyhedron::CenterOfGeometry() const
{
  return myCenter;
}
