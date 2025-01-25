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
#include <StepVisual_TriangulatedSurfaceSet.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_MakeTessellatedItem.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=============================================================================
// Method  : InitTriangulation
// Purpose : Get parameters from a TriangulatedFace or TriangulatedSurfaceSet
// Poly_Triangulation
//=============================================================================
static void InitTriangulation(const Handle(Poly_Triangulation)&       theMesh,
                              const Handle(TCollection_HAsciiString)& theName,
                              Handle(StepVisual_CoordinatesList)&     theCoordinates,
                              Handle(TColgp_HArray1OfXYZ)&            thePoints,
                              Handle(TColStd_HArray2OfReal)&          theNormals,
                              Handle(TColStd_HArray1OfInteger)&       theIndices,
                              Handle(TColStd_HArray2OfInteger)&       theTrias)
{
  for (Standard_Integer aNodeIndex = 1; aNodeIndex <= theMesh->NbNodes(); ++aNodeIndex)
  {
    thePoints->SetValue(aNodeIndex, theMesh->Node(aNodeIndex).XYZ());
  }
  theCoordinates->Init(theName, thePoints);
  if (!theMesh->HasNormals())
  {
    Poly::ComputeNormals(theMesh);
  }
  for (Standard_Integer aNodeIndex = 1; aNodeIndex <= theMesh->NbNodes(); ++aNodeIndex)
  {
    gp_Dir aNorm = theMesh->Normal(aNodeIndex);
    theNormals->SetValue(aNodeIndex, 1, aNorm.X());
    theNormals->SetValue(aNodeIndex, 2, aNorm.Y());
    theNormals->SetValue(aNodeIndex, 3, aNorm.Z());
  }
  for (Standard_Integer aNodeIndex = 1; aNodeIndex <= theMesh->NbNodes(); ++aNodeIndex)
  {
    theIndices->SetValue(aNodeIndex, aNodeIndex);
  }
  for (Standard_Integer aTriangleIndex = 1; aTriangleIndex <= theMesh->NbTriangles();
       ++aTriangleIndex)
  {
    const Poly_Triangle& aT = theMesh->Triangle(aTriangleIndex);
    theTrias->SetValue(aTriangleIndex, 1, aT.Value(1));
    theTrias->SetValue(aTriangleIndex, 2, aT.Value(2));
    theTrias->SetValue(aTriangleIndex, 3, aT.Value(3));
  }
}

//=================================================================================================

TopoDSToStep_MakeTessellatedItem::TopoDSToStep_MakeTessellatedItem()
    : TopoDSToStep_Root()
{
}

//=================================================================================================

TopoDSToStep_MakeTessellatedItem::TopoDSToStep_MakeTessellatedItem(
  const TopoDS_Face&                    theFace,
  TopoDSToStep_Tool&                    theTool,
  const Handle(Transfer_FinderProcess)& theFP,
  const Standard_Boolean                theToPreferSurfaceSet,
  const Message_ProgressRange&          theProgress)
    : TopoDSToStep_Root()
{
  Init(theFace, theTool, theFP, theToPreferSurfaceSet, theProgress);
}

//=================================================================================================

TopoDSToStep_MakeTessellatedItem::TopoDSToStep_MakeTessellatedItem(
  const TopoDS_Shell&                   theShell,
  TopoDSToStep_Tool&                    theTool,
  const Handle(Transfer_FinderProcess)& theFP,
  const Message_ProgressRange&          theProgress)
    : TopoDSToStep_Root()
{
  Init(theShell, theTool, theFP, theProgress);
}

