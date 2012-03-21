// Created on: 2000-05-24
// Created by: data exchange team
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



#include <ShapeUpgrade_EdgeDivide.ixx>
#include <BRep_Tool.hxx>
#include <ShapeAnalysis_Edge.hxx>

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
  Standard_Real f2d, l2d;
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
