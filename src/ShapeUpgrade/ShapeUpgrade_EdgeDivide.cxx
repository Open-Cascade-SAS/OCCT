// Created on: 2000-05-24
// Created by: data exchange team
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


#include <BRep_Tool.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeUpgrade_EdgeDivide.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_EdgeDivide,ShapeUpgrade_Tool)

//=======================================================================
//function : ShapeUpgrade_EdgeDivide
//purpose  : 
//=======================================================================
ShapeUpgrade_EdgeDivide::ShapeUpgrade_EdgeDivide():
      ShapeUpgrade_Tool()
{
  mySplitCurve3dTool = new ShapeUpgrade_SplitCurve3d;
  mySplitCurve2dTool = new ShapeUpgrade_SplitCurve2d;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void ShapeUpgrade_EdgeDivide::Clear()
{
  myKnots3d.Nullify();
  myKnots2d.Nullify();
  myHasCurve3d = Standard_False;
  myHasCurve2d = Standard_False;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_EdgeDivide::Compute(const TopoDS_Edge& anEdge)
{
  Clear();
  
  Standard_Real f, l;
  Handle(Geom_Curve) curve3d = BRep_Tool::Curve (anEdge, f, l);
  myHasCurve3d = !curve3d.IsNull();
  
  Handle(ShapeUpgrade_SplitCurve3d) theSplit3dTool = GetSplitCurve3dTool();
  if ( myHasCurve3d ) {
    theSplit3dTool->Init (curve3d, f, l);
    theSplit3dTool->Compute();
    myKnots3d = theSplit3dTool->SplitValues();
  }
  
  // on pcurve(s): all knots
  // assume that if seam-edge, its pcurve1 and pcurve2 has the same split knots !!!
  Standard_Real f2d = 0., l2d = 0.;
  Handle(Geom2d_Curve) pcurve1;
  if ( ! myFace.IsNull() ) { // process free edges
    ShapeAnalysis_Edge sae;
    sae.PCurve (anEdge, myFace, pcurve1, f2d, l2d, Standard_False);
  }
  myHasCurve2d = !pcurve1.IsNull();
  
  Handle(ShapeUpgrade_SplitCurve2d) theSplit2dTool = GetSplitCurve2dTool();
  if ( myHasCurve2d ) {
    theSplit2dTool->Init (pcurve1, f2d, l2d);
    theSplit2dTool->Compute();
    myKnots2d = theSplit2dTool->SplitValues();
  }
  
  if ( theSplit3dTool->Status ( ShapeExtend_DONE ) || 
      theSplit2dTool->Status ( ShapeExtend_DONE ) ) 
    return Standard_True;
  else
    return Standard_False;
}

//=======================================================================
//function : SetSplitCurve2dTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_EdgeDivide::SetSplitCurve2dTool(const Handle(ShapeUpgrade_SplitCurve2d)& splitCurve2dTool)
{
  mySplitCurve2dTool = splitCurve2dTool;
}

//=======================================================================
//function : SetSplitCurve3dTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_EdgeDivide::SetSplitCurve3dTool(const Handle(ShapeUpgrade_SplitCurve3d)& splitCurve3dTool)
{
  mySplitCurve3dTool = splitCurve3dTool;
}

//=======================================================================
//function : GetSplitCurve2dTool
//purpose  : 
//=======================================================================

Handle(ShapeUpgrade_SplitCurve2d) ShapeUpgrade_EdgeDivide::GetSplitCurve2dTool() const
{ 
  return mySplitCurve2dTool;
}

Handle(ShapeUpgrade_SplitCurve3d) ShapeUpgrade_EdgeDivide::GetSplitCurve3dTool() const
{ 
  return mySplitCurve3dTool;
}
