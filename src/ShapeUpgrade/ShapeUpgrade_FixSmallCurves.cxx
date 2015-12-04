// Created on: 2000-06-07
// Created by: Galina KULIKOVA
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <ShapeUpgrade_FixSmallCurves.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_FixSmallCurves,ShapeUpgrade_Tool)

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
