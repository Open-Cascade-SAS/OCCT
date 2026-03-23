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

#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomNode.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_UsagesView.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

//=================================================================================================

void BRepGraphCheck::CheckSolidMinimum(
  const BRepGraph&                         theGraph,
  int                                      theSolidDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView   aDefs   = theGraph.Defs();
  const BRepGraph::UsagesView aUsages = theGraph.Usages();
  const BRepGraph_TopoNode::SolidDef& aSolidDef = aDefs.Solid(theSolidDefIdx);
  const BRepGraph_NodeId aSolidNodeId = aSolidDef.Id;

  if (aSolidDef.Usages.IsEmpty())
    return;

  // Check shell imbrication: no face should appear in multiple shells.
  const BRepGraph_TopoNode::SolidUsage& aSolidUsage =
    aUsages.Solid(aSolidDef.Usages.Value(0).Index);

  NCollection_Map<int> aAllFaces;
  for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
  {
    const BRepGraph_UsageId& aShellUsageId = aSolidUsage.ShellUsages.Value(aShellIter);
    const BRepGraph_TopoNode::ShellUsage& aShellUsage = aUsages.Shell(aShellUsageId.Index);

    // Check shell orientation: shells in a solid should have FORWARD or REVERSED orientation.
    const TopAbs_Orientation aShellOri = aShellUsage.Orientation;
    if (aShellOri != TopAbs_FORWARD && aShellOri != TopAbs_REVERSED)
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aShellUsage.DefId;
      anIssue.ContextNodeId = aSolidNodeId;
      anIssue.Status        = BRepCheck_BadOrientationOfSubshape;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }

    for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
    {
      const BRepGraph_NodeId aFaceDefId = theGraph.DefOf(aShellUsage.FaceUsages.Value(aFaceIter));
      if (!aFaceDefId.IsValid())
        continue;

      if (!aAllFaces.Add(aFaceDefId.Index))
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
  if (aSolidUsage.ShellUsages.Length() < 2)
    return;

  const BRepGraph::ShapesView aShapes = theGraph.Shapes();
  const TopoDS_Shape aSolid = aShapes.Shape(aSolidNodeId);
  if (aSolid.IsNull() || aSolid.ShapeType() != TopAbs_SOLID)
    return;

  // Build classifier on the outer solid.
  BRepClass3d_SolidClassifier aClassifier(TopoDS::Solid(aSolid));

  // For each inner shell (index > 0), find a representative face point and classify it.
  for (int aShellIter = 1; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
  {
    const BRepGraph_UsageId& aShellUsageId = aSolidUsage.ShellUsages.Value(aShellIter);
    const BRepGraph_NodeId aShellDefId = theGraph.DefOf(aShellUsageId);
    if (!aShellDefId.IsValid())
      continue;

    const BRepGraph_TopoNode::ShellUsage& aShellUsage = aUsages.Shell(aShellUsageId.Index);
    if (aShellUsage.FaceUsages.IsEmpty())
      continue;

    // Get a point from the first face of the inner shell.
    const BRepGraph_NodeId aFaceDefId = theGraph.DefOf(aShellUsage.FaceUsages.Value(0));
    if (!aFaceDefId.IsValid())
      continue;

    const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(aFaceDefId.Index);
    if (!aFaceDef.SurfNodeId.IsValid())
      continue;

    const BRepGraph::GeomView aGeom = theGraph.Geom();
    const BRepGraph_GeomNode::Surf& aSurfNode = aGeom.Surface(aFaceDef.SurfNodeId.Index);
    if (aSurfNode.Surface.IsNull())
      continue;

    // Use the surface midpoint as representative.
    double aUMin = 0.0, aUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
    aSurfNode.Surface->Bounds(aUMin, aUMax, aVMin, aVMax);
    // Clamp infinite bounds.
    if (aUMin < -1.0e6) aUMin = -1.0e6;
    if (aUMax >  1.0e6) aUMax =  1.0e6;
    if (aVMin < -1.0e6) aVMin = -1.0e6;
    if (aVMax >  1.0e6) aVMax =  1.0e6;

    gp_Pnt aRepPnt = aSurfNode.Surface->Value(0.5 * (aUMin + aUMax), 0.5 * (aVMin + aVMax));
    if (!aSurfNode.SurfaceLocation.IsIdentity())
    {
      aRepPnt.Transform(aSurfNode.SurfaceLocation.Transformation());
    }

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
  for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
  {
    const BRepGraph_TopoNode::ShellUsage& aShellUsage =
      aUsages.Shell(aSolidUsage.ShellUsages.Value(aShellIter).Index);
    if (aShellUsage.Orientation == TopAbs_FORWARD)
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
