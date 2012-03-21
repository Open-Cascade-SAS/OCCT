// Created on: 1999-05-06
// Created by: Pavel DURANDIN
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



#include <ShapeUpgrade_ShapeDivideAngle.ixx>
#include <ShapeUpgrade_SplitSurfaceAngle.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>

//=======================================================================
//function : ShapeUpgrade_ShapeDivideAngle
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideAngle::ShapeUpgrade_ShapeDivideAngle (const Standard_Real MaxAngle)
{
  InitTool ( MaxAngle );
}

//=======================================================================
//function : ShapeUpgrade_ShapeDivideAngle
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideAngle::ShapeUpgrade_ShapeDivideAngle(const Standard_Real MaxAngle,
							     const TopoDS_Shape& S):
       ShapeUpgrade_ShapeDivide(S)
{
  InitTool ( MaxAngle );
}

//=======================================================================
//function : InitTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideAngle::InitTool (const Standard_Real MaxAngle)
{
  Handle(ShapeUpgrade_FaceDivide) tool = GetSplitFaceTool();
  tool->SetSplitSurfaceTool ( new ShapeUpgrade_SplitSurfaceAngle (MaxAngle) );
  tool->SetWireDivideTool ( 0 ); // no splitting of wire
  SetSplitFaceTool(tool);
}
     
//=======================================================================
//function : SetMaxAngle
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideAngle::SetMaxAngle (const Standard_Real MaxAngle)
{
  InitTool ( MaxAngle );
}
     
//=======================================================================
//function : MaxAngle
//purpose  : 
//=======================================================================

double ShapeUpgrade_ShapeDivideAngle::MaxAngle () const
{
  Handle(ShapeUpgrade_FaceDivide) faceTool = GetSplitFaceTool();
  if ( faceTool.IsNull() ) return 0.;
  Handle(ShapeUpgrade_SplitSurfaceAngle) tool = 
    Handle(ShapeUpgrade_SplitSurfaceAngle)::DownCast ( faceTool );
  return ( tool.IsNull() ? 0. : tool->MaxAngle() );
}
     
