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

#include <BRepMeshData_PCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepMesh_Vertex.hxx>
#include <Standard_OutOfRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMeshData_PCurve, IMeshData_PCurve)

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMeshData_PCurve::BRepMeshData_PCurve (
  const IMeshData::IFacePtr&               theDFace,
  const TopAbs_Orientation                 theOrientation,
  const Handle (NCollection_IncAllocator)& theAllocator)
  : IMeshData_PCurve (theDFace, theOrientation),
    myPoints2d   (NCollection_OccAllocator<gp_Pnt2d>(theAllocator)),
    myParameters (NCollection_OccAllocator<Standard_Real>(theAllocator)),
    myIndices    (NCollection_OccAllocator<Standard_Integer>(theAllocator))
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMeshData_PCurve::~BRepMeshData_PCurve ()
{
}

//=======================================================================
// Function: InsertPoint
// Purpose : 
//=======================================================================
void BRepMeshData_PCurve::InsertPoint(
  const Standard_Integer thePosition,
  const gp_Pnt2d&        thePoint,
  const Standard_Real    theParamOnPCurve)
{
  myPoints2d  .insert(myPoints2d  .begin() + thePosition, thePoint);
  myParameters.insert(myParameters.begin() + thePosition, theParamOnPCurve);
  myIndices   .insert(myIndices   .begin() + thePosition, 0);
}

//=======================================================================
// Function: AddPoint
// Purpose : 
//=======================================================================
void BRepMeshData_PCurve::AddPoint (
  const gp_Pnt2d&     thePoint,
  const Standard_Real theParamOnPCurve)
{
  myPoints2d  .push_back(thePoint);
  myParameters.push_back(theParamOnPCurve);
  myIndices   .push_back(0);
}

//=======================================================================
// Function: GetPoint
// Purpose : 
//=======================================================================
gp_Pnt2d& BRepMeshData_PCurve::GetPoint (const Standard_Integer theIndex)
{
  Standard_OutOfRange_Raise_if (
    theIndex < 0 || theIndex >= static_cast<Standard_Integer>(myPoints2d.size()),
    "BRepMeshData_PCurve::GetPoint");
  return myPoints2d[theIndex];
}

//=======================================================================
// Function: GetIndex
// Purpose : 
//=======================================================================
Standard_Integer& BRepMeshData_PCurve::GetIndex(const Standard_Integer theIndex)
{
  Standard_OutOfRange_Raise_if (
    theIndex < 0 || theIndex >= static_cast<Standard_Integer>(myIndices.size()),
    "BRepMeshData_PCurve::GetIndex");
  return myIndices[theIndex];
}

//=======================================================================
// Function: GetParameter
// Purpose : 
//=======================================================================
Standard_Real& BRepMeshData_PCurve::GetParameter (const Standard_Integer theIndex)
{
  Standard_OutOfRange_Raise_if (
    theIndex < 0 || theIndex >= ParametersNb(),
    "BRepMeshData_PCurve::GetParameter");
  return myParameters[theIndex];
}

//=======================================================================
// Function: ParameterNb
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_PCurve::ParametersNb() const
{
  return static_cast<Standard_Integer>(myParameters.size());
}

//=======================================================================
// Function: RemovePoint
// Purpose : 
//=======================================================================
void BRepMeshData_PCurve::RemovePoint (const Standard_Integer theIndex)
{
  myPoints2d.erase(myPoints2d.begin() + theIndex);
  myIndices .erase(myIndices .begin() + theIndex);
  removeParameter (theIndex);
}

//=======================================================================
// Function: removeParameter
// Purpose : 
//=======================================================================
void BRepMeshData_PCurve::removeParameter (const Standard_Integer theIndex)
{
  myParameters.erase(myParameters.begin() + theIndex);
}

//=======================================================================
// Function: Clear
// Purpose : 
//=======================================================================
void BRepMeshData_PCurve::Clear(const Standard_Boolean isKeepEndPoints)
{
  if (!isKeepEndPoints)
  {
    myPoints2d  .clear();
    myParameters.clear();
    myIndices   .clear();
  }
  else if (ParametersNb() > 2)
  {
    myPoints2d  .erase(myPoints2d  .begin() + 1, myPoints2d  .begin() + (myPoints2d  .size() - 1));
    myParameters.erase(myParameters.begin() + 1, myParameters.begin() + (myParameters.size() - 1));
    myIndices   .erase(myIndices   .begin() + 1, myIndices   .begin() + (myIndices   .size() - 1));
  }
}
