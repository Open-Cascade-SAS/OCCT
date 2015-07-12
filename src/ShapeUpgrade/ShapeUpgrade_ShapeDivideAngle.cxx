// Created on: 1999-05-06
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <ShapeUpgrade_FaceDivide.hxx>
#include <ShapeUpgrade_ShapeDivideAngle.hxx>
#include <ShapeUpgrade_SplitSurfaceAngle.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <TopoDS_Shape.hxx>

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
    Handle(ShapeUpgrade_SplitSurfaceAngle)::DownCast (faceTool->GetSplitSurfaceTool());
  return ( tool.IsNull() ? 0. : tool->MaxAngle() );
}
     
