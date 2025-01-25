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

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade_FaceDivideArea.hxx>
#include <ShapeUpgrade_SplitSurfaceArea.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_FaceDivideArea, ShapeUpgrade_FaceDivide)

//=================================================================================================

ShapeUpgrade_FaceDivideArea::ShapeUpgrade_FaceDivideArea()
{
  myMaxArea  = Precision::Infinite();
  myNbParts  = 0;
  myUnbSplit = myVnbSplit = -1;
  myIsSplittingByNumber   = Standard_False;
  SetPrecision(1.e-5);
  SetSplitSurfaceTool(new ShapeUpgrade_SplitSurfaceArea);
}

//=================================================================================================

ShapeUpgrade_FaceDivideArea::ShapeUpgrade_FaceDivideArea(const TopoDS_Face& F)
{
  myMaxArea  = Precision::Infinite();
  myNbParts  = 0;
  myUnbSplit = myVnbSplit = -1;
  myIsSplittingByNumber   = Standard_False;
  SetPrecision(1.e-5);
  SetSplitSurfaceTool(new ShapeUpgrade_SplitSurfaceArea);
  Init(F);
}

//=================================================================================================

Standard_Boolean ShapeUpgrade_FaceDivideArea::Perform(const Standard_Real)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  GProp_GProps aGprop;

  BRepGProp::SurfaceProperties(myFace, aGprop, Precision());
  Standard_Real anArea = aGprop.Mass();

  Standard_Integer anbParts = 0;
  if (myMaxArea == -1)
  {
    anbParts  = myNbParts;
    myMaxArea = anArea / anbParts;
  }

  if ((anArea - myMaxArea) < Precision::Confusion())
    return Standard_False;

  if (anbParts == 0)
    anbParts = RealToInt(ceil(anArea / myMaxArea));

  Handle(ShapeUpgrade_SplitSurfaceArea) aSurfTool =
    Handle(ShapeUpgrade_SplitSurfaceArea)::DownCast(GetSplitSurfaceTool());
  if (aSurfTool.IsNull())
    return Standard_False;
  aSurfTool->NbParts() = anbParts;
  if (myIsSplittingByNumber)
  {
    aSurfTool->SetSplittingIntoSquares(Standard_True);
    aSurfTool->SetNumbersUVSplits(myUnbSplit, myVnbSplit);
  }
  if (!ShapeUpgrade_FaceDivide::Perform(anArea))
    return Standard_False;

  TopoDS_Shape aResult = Result();
  if (aResult.ShapeType() == TopAbs_FACE)
    return Standard_False;
  Standard_Integer aStatus = myStatus;

  if (!myIsSplittingByNumber)
  {
    TopExp_Explorer aExpF(aResult, TopAbs_FACE);
    TopoDS_Shape    aCopyRes = aResult.EmptyCopied();

    Standard_Boolean isModified = Standard_False;
    for (; aExpF.More(); aExpF.Next())
    {
      TopoDS_Shape aSh   = Context()->Apply(aExpF.Current());
      TopoDS_Face  aFace = TopoDS::Face(aSh);
      Init(aFace);
      BRep_Builder aB;
      if (Perform())
      {
        isModified           = Standard_True;
        TopoDS_Shape    aRes = Result();
        TopExp_Explorer aExpR(aRes, TopAbs_FACE);
        for (; aExpR.More(); aExpR.Next())
          aB.Add(aCopyRes, aExpR.Current());
      }
      else
        aB.Add(aCopyRes, aFace);
    }
    if (isModified)
    {
      if (aCopyRes.ShapeType() == TopAbs_WIRE || aCopyRes.ShapeType() == TopAbs_SHELL)
        aCopyRes.Closed(BRep_Tool::IsClosed(aCopyRes));
      Context()->Replace(aResult, aCopyRes);
    }
  }

  myStatus |= aStatus;
  myResult = Context()->Apply(aResult);
  return Status(ShapeExtend_DONE);
}
