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

#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphCheck.hxx>
#include <BRepGraphCheck_Analyzer.hxx>
#include <BRepGraphCheck_Issue.hxx>
#include <BRepGraphCheck_Report.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Helper: check if a specific status code is present in the report
// ---------------------------------------------------------------------------
static bool hasIssueWithStatus(const BRepGraphCheck_Report& theReport, BRepCheck_Status theStatus)
{
  const NCollection_Vector<BRepGraphCheck_Issue>& anIssues = theReport.Issues();
  for (int anIter = 0; anIter < anIssues.Length(); ++anIter)
  {
    if (anIssues.Value(anIter).Status == theStatus)
      return true;
  }
  return false;
}

// ---------------------------------------------------------------------------
// Basic validity tests: valid shapes should produce no errors
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, ValidBox_NoIssues)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.Perform();

  EXPECT_TRUE(anAnalyzer.IsValid()) << "Valid box should have no errors. Found "
                                    << anAnalyzer.Report().Issues().Length() << " issues.";
}

TEST(BRepGraphCheck_AnalyzerTest, ValidCylinder_NoIssues)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.Perform();
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Valid cylinder should have no errors. Found "
                                    << anAnalyzer.Report().Issues().Length() << " issues.";
}

TEST(BRepGraphCheck_AnalyzerTest, ValidSphere_NoIssues)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.Perform();

  EXPECT_TRUE(anAnalyzer.IsValid()) << "Valid sphere should have no errors. Found "
                                    << anAnalyzer.Report().Issues().Length() << " issues.";
}

TEST(BRepGraphCheck_AnalyzerTest, ValidFusedBoxes_NoIssues)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  BRepAlgoAPI_Fuse   aFuser(aBox1, aBox2);
  ASSERT_TRUE(aFuser.IsDone());
  const TopoDS_Shape& aFused = aFuser.Shape();

  BRepGraph aGraph;
  aGraph.Build(aFused);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.Perform();

  EXPECT_TRUE(anAnalyzer.IsValid()) << "Fused boxes should have no errors. Found "
                                    << anAnalyzer.Report().Issues().Length() << " issues.";
}

// ---------------------------------------------------------------------------
// Edge minimum checks
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, EdgeMinimum_ValidEdge_NoIssues)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraphCheck_Issue> aIssues;
  BRepGraphCheck::CheckEdgeMinimum(aGraph, BRepGraph_EdgeId(0), aIssues);
  EXPECT_TRUE(aIssues.IsEmpty()) << "Valid edge should have no minimum issues.";
}

// ---------------------------------------------------------------------------
// Face minimum checks
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, FaceMinimum_ValidFace_NoIssues)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraphCheck_Issue> aIssues;
  BRepGraphCheck::CheckFaceMinimum(aGraph, BRepGraph_FaceId(0), aIssues);
  EXPECT_TRUE(aIssues.IsEmpty()) << "Valid face should have no surface issues.";
}

// ---------------------------------------------------------------------------
// Report tests
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_ReportTest, EmptyReport_IsValid)
{
  BRepGraphCheck_Report aReport;
  EXPECT_TRUE(aReport.IsValid());
  EXPECT_FALSE(aReport.HasIssues());
}

TEST(BRepGraphCheck_ReportTest, AddIssue_MakesInvalid)
{
  BRepGraphCheck_Report aReport;

  BRepGraphCheck_Issue anIssue;
  anIssue.NodeId        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0);
  anIssue.Status        = BRepCheck_No3DCurve;
  anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue);

  EXPECT_TRUE(aReport.HasIssues());
  EXPECT_FALSE(aReport.IsValid());
  EXPECT_EQ(aReport.Issues().Length(), 1);
}

TEST(BRepGraphCheck_ReportTest, WarningOnly_StillValid)
{
  BRepGraphCheck_Report aReport;

  BRepGraphCheck_Issue anIssue;
  anIssue.NodeId        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0);
  anIssue.Status        = BRepCheck_FreeEdge;
  anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
  aReport.AddIssue(anIssue);

  EXPECT_TRUE(aReport.HasIssues());
  EXPECT_TRUE(aReport.IsValid());
}

