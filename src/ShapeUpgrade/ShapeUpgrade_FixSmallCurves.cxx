// Created on: 2000-06-07
// Created by: Galina KULIKOVA
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <ShapeUpgrade_FixSmallCurves.ixx>
//#include <TColGeom_HArray1OfCurve.hxx>
//#include <TColStd_HSequenceOfReal.hxx>
#//include <TColGeom2d_HArray1OfCurve.hxx>
//#include <TColStd_HSequenceOfReal.hxx>
#include <ShapeExtend.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>

//=======================================================================
//function : ShapeUpgrade_FixSmallCurves
//purpose  : 
//=======================================================================

ShapeUpgrade_FixSmallCurves::ShapeUpgrade_FixSmallCurves()
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeUpgrade_FixSmallCurves::Init(const TopoDS_Edge& theEdge,const TopoDS_Face& theFace) 
{
  myEdge = theEdge;
  myFace = theFace;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_FixSmallCurves::Approx(Handle(Geom_Curve)& /*Curve3d*/,
                                                     Handle(Geom2d_Curve)& /*Curve2d*/,
                                                     Handle(Geom2d_Curve)& /*Curve2dR*/, 
                                                     Standard_Real& /*First*/,
                                                     Standard_Real& /*Last*/) 
{
  return Standard_False;
}

//=======================================================================
//function : SetSplitCurve3dTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_FixSmallCurves::SetSplitCurve3dTool(const Handle(ShapeUpgrade_SplitCurve3d)& splitCurve3dTool)
{
  mySplitCurve3dTool = splitCurve3dTool;
}

//=======================================================================
//function : SetSplitCurve2dTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_FixSmallCurves::SetSplitCurve2dTool(const Handle(ShapeUpgrade_SplitCurve2d)& splitCurve2dTool)
{
  mySplitCurve2dTool = splitCurve2dTool;
}

//=======================================================================
//function : GetSplitCurve3dTool
//purpose  : 
//=======================================================================

Handle(ShapeUpgrade_SplitCurve3d) ShapeUpgrade_FixSmallCurves::GetSplitCurve3dTool() const
{
  return mySplitCurve3dTool;
}

//=======================================================================
//function : GetSplitCurve2dTool
//purpose  : 
//=======================================================================

Handle(ShapeUpgrade_SplitCurve2d) ShapeUpgrade_FixSmallCurves::GetSplitCurve2dTool() const
{
  return mySplitCurve2dTool;
}
//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeUpgrade_FixSmallCurves::Status(const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus ( myStatus, status );
}
