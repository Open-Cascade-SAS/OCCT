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
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Poly.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
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
static void InitTriangulation(const occ::handle<Poly_Triangulation>&       theMesh,
                              const occ::handle<TCollection_HAsciiString>& theName,
                              occ::handle<StepVisual_CoordinatesList>&     theCoordinates,
                              occ::handle<NCollection_HArray1<gp_XYZ>>&    thePoints,
                              occ::handle<NCollection_HArray2<double>>&    theNormals,
                              occ::handle<NCollection_HArray1<int>>&       theIndices,
                              occ::handle<NCollection_HArray2<int>>&       theTrias,
                              const StepData_Factors&                      theLocalFactors)
{
  double aFactor = theLocalFactors.LengthFactor();
  for (int aNodeIndex = 1; aNodeIndex <= theMesh->NbNodes(); ++aNodeIndex)
  {
    thePoints->SetValue(aNodeIndex, theMesh->Node(aNodeIndex).XYZ() / aFactor);
  }
  theCoordinates->Init(theName, thePoints);
  if (!theMesh->HasNormals())
  {
    Poly::ComputeNormals(theMesh);
  }
  for (int aNodeIndex = 1; aNodeIndex <= theMesh->NbNodes(); ++aNodeIndex)
  {
    gp_Dir aNorm = theMesh->Normal(aNodeIndex);
    theNormals->SetValue(aNodeIndex, 1, aNorm.X());
    theNormals->SetValue(aNodeIndex, 2, aNorm.Y());
    theNormals->SetValue(aNodeIndex, 3, aNorm.Z());
  }
  for (int aNodeIndex = 1; aNodeIndex <= theMesh->NbNodes(); ++aNodeIndex)
  {
    theIndices->SetValue(aNodeIndex, aNodeIndex);
  }
  for (int aTriangleIndex = 1; aTriangleIndex <= theMesh->NbTriangles(); ++aTriangleIndex)
  {
    const Poly_Triangle& aT = theMesh->Triangle(aTriangleIndex);
    theTrias->SetValue(aTriangleIndex, 1, aT.Value(1));
    theTrias->SetValue(aTriangleIndex, 2, aT.Value(2));
    theTrias->SetValue(aTriangleIndex, 3, aT.Value(3));
  }
}

//=================================================================================================

TopoDSToStep_MakeTessellatedItem::TopoDSToStep_MakeTessellatedItem()

  = default;

//=================================================================================================

TopoDSToStep_MakeTessellatedItem::TopoDSToStep_MakeTessellatedItem(
  const TopoDS_Face&                         theFace,
  TopoDSToStep_Tool&                         theTool,
  const occ::handle<Transfer_FinderProcess>& theFP,
  const bool                                 theToPreferSurfaceSet,
  const StepData_Factors&                    theLocalFactors,
  const Message_ProgressRange&               theProgress)

{
  Init(theFace, theTool, theFP, theToPreferSurfaceSet, theLocalFactors, theProgress);
}

//=================================================================================================

TopoDSToStep_MakeTessellatedItem::TopoDSToStep_MakeTessellatedItem(
  const TopoDS_Shell&                        theShell,
  TopoDSToStep_Tool&                         theTool,
  const occ::handle<Transfer_FinderProcess>& theFP,
  const StepData_Factors&                    theLocalFactors,
  const Message_ProgressRange&               theProgress)

{
  Init(theShell, theTool, theFP, theLocalFactors, theProgress);
}

