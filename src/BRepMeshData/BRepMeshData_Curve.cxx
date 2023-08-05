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

#include <BRepMeshData_Curve.hxx>
#include <gp_Pnt.hxx>
#include <BRepMesh_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMeshData_Curve, IMeshData_Curve)

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMeshData_Curve::BRepMeshData_Curve (const Handle (NCollection_IncAllocator)& theAllocator)
: myPoints     (NCollection_OccAllocator<gp_Pnt>(theAllocator)),
  myParameters (NCollection_OccAllocator<Standard_Real>(theAllocator))
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMeshData_Curve::~BRepMeshData_Curve ()
{
}

//=======================================================================
// Function: InsertPoint
// Purpose : 
//=======================================================================
void BRepMeshData_Curve::InsertPoint(
  const Standard_Integer thePosition,
  const gp_Pnt&          thePoint,
  const Standard_Real    theParamOnPCurve)
{
  myPoints    .insert(myPoints    .begin() + thePosition, thePoint);
  myParameters.insert(myParameters.begin() + thePosition, theParamOnPCurve);
}

//=======================================================================
// Function: AddPoint
// Purpose : 
//=======================================================================
void BRepMeshData_Curve::AddPoint (
  const gp_Pnt&       thePoint,
  const Standard_Real theParamOnPCurve)
{
  myPoints    .push_back(thePoint);
  myParameters.push_back(theParamOnPCurve);
}

//=======================================================================
// Function: GetPoint
// Purpose : 
//=======================================================================
gp_Pnt& BRepMeshData_Curve::GetPoint (const Standard_Integer theIndex)
{
  return myPoints[theIndex];
}

//=======================================================================
// Function: GetParameter
// Purpose : 
//=======================================================================
Standard_Real& BRepMeshData_Curve::GetParameter (const Standard_Integer theIndex)
{
  return myParameters[theIndex];
}

//=======================================================================
// Function: ParameterNb
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_Curve::ParametersNb() const
{
  return static_cast<Standard_Integer>(myParameters.size());
}

//=======================================================================
// Function: RemovePoint
// Purpose : 
//=======================================================================
void BRepMeshData_Curve::RemovePoint (const Standard_Integer theIndex)
{
  myPoints.erase(myPoints.begin() + theIndex);
  removeParameter (theIndex);
}

//=======================================================================
// Function: removeParameter
// Purpose : 
//=======================================================================
void BRepMeshData_Curve::removeParameter (const Standard_Integer theIndex)
{
  myParameters.erase(myParameters.begin() + theIndex);
}

//=======================================================================
// Function: Clear
// Purpose : 
//=======================================================================
void BRepMeshData_Curve::Clear(const Standard_Boolean isKeepEndPoints)
{
  if (!isKeepEndPoints)
  {
    myPoints    .clear();
    myParameters.clear();
  }
  else if (ParametersNb() > 2)
  {
    myPoints    .erase(myPoints    .begin() + 1, myPoints    .begin() + (myPoints    .size() - 1));
    myParameters.erase(myParameters.begin() + 1, myParameters.begin() + (myParameters.size() - 1));
  }
}