TEST(BRepGraphCheck_ReportTest, IssuesForNode_ReturnsCorrectSubset)
{
  BRepGraphCheck_Report aReport;

  const BRepGraph_NodeId aNode0(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId aNode1(BRepGraph_NodeId::Kind::Edge, 1);

  BRepGraphCheck_Issue anIssue0;
  anIssue0.NodeId        = aNode0;
  anIssue0.Status        = BRepCheck_No3DCurve;
  anIssue0.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue0);

  BRepGraphCheck_Issue anIssue1;
  anIssue1.NodeId        = aNode1;
  anIssue1.Status        = BRepCheck_InvalidRange;
  anIssue1.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue1);

  BRepGraphCheck_Issue anIssue2;
  anIssue2.NodeId        = aNode0;
  anIssue2.Status        = BRepCheck_InvalidToleranceValue;
  anIssue2.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
  aReport.AddIssue(anIssue2);

  const NCollection_Vector<BRepGraphCheck_Issue> aNode0Issues = aReport.IssuesForNode(aNode0);
  EXPECT_EQ(aNode0Issues.Length(), 2);

  const NCollection_Vector<BRepGraphCheck_Issue> aNode1Issues = aReport.IssuesForNode(aNode1);
  EXPECT_EQ(aNode1Issues.Length(), 1);
}

TEST(BRepGraphCheck_ReportTest, Clear_ResetsEverything)
{
  BRepGraphCheck_Report aReport;

  BRepGraphCheck_Issue anIssue;
  anIssue.NodeId        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0);
  anIssue.Status        = BRepCheck_NoSurface;
  anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue);

  EXPECT_TRUE(aReport.HasIssues());
  aReport.Clear();
  EXPECT_FALSE(aReport.HasIssues());
  EXPECT_TRUE(aReport.IsValid());
}

// ---------------------------------------------------------------------------
// Incremental checks
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, IncrementalCheckEdge_ValidBox)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  for (int anEdgeIter = 0; anEdgeIter < aGraph.Defs().NbEdges(); ++anEdgeIter)
  {
    anAnalyzer.CheckEdge(BRepGraph_EdgeId(anEdgeIter));
  }

  EXPECT_TRUE(anAnalyzer.IsValid())
    << "Incremental edge check on valid box should produce no errors.";
}

TEST(BRepGraphCheck_AnalyzerTest, IncrementalCheckFace_ValidBox)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  for (int aFaceIter = 0; aFaceIter < aGraph.Defs().NbFaces(); ++aFaceIter)
  {
    anAnalyzer.CheckFace(BRepGraph_FaceId(aFaceIter));
  }

  EXPECT_TRUE(anAnalyzer.IsValid())
    << "Incremental face check on valid box should produce no errors.";
}

// ---------------------------------------------------------------------------
// Parallel execution
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, ParallelPerform_ValidBox_NoIssues)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.SetParallel(true);
  anAnalyzer.Perform();

  EXPECT_TRUE(anAnalyzer.IsValid()) << "Parallel check on valid box should have no errors. Found "
                                    << anAnalyzer.Report().Issues().Length() << " issues.";
}

// ---------------------------------------------------------------------------
// Shell checks on valid shapes
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, ShellChecks_ValidBox)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbShells(), 0);

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.CheckShell(BRepGraph_ShellId(0));

  // Box shell should be closed with consistent orientation.
  // Only warnings (FreeEdge) are acceptable, not errors.
  EXPECT_TRUE(anAnalyzer.IsValid());
}

// ---------------------------------------------------------------------------
// Consistency with BRepCheck_Analyzer
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, Consistency_VsBRepCheck_Box)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  // Legacy check.
  BRepCheck_Analyzer aLegacy(aBox);
  const bool         aLegacyValid = aLegacy.IsValid();

  // Graph check.
  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer aGraphCheck(aGraph);
  aGraphCheck.Perform();

  EXPECT_EQ(aGraphCheck.IsValid(), aLegacyValid)
    << "Graph check and legacy check should agree on box validity.";
}

TEST(BRepGraphCheck_AnalyzerTest, Consistency_VsBRepCheck_Cylinder)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape();

  BRepCheck_Analyzer aLegacy(aCyl);
  const bool         aLegacyValid = aLegacy.IsValid();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer aGraphCheck(aGraph);
  aGraphCheck.Perform();

  EXPECT_EQ(aGraphCheck.IsValid(), aLegacyValid)
    << "Graph check and legacy check should agree on cylinder validity.";
}

TEST(BRepGraphCheck_AnalyzerTest, Consistency_VsBRepCheck_Sphere)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepCheck_Analyzer aLegacy(aSphere);
  const bool         aLegacyValid = aLegacy.IsValid();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer aGraphCheck(aGraph);
  aGraphCheck.Perform();

  EXPECT_EQ(aGraphCheck.IsValid(), aLegacyValid)
    << "Graph check and legacy check should agree on sphere validity.";
}

