// Copyright (c) 2022 OPEN CASCADE SAS
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
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <Poly.hxx>
#include <StdFail_NotDone.hxx>
#include <StepVisual_FaceOrSurface.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TriangulatedFace.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_MakeTessellatedItem.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=============================================================================
// 
//=============================================================================

TopoDSToStep_MakeTessellatedItem::TopoDSToStep_MakeTessellatedItem()
  : TopoDSToStep_Root()
{
}

TopoDSToStep_MakeTessellatedItem::
TopoDSToStep_MakeTessellatedItem(const TopoDS_Face& theFace,
                                 TopoDSToStep_Tool& theTool,
                                 const Handle(Transfer_FinderProcess)& theFP,
                                 const Message_ProgressRange& theProgress)
  : TopoDSToStep_Root()
{
  Init(theFace, theTool, theFP, theProgress);
}

//=============================================================================
// 
//=============================================================================

TopoDSToStep_MakeTessellatedItem::
TopoDSToStep_MakeTessellatedItem(const TopoDS_Shell& theShell,
                                 TopoDSToStep_Tool& theTool,
                                 const Handle(Transfer_FinderProcess)& theFP,
                                 const Message_ProgressRange& theProgress)
  : TopoDSToStep_Root()
{
  Init(theShell, theTool, theFP, theProgress);
}

//=============================================================================
// Create a TriangulatedFace of StepVisual from a Face of TopoDS
//=============================================================================

void TopoDSToStep_MakeTessellatedItem::Init(const TopoDS_Face& theFace,
                                            TopoDSToStep_Tool& theTool,
                                            const Handle(Transfer_FinderProcess)& theFP,
                                            const Message_ProgressRange& theProgress)
{
  done = Standard_False;

  if (theProgress.UserBreak())
    return;

  TopLoc_Location aLoc;
  const Handle(Poly_Triangulation)& aMesh = BRep_Tool::Triangulation(theFace, aLoc);
  if (!aMesh.IsNull()) 
  {
    Handle(StepVisual_TriangulatedFace) aTriaFace = new StepVisual_TriangulatedFace();
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
    Handle(StepVisual_CoordinatesList) aCoordinates = new StepVisual_CoordinatesList();
    Handle(TColgp_HArray1OfXYZ) aPoints = new TColgp_HArray1OfXYZ(1, aMesh->NbNodes());
    for (Standard_Integer i = 1; i <= aMesh->NbNodes(); ++i) 
    {
      aPoints->SetValue(i, aMesh->Node(i).XYZ());
    }
    aCoordinates->Init(aName, aPoints);
    Handle(TColStd_HArray2OfReal) aNormals = new TColStd_HArray2OfReal(1, aMesh->NbNodes(), 1, 3);
    if (!aMesh->HasNormals()) 
    {
      Poly::ComputeNormals(aMesh);
    }
    for (Standard_Integer i = 1; i <= aMesh->NbNodes(); ++i)
    {
      gp_Dir aNorm = aMesh->Normal(i);
      aNormals->SetValue(i, 1, aNorm.X());
      aNormals->SetValue(i, 2, aNorm.Y());
      aNormals->SetValue(i, 3, aNorm.Z());
    }
    const Standard_Boolean aHasGeomLink = theTool.IsBound(theFace);
    StepVisual_FaceOrSurface aGeomLink;
    if (aHasGeomLink) 
    {
      Handle(StepShape_TopologicalRepresentationItem) aTopoItem = theTool.Find(theFace);
      aGeomLink.SetValue(aTopoItem);
    }
    Handle(TColStd_HArray1OfInteger) anIndices = new TColStd_HArray1OfInteger(1, aMesh->NbNodes());
    for (Standard_Integer i = 1; i <= aMesh->NbNodes(); ++i) 
    {
      anIndices->SetValue(i, i);
    }
    Handle(TColStd_HArray2OfInteger) aTrias = new TColStd_HArray2OfInteger(1, aMesh->NbTriangles(), 1, 3);
    for (Standard_Integer i = 1; i <= aMesh->NbTriangles(); ++i) 
    {
      const Poly_Triangle& aT = aMesh->Triangle(i);
      aTrias->SetValue(i, 1, aT.Value(1));
      aTrias->SetValue(i, 2, aT.Value(2));
      aTrias->SetValue(i, 3, aT.Value(3));
    }
    aTriaFace->Init(aName, aCoordinates, aMesh->NbNodes(), aNormals, aHasGeomLink, aGeomLink, anIndices, aTrias);
    theTessellatedItem = aTriaFace;

    done = Standard_True;
  }
  else 
  {
    done = Standard_False;
    Handle(TransferBRep_ShapeMapper) anErrShape =
      new TransferBRep_ShapeMapper(theFace);
    theFP->AddWarning(anErrShape, " Face not mapped to TessellatedItem");
  }

}

