// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraphCheck.hxx>
#include <BRepGraphInc_Definition.hxx>

#include <BRepGraph_RefsView.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

namespace
{

//=================================================================================================

static NCollection_Vector<BRepGraphInc::ShellRefEntry> collectSolidShellRefs(
  const BRepGraph::TopoView& theDefs,
  const BRepGraph::RefsView& theRefs,
  const BRepGraph_NodeId     theSolidNodeId)
{
  NCollection_Vector<BRepGraphInc::ShellRefEntry> aResult;
  const BRepGraphInc::SolidDef& aSolidDef = theDefs.Solid(BRepGraph_SolidId(theSolidNodeId.Index));
  for (int aRefIter = 0; aRefIter < aSolidDef.ShellRefIds.Length(); ++aRefIter)
  {
    const BRepGraph_ShellRefId          aRefId = aSolidDef.ShellRefIds.Value(aRefIter);
    const BRepGraphInc::ShellRefEntry& aRef   = theRefs.Shell(aRefId);
    if (aRef.IsRemoved || !aRef.ShellDefId.IsValid(theDefs.NbShells()))
    {
      continue;
    }
    aResult.Append(aRef);
  }
  return aResult;
}

//=================================================================================================

static NCollection_Vector<BRepGraphInc::FaceRefEntry> collectShellFaceRefs(
  const BRepGraph::TopoView& theDefs,
  const BRepGraph::RefsView& theRefs,
  const BRepGraph_NodeId     theShellNodeId)
{
  NCollection_Vector<BRepGraphInc::FaceRefEntry> aResult;
  const BRepGraphInc::ShellDef& aShellDef = theDefs.Shell(BRepGraph_ShellId(theShellNodeId.Index));
  for (int aRefIter = 0; aRefIter < aShellDef.FaceRefIds.Length(); ++aRefIter)
  {
    const BRepGraph_FaceRefId          aRefId = aShellDef.FaceRefIds.Value(aRefIter);
    const BRepGraphInc::FaceRefEntry& aRef   = theRefs.Face(aRefId);
    if (aRef.IsRemoved || !aRef.FaceDefId.IsValid(theDefs.NbFaces()))
    {
      continue;
    }
    aResult.Append(aRef);
  }
  return aResult;
}

} // namespace

//=================================================================================================

