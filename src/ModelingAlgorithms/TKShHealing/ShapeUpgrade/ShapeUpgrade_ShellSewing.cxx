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

#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeUpgrade_ShellSewing.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

//=================================================================================================

ShapeUpgrade_ShellSewing::ShapeUpgrade_ShellSewing()
    : myReShape(new ShapeBuild_ReShape)
{
}

//=================================================================================================

void ShapeUpgrade_ShellSewing::Init(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
  {
    return;
  }
  if (theShape.ShapeType() == TopAbs_SHELL)
  {
    myShells.Add(theShape);
  }
  else
  {
    for (TopExp_Explorer aShellExplorer(theShape, TopAbs_SHELL); aShellExplorer.More();
         aShellExplorer.Next())
    {
      myShells.Add(aShellExplorer.Current());
    }
  }
}

//=================================================================================================

size_t ShapeUpgrade_ShellSewing::Prepare(const double theTolerance)
{
  size_t aSewedShellCount = 0;
  for (const TopoDS_Shape& aShellShape : myShells)
  {
    const TopoDS_Shell    aShell = TopoDS::Shell(aShellShape);
    BRepBuilderAPI_Sewing aSewer(theTolerance);
    for (TopExp_Explorer aFaceExplorer(aShell, TopAbs_FACE); aFaceExplorer.More();
         aFaceExplorer.Next())
    {
      aSewer.Add(aFaceExplorer.Current());
    }
    aSewer.Perform();
    const TopoDS_Shape aSewedShape = aSewer.SewedShape();
    if (!aSewedShape.IsNull())
    {
      myReShape->Replace(aShell, aSewedShape);
      ++aSewedShellCount;
    }
  }
  return aSewedShellCount;
}

//=================================================================================================

TopoDS_Shape ShapeUpgrade_ShellSewing::Apply(const TopoDS_Shape& theShape,
                                             const double        theTolerance)
{
  if (theShape.IsNull() || myShells.IsEmpty())
  {
    return theShape;
  }

  TopoDS_Shape aResult = myReShape->Apply(theShape, TopAbs_FACE, 2);

  // Orient rebuilt solids so that the infinite point remains outside.
  myReShape->Clear();
  bool hasReversedSolid = false;
  for (TopExp_Explorer aSolidExplorer(theShape, TopAbs_SOLID); aSolidExplorer.More();
       aSolidExplorer.Next())
  {
    const TopoDS_Solid          aSolid = TopoDS::Solid(aSolidExplorer.Current());
    BRepClass3d_SolidClassifier aClassifier(aSolid);
    aClassifier.PerformInfinitePoint(theTolerance);
    if (aClassifier.State() == TopAbs_IN)
    {
      myReShape->Replace(aSolid, aSolid.Reversed());
      hasReversedSolid = true;
    }
  }

  if (hasReversedSolid)
  {
    aResult = myReShape->Apply(aResult, TopAbs_SHELL, 2);
  }

  return aResult;
}

//=================================================================================================

TopoDS_Shape ShapeUpgrade_ShellSewing::ApplySewing(const TopoDS_Shape& theShape,
                                                   const double        theTolerance)
{
  if (theShape.IsNull())
  {
    return theShape;
  }

  double aTolerance = theTolerance;
  if (aTolerance <= 0.0)
  {
    ShapeAnalysis_ShapeTolerance aToleranceAnalyzer;
    aTolerance = aToleranceAnalyzer.Tolerance(theShape, 0);
  }

  Init(theShape);
  if (Prepare(aTolerance) != 0)
  {
    return Apply(theShape, aTolerance);
  }

  return TopoDS_Shape();
}