//=============================================================================
// Method  : Init
// Purpose : Create a TriangulatedFace or TriangulatedSurfaceSet of StepVisual
// from a Face of TopoDS
//=============================================================================
void TopoDSToStep_MakeTessellatedItem::Init(const TopoDS_Face&                         theFace,
                                            TopoDSToStep_Tool&                         theTool,
                                            const occ::handle<Transfer_FinderProcess>& theFP,
                                            const bool                   theToPreferSurfaceSet,
                                            const StepData_Factors&      theLocalFactors,
                                            const Message_ProgressRange& theProgress)
{
  done = false;
  if (theProgress.UserBreak())
    return;
  TopLoc_Location                        aLoc;
  const occ::handle<Poly_Triangulation>& aMesh = BRep_Tool::Triangulation(theFace, aLoc);
  if (aMesh.IsNull())
  {
    occ::handle<TransferBRep_ShapeMapper> anErrShape = new TransferBRep_ShapeMapper(theFace);
    theFP->AddWarning(anErrShape, " Face not mapped to TessellatedItem");
    return;
  }
  occ::handle<TCollection_HAsciiString>    aName        = new TCollection_HAsciiString("");
  occ::handle<StepVisual_CoordinatesList>  aCoordinates = new StepVisual_CoordinatesList();
  occ::handle<NCollection_HArray1<gp_XYZ>> aPoints =
    new NCollection_HArray1<gp_XYZ>(1, aMesh->NbNodes());
  occ::handle<NCollection_HArray2<double>> aNormals =
    new NCollection_HArray2<double>(1, aMesh->NbNodes(), 1, 3);
  occ::handle<NCollection_HArray1<int>> anIndices =
    new NCollection_HArray1<int>(1, aMesh->NbNodes());
  occ::handle<NCollection_HArray2<int>> aTrias =
    new NCollection_HArray2<int>(1, aMesh->NbTriangles(), 1, 3);
  InitTriangulation(aMesh,
                    aName,
                    aCoordinates,
                    aPoints,
                    aNormals,
                    anIndices,
                    aTrias,
                    theLocalFactors);

  const bool               aHasGeomLink = theTool.IsBound(theFace);
  StepVisual_FaceOrSurface aGeomLink;
  if (aHasGeomLink)
  {
    occ::handle<StepShape_TopologicalRepresentationItem> aTopoItem = theTool.Find(theFace);
    aGeomLink.SetValue(aTopoItem);
    occ::handle<StepVisual_TriangulatedFace> aTriaFace = new StepVisual_TriangulatedFace();
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
    occ::handle<StepVisual_TriangulatedSurfaceSet> aTriaSurfaceSet =
      new StepVisual_TriangulatedSurfaceSet();
    aTriaSurfaceSet->Init(aName, aCoordinates, aMesh->NbNodes(), aNormals, anIndices, aTrias);
    theTessellatedItem = aTriaSurfaceSet;
  }
  else
  {
    occ::handle<StepVisual_TriangulatedFace> aTriaFace = new StepVisual_TriangulatedFace();
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
  done = true;
}

//=============================================================================
// Method  : Init
// Purpose : Create a TesselatedShell of StepVisual from a Shell of TopoDS
//=============================================================================
void TopoDSToStep_MakeTessellatedItem::Init(const TopoDS_Shell&                        theShell,
                                            TopoDSToStep_Tool&                         theTool,
                                            const occ::handle<Transfer_FinderProcess>& theFP,
                                            const StepData_Factors&      theLocalFactors,
                                            const Message_ProgressRange& theProgress)
{
  done = false;
  theTessellatedItem.Nullify();

  if (theProgress.UserBreak())
    return;

  TopExp_Explorer anExp;
  int             aNbFaces = 0;
  for (anExp.Init(theShell, TopAbs_FACE); anExp.More(); anExp.Next(), ++aNbFaces)
  {
  }

  Message_ProgressScope aPS(theProgress, nullptr, aNbFaces);

  NCollection_Sequence<occ::handle<StepVisual_TessellatedStructuredItem>> aTessFaces;
  for (anExp.Init(theShell, TopAbs_FACE); anExp.More() && aPS.More(); anExp.Next(), aPS.Next())
  {
    const TopoDS_Face                aFace = TopoDS::Face(anExp.Current());
    TopoDSToStep_MakeTessellatedItem aMakeFace(aFace,
                                               theTool,
                                               theFP,
                                               false,
                                               theLocalFactors,
                                               aPS.Next());
    if (aMakeFace.IsDone())
    {
      aTessFaces.Append(occ::down_cast<StepVisual_TessellatedStructuredItem>(aMakeFace.Value()));
    }
  }

  if (aTessFaces.IsEmpty())
  {
    return;
  }

  occ::handle<StepVisual_TessellatedShell> aTessShell = new StepVisual_TessellatedShell();
  occ::handle<TCollection_HAsciiString>    aName      = new TCollection_HAsciiString("");

  occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>> anItems =
    new NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>(1,
                                                                               aTessFaces.Size());
  for (int anIndx = aTessFaces.Lower(); anIndx <= aTessFaces.Upper(); ++anIndx)
  {
    anItems->SetValue(anIndx, aTessFaces.Value(anIndx));
  }

  occ::handle<StepShape_ConnectedFaceSet> aFaceSet;
  if (theTool.IsBound(theShell))
  {
    aFaceSet = occ::down_cast<StepShape_ConnectedFaceSet>(theTool.Find(theShell));
  }

  const bool aHasTopoLink = !aFaceSet.IsNull();
  aTessShell->Init(aName, anItems, aHasTopoLink, aFaceSet);

  theTessellatedItem = aTessShell;

  // TopoDSToStep::AddResult(theFP, theShell, theTessellatedItem);
  done = true;
}

// ============================================================================
// Method  : Value
// Purpose : Returns TessellatedItem as the result
// ============================================================================
const occ::handle<StepVisual_TessellatedItem>& TopoDSToStep_MakeTessellatedItem::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeTessellatedItem::Value() - no result");
  return theTessellatedItem;
}