void BRepGraphCheck::CheckSolidMinimum(const BRepGraph&                          theGraph,
                                       const BRepGraph_SolidId                   theSolid,
                                       NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::TopoView&          aDefs        = theGraph.Topo();
  const BRepGraph::RefsView&          aRefs        = theGraph.Refs();
  const BRepGraphInc::SolidDef& aSolidDef    = aDefs.Solid(theSolid);
  const BRepGraph_NodeId              aSolidNodeId = aSolidDef.Id;
  const NCollection_Vector<BRepGraphInc::ShellRefEntry> aSolidShellRefs =
    collectSolidShellRefs(aDefs, aRefs, aSolidNodeId);

  if (aSolidShellRefs.IsEmpty())
    return;

  // Check shell imbrication: no face should appear in multiple shells.
  NCollection_Map<int> aAllFaces;
  for (int aShellIter = 0; aShellIter < aSolidShellRefs.Length(); ++aShellIter)
  {
    const BRepGraphInc::ShellRefEntry&  aSR          = aSolidShellRefs.Value(aShellIter);
    const BRepGraphInc::ShellDef& aShellDef    = aDefs.Shell(aSR.ShellDefId);
    const BRepGraph_NodeId              aShellNodeId = BRepGraph_NodeId::Shell(aSR.ShellDefId.Index);
    const NCollection_Vector<BRepGraphInc::FaceRefEntry> aShellFaceRefs =
      collectShellFaceRefs(aDefs, aRefs, aShellNodeId);

    // Check shell orientation: shells in a solid should have FORWARD or REVERSED orientation.
    const TopAbs_Orientation aShellOri = aSR.Orientation;
    if (aShellOri != TopAbs_FORWARD && aShellOri != TopAbs_REVERSED)
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aShellDef.Id;
      anIssue.ContextNodeId = aSolidNodeId;
      anIssue.Status        = BRepCheck_BadOrientationOfSubshape;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }

    for (int aFaceIter = 0; aFaceIter < aShellFaceRefs.Length(); ++aFaceIter)
    {
      const BRepGraphInc::FaceRefEntry& aFR     = aShellFaceRefs.Value(aFaceIter);
      const BRepGraph_NodeId       aFaceDefId = aFR.FaceDefId;

      if (!aAllFaces.Add(aFR.FaceDefId.Index))
      {
        // Face appears in multiple shells.
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = aFaceDefId;
        anIssue.ContextNodeId = aSolidNodeId;
        anIssue.Status        = BRepCheck_InvalidImbricationOfShells;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
      }
    }
  }

  // Solid containment checks via BRepClass3d_SolidClassifier.
  // Requires shape reconstruction for the solid and inner shells.
  if (aSolidShellRefs.Length() < 2)
    return;

  const BRepGraph::ShapesView& aShapes = theGraph.Shapes();
  const TopoDS_Shape          aSolid  = aShapes.Shape(aSolidNodeId);
  if (aSolid.IsNull() || aSolid.ShapeType() != TopAbs_SOLID)
    return;

  // Build classifier on the outer solid.
  BRepClass3d_SolidClassifier aClassifier(TopoDS::Solid(aSolid));

  // For each inner shell (index > 0), find a representative face point and classify it.
  for (int aShellIter = 1; aShellIter < aSolidShellRefs.Length(); ++aShellIter)
  {
    const BRepGraphInc::ShellRefEntry& aSR         = aSolidShellRefs.Value(aShellIter);
    const BRepGraph_NodeId             aShellDefId = aSR.ShellDefId;
    const NCollection_Vector<BRepGraphInc::FaceRefEntry> aShellFaceRefs =
      collectShellFaceRefs(aDefs, aRefs, BRepGraph_NodeId::Shell(aSR.ShellDefId.Index));

    if (aShellFaceRefs.IsEmpty())
      continue;

    // Get a point from the first face of the inner shell.
    const BRepGraphInc::FaceRefEntry& aFR = aShellFaceRefs.Value(0);

    if (!BRepGraph_Tool::Face::HasSurface(theGraph, aFR.FaceDefId))
      continue;

    // Use the surface midpoint as representative.
    const GeomAdaptor_TransformedSurface aSurfAdaptor =
      BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, aFR.FaceDefId);
    double aUMin = aSurfAdaptor.FirstUParameter();
    double aUMax = aSurfAdaptor.LastUParameter();
    double aVMin = aSurfAdaptor.FirstVParameter();
    double aVMax = aSurfAdaptor.LastVParameter();
    // Clamp infinite bounds.
    if (aUMin < -Precision::Infinite())
      aUMin = -Precision::Infinite();
    if (aUMax > Precision::Infinite())
      aUMax = Precision::Infinite();
    if (aVMin < -Precision::Infinite())
      aVMin = -Precision::Infinite();
    if (aVMax > Precision::Infinite())
      aVMax = Precision::Infinite();

    // Geometry is stored at identity, no location transform needed.
    const gp_Pnt aRepPnt = aSurfAdaptor.Value(0.5 * (aUMin + aUMax), 0.5 * (aVMin + aVMax));

    aClassifier.Perform(aRepPnt, Precision::Confusion());
    const TopAbs_State aState = aClassifier.State();

    if (aState == TopAbs_OUT)
    {
      // Inner shell representative point is outside the solid.
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aShellDefId;
      anIssue.ContextNodeId = aSolidNodeId;
      anIssue.Status        = BRepCheck_SubshapeNotInShape;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
  }

  // Check for enclosed regions: multiple FORWARD shells would mean
  // multiple outer boundaries, which is invalid for a single solid.
  int aForwardShellCount = 0;
  for (int aShellIter = 0; aShellIter < aSolidShellRefs.Length(); ++aShellIter)
  {
    const BRepGraphInc::ShellRefEntry& aSR = aSolidShellRefs.Value(aShellIter);
    if (aSR.Orientation == TopAbs_FORWARD)
      aForwardShellCount++;
  }

  if (aForwardShellCount > 1)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aSolidNodeId;
    anIssue.Status        = BRepCheck_EnclosedRegion;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}
