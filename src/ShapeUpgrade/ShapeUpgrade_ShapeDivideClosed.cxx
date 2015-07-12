// Created on: 1999-07-22
// Created by: data exchange team
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


#include <ShapeUpgrade_ClosedFaceDivide.hxx>
#include <ShapeUpgrade_ShapeDivideClosed.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <TopoDS_Shape.hxx>

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
