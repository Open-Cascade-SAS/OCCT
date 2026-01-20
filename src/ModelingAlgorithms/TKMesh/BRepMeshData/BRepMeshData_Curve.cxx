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

//=================================================================================================

BRepMeshData_Curve::BRepMeshData_Curve(const occ::handle<NCollection_IncAllocator>& theAllocator)
    : myPoints(NCollection_OccAllocator<gp_Pnt>(theAllocator)),
      myParameters(NCollection_OccAllocator<double>(theAllocator))
{
}

//=================================================================================================

BRepMeshData_Curve::~BRepMeshData_Curve() {}

//=================================================================================================

void BRepMeshData_Curve::InsertPoint(const int thePosition,
                                     const gp_Pnt&          thePoint,
                                     const double    theParamOnPCurve)
{
  myPoints.insert(myPoints.begin() + thePosition, thePoint);
  myParameters.insert(myParameters.begin() + thePosition, theParamOnPCurve);
}

//=================================================================================================

void BRepMeshData_Curve::AddPoint(const gp_Pnt& thePoint, const double theParamOnPCurve)
{
  myPoints.push_back(thePoint);
  myParameters.push_back(theParamOnPCurve);
}

//=================================================================================================

gp_Pnt& BRepMeshData_Curve::GetPoint(const int theIndex)
{
  return myPoints[theIndex];
}

//=================================================================================================

double& BRepMeshData_Curve::GetParameter(const int theIndex)
{
  return myParameters[theIndex];
}

//=================================================================================================

int BRepMeshData_Curve::ParametersNb() const
{
  return static_cast<int>(myParameters.size());
}

//=================================================================================================

void BRepMeshData_Curve::RemovePoint(const int theIndex)
{
  myPoints.erase(myPoints.begin() + theIndex);
  removeParameter(theIndex);
}

//=================================================================================================

void BRepMeshData_Curve::removeParameter(const int theIndex)
{
  myParameters.erase(myParameters.begin() + theIndex);
}

//=================================================================================================

void BRepMeshData_Curve::Clear(const bool isKeepEndPoints)
{
  if (!isKeepEndPoints)
  {
    myPoints.clear();
    myParameters.clear();
  }
  else if (ParametersNb() > 2)
  {
    myPoints.erase(myPoints.begin() + 1, myPoints.begin() + (myPoints.size() - 1));
    myParameters.erase(myParameters.begin() + 1, myParameters.begin() + (myParameters.size() - 1));
  }
}
