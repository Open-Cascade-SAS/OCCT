// Created on: 2000-05-25
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



#include <ShapeUpgrade_ShapeDivideClosedEdges.ixx>
#include <ShapeUpgrade_ClosedEdgeDivide.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>

//=======================================================================
//function : ShapeUpgrade_ShapeDivideClosedEdges
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideClosedEdges::ShapeUpgrade_ShapeDivideClosedEdges(const TopoDS_Shape& S):
       ShapeUpgrade_ShapeDivide(S)  
{
  SetNbSplitPoints(1);
}

//=======================================================================
//function : SetNbSplitPoints
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideClosedEdges::SetNbSplitPoints(const Standard_Integer /*num*/)
{
  Handle(ShapeUpgrade_ClosedEdgeDivide) tool = new ShapeUpgrade_ClosedEdgeDivide;
  Handle(ShapeUpgrade_WireDivide) wtool = new ShapeUpgrade_WireDivide;
  wtool->SetEdgeDivideTool(tool);
  Handle(ShapeUpgrade_FaceDivide) ftool = new ShapeUpgrade_FaceDivide;
  ftool->SetWireDivideTool(wtool);
  ftool->SetSplitSurfaceTool( 0 );
  SetSplitFaceTool(ftool);
}