//=============================================================================
// Create a TesselatedShell of StepVisual from a Shell of TopoDS
//=============================================================================

void TopoDSToStep_MakeTessellatedItem::Init(const TopoDS_Shell& theShell,
                                            TopoDSToStep_Tool& theTool,
                                            const Handle(Transfer_FinderProcess)& theFP,
                                            const Message_ProgressRange& theProgress)
{
  done = Standard_False;
  theTessellatedItem.Nullify();

  if (theProgress.UserBreak())
    return;

  TopExp_Explorer anExp;
  Standard_Integer aNbFaces = 0;
  for (anExp.Init(theShell, TopAbs_FACE); anExp.More(); anExp.Next(), ++aNbFaces) {}

  Message_ProgressScope aPS(theProgress, NULL, aNbFaces);

  NCollection_Sequence<Handle(StepVisual_TessellatedStructuredItem)> aTessFaces;
  for (anExp.Init(theShell, TopAbs_FACE); anExp.More() && aPS.More(); anExp.Next(), aPS.Next())
  {
    const TopoDS_Face aFace = TopoDS::Face(anExp.Current());
    TopoDSToStep_MakeTessellatedItem aMakeFace(aFace, theTool, theFP, aPS.Next());
    if (aMakeFace.IsDone()) 
    {
      aTessFaces.Append(Handle(StepVisual_TessellatedStructuredItem)::DownCast(aMakeFace.Value()));
    }
  }

  if (aTessFaces.IsEmpty())
  {
    return;
  }

  Handle(StepVisual_TessellatedShell) aTessShell = new StepVisual_TessellatedShell();
  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");

  Handle(StepVisual_HArray1OfTessellatedStructuredItem) anItems
    = new StepVisual_HArray1OfTessellatedStructuredItem(1, aTessFaces.Size());
  for (Standard_Integer i = aTessFaces.Lower(); i <= aTessFaces.Upper(); ++i)
  {
    anItems->SetValue(i, aTessFaces.Value(i));
  }

  Handle(StepShape_ConnectedFaceSet) aFaceSet;
  if (theTool.IsBound(theShell)) 
  {
    aFaceSet = Handle(StepShape_ConnectedFaceSet)::DownCast(theTool.Find(theShell));
  }

  const Standard_Boolean aHasTopoLink = !aFaceSet.IsNull();
  aTessShell->Init(aName, anItems, aHasTopoLink, aFaceSet);

  theTessellatedItem = aTessShell;

  //TopoDSToStep::AddResult(theFP, theShell, theTessellatedItem);
  done = Standard_True;
}

// ============================================================================
// Method  : TopoDSToStep_MakeTessellatedItem::Value
// Purpose : Returns TessellatedItem as the result
// ============================================================================

const Handle(StepVisual_TessellatedItem) &
TopoDSToStep_MakeTessellatedItem::Value() const
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeTessellatedItem::Value() - no result");
  return theTessellatedItem;
}
