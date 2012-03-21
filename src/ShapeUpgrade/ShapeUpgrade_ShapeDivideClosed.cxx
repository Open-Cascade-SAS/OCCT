// Created on: 1999-07-22
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <ShapeUpgrade_ShapeDivideClosed.ixx>
#include <ShapeUpgrade_ClosedFaceDivide.hxx>

//=======================================================================
//function : ShapeUpgrade_ShapeDivideClosed
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideClosed::ShapeUpgrade_ShapeDivideClosed(const TopoDS_Shape& S):
     ShapeUpgrade_ShapeDivide(S)  
{
  SetNbSplitPoints(1);
}

//=======================================================================
//function : SetNbSplitPoints
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideClosed::SetNbSplitPoints(const Standard_Integer num)
{
  Handle(ShapeUpgrade_ClosedFaceDivide) tool = new ShapeUpgrade_ClosedFaceDivide;
  tool->SetNbSplitPoints(num);
  tool->SetWireDivideTool ( 0 ); // no splitting of wire
  SetSplitFaceTool(tool);
}