//=============================================================================
// Method  : Init
// Purpose : Create a TriangulatedFace or TriangulatedSurfaceSet of StepVisual
// from a Face of TopoDS
//=============================================================================
void TopoDSToStep_MakeTessellatedItem::Init(const TopoDS_Face&                    theFace,
                                            TopoDSToStep_Tool&                    theTool,
                                            const Handle(Transfer_FinderProcess)& theFP,
                                            const Standard_Boolean       theToPreferSurfaceSet,
                                            const Message_ProgressRange& theProgress)
{
  done = Standard_False;
  if (theProgress.UserBreak())
    return;
  TopLoc_Location                   aLoc;
  const Handle(Poly_Triangulation)& aMesh = BRep_Tool::Triangulation(theFace, aLoc);
  if (aMesh.IsNull())
  {
    Handle(TransferBRep_ShapeMapper) anErrShape = new TransferBRep_ShapeMapper(theFace);
    theFP->AddWarning(anErrShape, " Face not mapped to TessellatedItem");
    return;
  }
  Handle(TCollection_HAsciiString)   aName        = new TCollection_HAsciiString("");
  Handle(StepVisual_CoordinatesList) aCoordinates = new StepVisual_CoordinatesList();
  Handle(TColgp_HArray1OfXYZ)        aPoints      = new TColgp_HArray1OfXYZ(1, aMesh->NbNodes());
  Handle(TColStd_HArray2OfReal)    aNormals  = new TColStd_HArray2OfReal(1, aMesh->NbNodes(), 1, 3);
  Handle(TColStd_HArray1OfInteger) anIndices = new TColStd_HArray1OfInteger(1, aMesh->NbNodes());
  Handle(TColStd_HArray2OfInteger) aTrias =
    new TColStd_HArray2OfInteger(1, aMesh->NbTriangles(), 1, 3);
  InitTriangulation(aMesh, aName, aCoordinates, aPoints, aNormals, anIndices, aTrias);

  const Standard_Boolean   aHasGeomLink = theTool.IsBound(theFace);
  StepVisual_FaceOrSurface aGeomLink;
  if (aHasGeomLink)
  {
    Handle(StepShape_TopologicalRepresentationItem) aTopoItem = theTool.Find(theFace);
    aGeomLink.SetValue(aTopoItem);
    Handle(StepVisual_TriangulatedFace) aTriaFace = new StepVisual_TriangulatedFace();
    aTriaFace->Init(aName,
                    aCoordinates,
                    aMesh->NbNodes(),
                    aNormals,
                    aHasGeomLink,
                    aGeomLink,
                    anIndices,
                    aTrias);
    theTessellatedItem = aTriaFace;
  }
  else if (theToPreferSurfaceSet)
  {
    Handle(StepVisual_TriangulatedSurfaceSet) aTriaSurfaceSet =
      new StepVisual_TriangulatedSurfaceSet();
    aTriaSurfaceSet->Init(aName, aCoordinates, aMesh->NbNodes(), aNormals, anIndices, aTrias);
    theTessellatedItem = aTriaSurfaceSet;
  }
  else
  {
    Handle(StepVisual_TriangulatedFace) aTriaFace = new StepVisual_TriangulatedFace();
    aTriaFace->Init(aName,
                    aCoordinates,
                    aMesh->NbNodes(),
                    aNormals,
                    aHasGeomLink,
                    aGeomLink,
                    anIndices,
                    aTrias);
    theTessellatedItem = aTriaFace;
  }
  done = Standard_True;
}

//=============================================================================
// Method  : Init
// Purpose : Create a TesselatedShell of StepVisual from a Shell of TopoDS
//=============================================================================
void TopoDSToStep_MakeTessellatedItem::Init(const TopoDS_Shell&                   theShell,
                                            TopoDSToStep_Tool&                    theTool,
                                            const Handle(Transfer_FinderProcess)& theFP,
                                            const Message_ProgressRange&          theProgress)
{
  done = Standard_False;
  theTessellatedItem.Nullify();

  if (theProgress.UserBreak())
    return;

  TopExp_Explorer  anExp;
  Standard_Integer aNbFaces = 0;
  for (anExp.Init(theShell, TopAbs_FACE); anExp.More(); anExp.Next(), ++aNbFaces)
  {
  }

  Message_ProgressScope aPS(theProgress, NULL, aNbFaces);

  NCollection_Sequence<Handle(StepVisual_TessellatedStructuredItem)> aTessFaces;
  for (anExp.Init(theShell, TopAbs_FACE); anExp.More() && aPS.More(); anExp.Next(), aPS.Next())
  {
    const TopoDS_Face                aFace = TopoDS::Face(anExp.Current());
    TopoDSToStep_MakeTessellatedItem aMakeFace(aFace, theTool, theFP, Standard_False, aPS.Next());
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
  Handle(TCollection_HAsciiString)    aName      = new TCollection_HAsciiString("");

  Handle(StepVisual_HArray1OfTessellatedStructuredItem) anItems =
    new StepVisual_HArray1OfTessellatedStructuredItem(1, aTessFaces.Size());
  for (Standard_Integer anIndx = aTessFaces.Lower(); anIndx <= aTessFaces.Upper(); ++anIndx)
  {
    anItems->SetValue(anIndx, aTessFaces.Value(anIndx));
  }

  Handle(StepShape_ConnectedFaceSet) aFaceSet;
  if (theTool.IsBound(theShell))
  {
    aFaceSet = Handle(StepShape_ConnectedFaceSet)::DownCast(theTool.Find(theShell));
  }

  const Standard_Boolean aHasTopoLink = !aFaceSet.IsNull();
  aTessShell->Init(aName, anItems, aHasTopoLink, aFaceSet);

  theTessellatedItem = aTessShell;

  // TopoDSToStep::AddResult(theFP, theShell, theTessellatedItem);
  done = Standard_True;
}

// ============================================================================
// Method  : Value
// Purpose : Returns TessellatedItem as the result
// ============================================================================
const Handle(StepVisual_TessellatedItem)& TopoDSToStep_MakeTessellatedItem::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeTessellatedItem::Value() - no result");
  return theTessellatedItem;
}
