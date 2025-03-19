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

#include <Precision.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>
#include <ShapeUpgrade_FaceDivideArea.hxx>
#include <ShapeUpgrade_ShapeDivideArea.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

ShapeUpgrade_ShapeDivideArea::ShapeUpgrade_ShapeDivideArea()
    : ShapeUpgrade_ShapeDivide()
{
  myMaxArea  = Precision::Infinite();
  myNbParts  = 0;
  myUnbSplit = myVnbSplit = -1;
  myIsSplittingByNumber   = Standard_False;
}

//=================================================================================================

ShapeUpgrade_ShapeDivideArea::ShapeUpgrade_ShapeDivideArea(const TopoDS_Shape& S)
    : ShapeUpgrade_ShapeDivide(S)

{
  myMaxArea  = Precision::Infinite();
  myNbParts  = 0;
  myUnbSplit = myVnbSplit = -1;
  myIsSplittingByNumber   = Standard_False;
}

//=================================================================================================

Handle(ShapeUpgrade_FaceDivide) ShapeUpgrade_ShapeDivideArea::GetSplitFaceTool() const
{
  Handle(ShapeUpgrade_FaceDivideArea) aFaceTool = new ShapeUpgrade_FaceDivideArea;
  aFaceTool->MaxArea()                          = myMaxArea;
  aFaceTool->NbParts()                          = myNbParts;
  aFaceTool->SetNumbersUVSplits(myUnbSplit, myVnbSplit);
  aFaceTool->SetSplittingByNumber(myIsSplittingByNumber);
  return aFaceTool;
}