// ---------------------------------------------------------------------------
// Negative tests: open shell (remove one face from box)
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, OpenShell_DetectsNotClosed)
{
  // Build a box and remove one face to create an open shell.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  NCollection_IndexedMap<TopoDS_Shape> aFaces;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
    aFaces.Add(anExp.Current());
  ASSERT_GE(aFaces.Extent(), 6);

  // Build a shell with only 5 of the 6 faces.
  BRep_Builder aBuilder;
  TopoDS_Shell anOpenShell;
  aBuilder.MakeShell(anOpenShell);
  for (int aFaceIter = 1; aFaceIter <= 5; ++aFaceIter)
  {
    aBuilder.Add(anOpenShell, aFaces(aFaceIter));
  }

  BRepGraph aGraph;
  aGraph.Build(anOpenShell);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbShells(), 0);

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.CheckShell(BRepGraph_ShellId(0));
  const BRepGraphCheck_Report& aReport = anAnalyzer.Report();

  EXPECT_TRUE(hasIssueWithStatus(aReport, BRepCheck_NotClosed))
    << "Open shell should report NotClosed.";
  EXPECT_TRUE(hasIssueWithStatus(aReport, BRepCheck_FreeEdge))
    << "Open shell should report FreeEdge.";
}

// ---------------------------------------------------------------------------
// Negative test: shell with inconsistent orientation
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, InconsistentShell_DetectsBadOrientation)
{
  // Build a box, reverse one face in the shell.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  NCollection_IndexedMap<TopoDS_Shape> aFaces;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
    aFaces.Add(anExp.Current());
  ASSERT_GE(aFaces.Extent(), 6);

  BRep_Builder aBuilder;
  TopoDS_Shell aBadShell;
  aBuilder.MakeShell(aBadShell);
  for (int aFaceIter = 1; aFaceIter <= aFaces.Extent(); ++aFaceIter)
  {
    if (aFaceIter == 1)
      aBuilder.Add(aBadShell, aFaces(aFaceIter).Reversed());
    else
      aBuilder.Add(aBadShell, aFaces(aFaceIter));
  }

  BRepGraph aGraph;
  aGraph.Build(aBadShell);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbShells(), 0);

  NCollection_Vector<BRepGraphCheck_Issue> aIssues;
  BRepGraphCheck::CheckShellOrientation(aGraph, BRepGraph_ShellId(0), aIssues);

  bool aHasBadOrientation = false;
  for (int anIter = 0; anIter < aIssues.Length(); ++anIter)
  {
    if (aIssues.Value(anIter).Status == BRepCheck_BadOrientation)
      aHasBadOrientation = true;
  }
  EXPECT_TRUE(aHasBadOrientation) << "Shell with reversed face should report BadOrientation.";
}

// ---------------------------------------------------------------------------
// Negative test: disconnected shell
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, DisconnectedShell_DetectsNotConnected)
{
  // Build two separate boxes, combine faces into a single shell.
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(100.0, 100.0, 100.0), 10.0, 10.0, 10.0).Shape();

  BRep_Builder aBuilder;
  TopoDS_Shell aDisconnectedShell;
  aBuilder.MakeShell(aDisconnectedShell);

  for (TopExp_Explorer anExp(aBox1, TopAbs_FACE); anExp.More(); anExp.Next())
    aBuilder.Add(aDisconnectedShell, anExp.Current());
  for (TopExp_Explorer anExp(aBox2, TopAbs_FACE); anExp.More(); anExp.Next())
    aBuilder.Add(aDisconnectedShell, anExp.Current());

  BRepGraph aGraph;
  aGraph.Build(aDisconnectedShell);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbShells(), 0);

  NCollection_Vector<BRepGraphCheck_Issue> aIssues;
  BRepGraphCheck::CheckShellMinimum(aGraph, BRepGraph_ShellId(0), aIssues);

  bool aHasNotConnected = false;
  for (int anIter = 0; anIter < aIssues.Length(); ++anIter)
  {
    if (aIssues.Value(anIter).Status == BRepCheck_NotConnected)
      aHasNotConnected = true;
  }
  EXPECT_TRUE(aHasNotConnected) << "Disconnected shell should report NotConnected.";
}

// ---------------------------------------------------------------------------
// Valid shapes with geometric controls enabled
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, GeomControls_ValidBox_NoIssues)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.SetGeometricControls(true);
  anAnalyzer.Perform();

  EXPECT_TRUE(anAnalyzer.IsValid())
    << "Valid box with geometric controls should have no errors. Found "
    << anAnalyzer.Report().Issues().Length() << " issues.";
}

