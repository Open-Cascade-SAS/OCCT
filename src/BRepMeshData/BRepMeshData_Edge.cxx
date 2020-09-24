// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMeshData_Edge.hxx>
#include <BRepMeshData_PCurve.hxx>
#include <BRepMeshData_Curve.hxx>
#include <BRepMesh_OrientedEdge.hxx>
#include <BRepMesh_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMeshData_Edge, IMeshData_Edge)

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMeshData_Edge::BRepMeshData_Edge (
  const TopoDS_Edge&                       theEdge,
  const Handle (NCollection_IncAllocator)& theAllocator)
  : IMeshData_Edge (theEdge),
    myAllocator (theAllocator),
    myPCurves (256, myAllocator),
    myPCurvesMap(1, myAllocator)
{
  SetCurve (IMeshData::ICurveHandle (new (myAllocator) BRepMeshData_Curve (myAllocator)));
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMeshData_Edge::~BRepMeshData_Edge ()
{
}

//=======================================================================
// Function: AddPCurve
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_Edge::PCurvesNb () const
{
  return myPCurves.Size ();
}

//=======================================================================
// Function: AddPCurve
// Purpose : 
//=======================================================================
const IMeshData::IPCurveHandle& BRepMeshData_Edge::AddPCurve (
  const IMeshData::IFacePtr& theDFace,
  const TopAbs_Orientation   theOrientation)
{
  const Standard_Integer aPCurveIndex = PCurvesNb ();
  // Add pcurve to list of pcurves
  IMeshData::IPCurveHandle aPCurve (new (myAllocator) BRepMeshData_PCurve (theDFace, theOrientation, myAllocator));
  myPCurves.Append (aPCurve);

  // Map pcurve to faces.
  if (!myPCurvesMap.IsBound(theDFace))
  {
    myPCurvesMap.Bind(theDFace, IMeshData::ListOfInteger(myAllocator));
  }

  IMeshData::ListOfInteger& aListOfPCurves = myPCurvesMap.ChangeFind(theDFace);
  aListOfPCurves.Append (aPCurveIndex);

  return GetPCurve (aPCurveIndex);
}

//=======================================================================
// Function: GetPCurve
// Purpose : 
//=======================================================================
const IMeshData::IPCurveHandle& BRepMeshData_Edge::GetPCurve (
  const IMeshData::IFacePtr& theDFace,
  const TopAbs_Orientation   theOrientation) const
{
  const IMeshData::ListOfInteger& aListOfPCurves = myPCurvesMap.Find (theDFace);
  const IMeshData::IPCurveHandle& aPCurve1 = myPCurves (aListOfPCurves.First ());
  return (aPCurve1->GetOrientation () == theOrientation) ?
    aPCurve1 :
    myPCurves (aListOfPCurves.Last ());
}

//=======================================================================
// Function: GetPCurve
// Purpose : 
//=======================================================================
const IMeshData::IPCurveHandle& BRepMeshData_Edge::GetPCurve (
  const Standard_Integer theIndex) const
{
  return myPCurves (theIndex);
}