TEST(BRepGraphCheck_AnalyzerTest, GeomControls_ValidCylinder_NoIssues)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.SetGeometricControls(true);
  anAnalyzer.Perform();

  EXPECT_TRUE(anAnalyzer.IsValid())
    << "Valid cylinder with geometric controls should have no errors. Found "
    << anAnalyzer.Report().Issues().Length() << " issues.";
}

TEST(BRepGraphCheck_AnalyzerTest, GeomControls_ValidSphere_NoIssues)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.SetGeometricControls(true);
  anAnalyzer.Perform();

  EXPECT_TRUE(anAnalyzer.IsValid())
    << "Valid sphere with geometric controls should have no errors. Found "
    << anAnalyzer.Report().Issues().Length() << " issues.";
}

// ---------------------------------------------------------------------------
// Negative test: displaced vertex -> InvalidPointOnCurve
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, DisplacedVertex_DetectsInvalidPointOnCurve)
{
  // Build a box, extract first edge, displace its start vertex.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  TopExp_Explorer anEdgeExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());

  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(anEdge, aV1, aV2);

  // Displace start vertex far from the curve.
  BRep_Builder aBB;
  aBB.UpdateVertex(aV1, gp_Pnt(999.0, 999.0, 999.0), Precision::Confusion());

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Find the edge and its vertices, check for InvalidPointOnCurve.
  const BRepGraph::DefsView aDefs            = aGraph.Defs();
  bool                      aHasInvalidPoint = false;
  for (int anEdgeIter = 0; anEdgeIter < aDefs.NbEdges(); ++anEdgeIter)
  {
    const BRepGraph_EdgeId                   anEdgeId  = BRepGraph_EdgeId(anEdgeIter);
    const BRepGraph_TopoNode::EdgeDef&       anEdgeDef = aDefs.Edge(anEdgeId);
    NCollection_Vector<BRepGraphCheck_Issue> aIssues;
    if (anEdgeDef.StartVertexDefId().IsValid())
    {
      BRepGraphCheck::CheckVertexOnEdge(aGraph,
                                        anEdgeDef.StartVertex.VertexDefId,
                                        anEdgeId,
                                        aIssues);
    }
    if (anEdgeDef.EndVertexDefId().IsValid())
    {
      BRepGraphCheck::CheckVertexOnEdge(aGraph,
                                        anEdgeDef.EndVertex.VertexDefId,
                                        anEdgeId,
                                        aIssues);
    }
    for (int anIssueIter = 0; anIssueIter < aIssues.Length(); ++anIssueIter)
    {
      if (aIssues.Value(anIssueIter).Status == BRepCheck_InvalidPointOnCurve)
        aHasInvalidPoint = true;
    }
    if (aHasInvalidPoint)
      break;
  }
  EXPECT_TRUE(aHasInvalidPoint) << "Displaced vertex should report InvalidPointOnCurve.";
}

// ---------------------------------------------------------------------------
// Negative test: edge missing PCurve on face -> NoCurveOnSurface
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, MissingPCurve_DetectsNoCurveOnSurface)
{
  // Build a box, find an edge that doesn't belong to a specific face,
  // then call CheckEdgeOnFace directly to verify NoCurveOnSurface detection.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph::DefsView aDefs = aGraph.Defs();
  ASSERT_GT(aDefs.NbFaces(), 1);
  ASSERT_GT(aDefs.NbEdges(), 0);

  // Find an edge that does NOT have a PCurve on face 0.
  // A box face has 4 edges; the box has 12 edges total.
  // So at least some edges are not on face 0.
  bool aHasNoCurveOnSurface = false;
  for (int anEdgeIter = 0; anEdgeIter < aDefs.NbEdges(); ++anEdgeIter)
  {
    const BRepGraph_EdgeId anEdgeId(anEdgeIter);
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anEdgeId)))
      continue;

    // Check if this edge has a PCurve on face 0.
    const BRepGraph_FaceId             aFaceId(0);
    const BRepGraphInc::CoEdgeEntity* aPCEntry =
      BRepGraph_Tool::Edge::FindPCurve(aGraph, BRepGraph_EdgeId(anEdgeId), aFaceId);
    if (aPCEntry != nullptr)
      continue; // This edge has a PCurve on face 0, skip it.

    // Found an edge without PCurve on face 0 - verify detection.
    NCollection_Vector<BRepGraphCheck_Issue> aIssues;
    BRepGraphCheck::CheckEdgeOnFace(aGraph, anEdgeId, aFaceId, false, aIssues);
    for (int anIssueIter = 0; anIssueIter < aIssues.Length(); ++anIssueIter)
    {
      if (aIssues.Value(anIssueIter).Status == BRepCheck_NoCurveOnSurface)
        aHasNoCurveOnSurface = true;
    }
    if (aHasNoCurveOnSurface)
      break;
  }
  EXPECT_TRUE(aHasNoCurveOnSurface)
    << "Edge without PCurve on face should report NoCurveOnSurface.";
}

// ---------------------------------------------------------------------------
// Negative test: wire with self-intersection -> SelfIntersectingWire
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, SelfIntersectingWire_DetectsCrossing)
{
  // Build a "bowtie" wire on a plane: two edges cross at (0.5, 0.5, 0).
  // Edge1: (0,0,0)->(1,1,0), Edge2: (1,1,0)->(1,0,0),
  // Edge3: (1,0,0)->(0,1,0) (crosses Edge1), Edge4: (0,1,0)->(0,0,0).
  const gp_Pnt aP00(0.0, 0.0, 0.0);
  const gp_Pnt aP11(1.0, 1.0, 0.0);
  const gp_Pnt aP10(1.0, 0.0, 0.0);
  const gp_Pnt aP01(0.0, 1.0, 0.0);

  BRepBuilderAPI_MakeEdge aE1(aP00, aP11);
  BRepBuilderAPI_MakeEdge aE2(aP11, aP10);
  BRepBuilderAPI_MakeEdge aE3(aP10, aP01);
  BRepBuilderAPI_MakeEdge aE4(aP01, aP00);
  ASSERT_TRUE(aE1.IsDone() && aE2.IsDone() && aE3.IsDone() && aE4.IsDone());

  BRepBuilderAPI_MakeWire aMakeWire;
  aMakeWire.Add(aE1.Edge());
  aMakeWire.Add(aE2.Edge());
  aMakeWire.Add(aE3.Edge());
  aMakeWire.Add(aE4.Edge());
  ASSERT_TRUE(aMakeWire.IsDone());

  // Build a face from the wire (which computes PCurves).
  BRepBuilderAPI_MakeFace aMakeFace(gp_Pln(), aMakeWire.Wire());
  ASSERT_TRUE(aMakeFace.IsDone());

  BRepGraph aGraph;
  aGraph.Build(aMakeFace.Face());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphCheck_Analyzer anAnalyzer(aGraph);
  anAnalyzer.SetGeometricControls(true);
  anAnalyzer.Perform();

  EXPECT_TRUE(hasIssueWithStatus(anAnalyzer.Report(), BRepCheck_SelfIntersectingWire))
    << "Bowtie wire should report SelfIntersectingWire.";
}

// ---------------------------------------------------------------------------
// Negative test: solid with shared face between shells -> InvalidImbricationOfShells
// ---------------------------------------------------------------------------

TEST(BRepGraphCheck_AnalyzerTest, SharedFaceBetweenShells_DetectsImbrication)
{
  // Build a box and split its faces into two shells with one shared face.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  NCollection_IndexedMap<TopoDS_Shape> aFaces;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
    aFaces.Add(anExp.Current());
  ASSERT_GE(aFaces.Extent(), 6);

  BRep_Builder aBB;
  TopoDS_Shell aShell1, aShell2;
  aBB.MakeShell(aShell1);
  aBB.MakeShell(aShell2);

  // Shell1 gets faces 1-3, Shell2 gets faces 3-6 (face 3 is shared).
  for (int aFaceIter = 1; aFaceIter <= 3; ++aFaceIter)
    aBB.Add(aShell1, aFaces(aFaceIter));
  for (int aFaceIter = 3; aFaceIter <= 6; ++aFaceIter)
    aBB.Add(aShell2, aFaces(aFaceIter));

  TopoDS_Solid aSolid;
  aBB.MakeSolid(aSolid);
  aBB.Add(aSolid, aShell1);
  aBB.Add(aSolid, aShell2);

  BRepGraph aGraph;
  aGraph.Build(aSolid);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbSolids(), 0);

  NCollection_Vector<BRepGraphCheck_Issue> aIssues;
  BRepGraphCheck::CheckSolidMinimum(aGraph, BRepGraph_SolidId(0), aIssues);

  bool aHasImbrication = false;
  for (int anIter = 0; anIter < aIssues.Length(); ++anIter)
  {
    if (aIssues.Value(anIter).Status == BRepCheck_InvalidImbricationOfShells)
      aHasImbrication = true;
  }
  EXPECT_TRUE(aHasImbrication)
    << "Solid with shared face between shells should report InvalidImbricationOfShells.";
}
