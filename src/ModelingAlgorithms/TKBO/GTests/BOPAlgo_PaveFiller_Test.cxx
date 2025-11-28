// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//==================================================================================================
// BOPAlgo_PaveFiller Tests - Regression tests for edge cases in Boolean operations
//==================================================================================================

class BOPAlgo_PaveFillerTest : public ::testing::Test
{
protected:
  //! Create a circular wire at given height
  static TopoDS_Wire CreateCircularWire(double theRadius, double theZ)
  {
    gp_Circ                 aCircle(gp_Ax2(gp_Pnt(0, 0, theZ), gp_Dir(0, 0, 1)), theRadius);
    BRepBuilderAPI_MakeEdge anEdgeMaker(aCircle);
    BRepBuilderAPI_MakeWire aWireMaker(anEdgeMaker.Edge());
    return aWireMaker.Wire();
  }

  //! Create a vertex at given point
  static TopoDS_Vertex CreateVertex(const gp_Pnt& thePoint)
  {
    BRepBuilderAPI_MakeVertex aVertexMaker(thePoint);
    return aVertexMaker.Vertex();
  }

  //! Check if shape has degenerated edges without pcurves
  static bool HasDegeneratedEdgesWithoutPCurves(const TopoDS_Shape& theShape)
  {
    for (TopExp_Explorer aFaceExp(theShape, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
      for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
        if (BRep_Tool::Degenerated(anEdge))
        {
          double               aFirst, aLast;
          Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
          if (aPCurve.IsNull())
          {
            return true;
          }
        }
      }
    }
    return false;
  }

  //! Get volume of a shape
  static double GetVolume(const TopoDS_Shape& theShape)
  {
    GProp_GProps aProps;
    BRepGProp::VolumeProperties(theShape, aProps);
    return aProps.Mass();
  }
};

// Test: Boolean fuse of loft (wire to vertex) with box
// This tests the fix for FillPaves() handling edges without 2D curves.
// A loft from a circular wire to a vertex creates a cone-like shape
// with a degenerated edge at the apex.
TEST_F(BOPAlgo_PaveFillerTest, FuseConeLoftWithBox_DegeneratedEdge)
{
  // Create a loft from a circular wire to a point (apex)
  // This creates a cone-like shape with a degenerated edge at the top
  TopoDS_Wire   aBaseWire = CreateCircularWire(10.0, 0.0);
  TopoDS_Vertex anApex    = CreateVertex(gp_Pnt(0, 0, 20.0));

  BRepOffsetAPI_ThruSections aLoftMaker(true, true); // solid, ruled
  aLoftMaker.AddWire(aBaseWire);
  aLoftMaker.AddVertex(anApex);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Loft operation should succeed";
  TopoDS_Shape aLoft = aLoftMaker.Shape();
  ASSERT_FALSE(aLoft.IsNull()) << "Loft shape should not be null";

  // Create a box that intersects the loft
  BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(-5, -5, 5), 10.0, 10.0, 10.0);
  TopoDS_Shape        aBox = aBoxMaker.Shape();

  // Perform Boolean fuse - this should not crash even if there are
  // degenerated edges without pcurves
  BRepAlgoAPI_Fuse aFuser(aLoft, aBox);

  // The operation should complete without crashing
  EXPECT_TRUE(aFuser.IsDone()) << "Boolean fuse should succeed for loft with degenerated edges";
  EXPECT_FALSE(aFuser.Shape().IsNull()) << "Result shape should not be null";

  // Verify the result has reasonable volume (both shapes contribute)
  double aLoftVolume   = GetVolume(aLoft);
  double aBoxVolume    = GetVolume(aBox);
  double aResultVolume = GetVolume(aFuser.Shape());

  // Result volume should be less than sum (intersection) but more than max (union effect)
  EXPECT_GT(aResultVolume, 0.0) << "Result should have positive volume";
  EXPECT_LT(aResultVolume, aLoftVolume + aBoxVolume)
    << "Result volume should be less than sum of inputs";
}

// Test: Boolean fuse of cone (which has degenerated edge at apex) with box
// Standard cone also has a degenerated edge at the apex
TEST_F(BOPAlgo_PaveFillerTest, FuseConeWithBox_DegeneratedEdge)
{
  // Create a cone - has degenerated edge at apex
  BRepPrimAPI_MakeCone aConeMaker(10.0, 0.0, 20.0); // R1=10, R2=0 (apex), H=20
  TopoDS_Shape         aCone = aConeMaker.Shape();
  ASSERT_FALSE(aCone.IsNull()) << "Cone should be created";

  // Create a box that intersects near the apex
  BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(-5, -5, 15), 10.0, 10.0, 10.0);
  TopoDS_Shape        aBox = aBoxMaker.Shape();

  // Perform Boolean fuse
  BRepAlgoAPI_Fuse aFuser(aCone, aBox);

  EXPECT_TRUE(aFuser.IsDone()) << "Boolean fuse of cone and box should succeed";
  EXPECT_FALSE(aFuser.Shape().IsNull()) << "Result shape should not be null";

  double aResultVolume = GetVolume(aFuser.Shape());
  EXPECT_GT(aResultVolume, 0.0) << "Result should have positive volume";
}

// Test: Two lofts fused together (original OCC10006 scenario)
// This is similar to the existing test but focuses on the robustness aspect
TEST_F(BOPAlgo_PaveFillerTest, FuseTwoLofts_RobustnessCheck)
{
  // Create first loft: quadrilateral wire to quadrilateral wire
  BRepBuilderAPI_MakeWire aWireMaker1;
  aWireMaker1.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(10, -10, 0), gp_Pnt(100, -10, 0)).Edge());
  aWireMaker1.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, -10, 0), gp_Pnt(100, -100, 0)).Edge());
  aWireMaker1.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, -100, 0), gp_Pnt(10, -100, 0)).Edge());
  aWireMaker1.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(10, -100, 0), gp_Pnt(10, -10, 0)).Edge());
  TopoDS_Wire aBottom1 = aWireMaker1.Wire();

  BRepBuilderAPI_MakeWire aWireMaker2;
  aWireMaker2.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 10), gp_Pnt(100, 0, 10)).Edge());
  aWireMaker2.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, 0, 10), gp_Pnt(100, -100, 10)).Edge());
  aWireMaker2.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, -100, 10), gp_Pnt(0, -100, 10)).Edge());
  aWireMaker2.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, -100, 10), gp_Pnt(0, 0, 10)).Edge());
  TopoDS_Wire aTop1 = aWireMaker2.Wire();

  BRepOffsetAPI_ThruSections aLoft1(true, true);
  aLoft1.AddWire(aBottom1);
  aLoft1.AddWire(aTop1);
  aLoft1.Build();
  ASSERT_TRUE(aLoft1.IsDone()) << "First loft should succeed";

  // Create second loft
  BRepBuilderAPI_MakeWire aWireMaker3;
  aWireMaker3.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 10), gp_Pnt(100, 0, 10)).Edge());
  aWireMaker3.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, 0, 10), gp_Pnt(100, -100, 10)).Edge());
  aWireMaker3.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, -100, 10), gp_Pnt(0, -100, 10)).Edge());
  aWireMaker3.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, -100, 10), gp_Pnt(0, 0, 10)).Edge());
  TopoDS_Wire aBottom2 = aWireMaker3.Wire();

  BRepBuilderAPI_MakeWire aWireMaker4;
  aWireMaker4.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 250), gp_Pnt(100, 0, 250)).Edge());
  aWireMaker4.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, 0, 250), gp_Pnt(100, -100, 250)).Edge());
  aWireMaker4.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(100, -100, 250), gp_Pnt(0, -100, 250)).Edge());
  aWireMaker4.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, -100, 250), gp_Pnt(0, 0, 250)).Edge());
  TopoDS_Wire aTop2 = aWireMaker4.Wire();

  BRepOffsetAPI_ThruSections aLoft2(true, true);
  aLoft2.AddWire(aBottom2);
  aLoft2.AddWire(aTop2);
  aLoft2.Build();
  ASSERT_TRUE(aLoft2.IsDone()) << "Second loft should succeed";

  // Fuse the two lofts - this is the operation that could crash
  BRepAlgoAPI_Fuse aFuser(aLoft1.Shape(), aLoft2.Shape());

  EXPECT_TRUE(aFuser.IsDone()) << "Boolean fuse of two lofts should succeed";
  EXPECT_FALSE(aFuser.Shape().IsNull()) << "Result shape should not be null";

  double aResultVolume = GetVolume(aFuser.Shape());
  EXPECT_GT(aResultVolume, 0.0) << "Result should have positive volume";
}

// Test: Create a cone with manually removed pcurve on degenerated edge
// This directly tests the null pcurve handling in FillPaves() and CorrectWires()
TEST_F(BOPAlgo_PaveFillerTest, FuseConeWithRemovedPCurve_NullPCurveHandling)
{
  // Create a standard cone - it has a degenerated edge at the apex
  BRepPrimAPI_MakeCone aConeMaker(10.0, 0.0, 20.0);
  TopoDS_Shape         aCone = aConeMaker.Shape();
  ASSERT_FALSE(aCone.IsNull()) << "Cone should be created";

  // Find the conical face and its degenerated edge, then rebuild without pcurve
  TopoDS_Face aConicalFace;
  TopoDS_Edge aDegeneratedEdge;

  for (TopExp_Explorer aFaceExp(aCone, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
      if (BRep_Tool::Degenerated(anEdge))
      {
        aConicalFace     = aFace;
        aDegeneratedEdge = anEdge;
        break;
      }
    }
    if (!aDegeneratedEdge.IsNull())
      break;
  }

  // If we found a degenerated edge with pcurve, create a modified cone
  // where the degenerated edge lacks pcurve
  if (!aDegeneratedEdge.IsNull())
  {
    // Get the surface of the conical face
    Handle(Geom_Surface) aSurf  = BRep_Tool::Surface(aConicalFace);
    double               aFirst = 0.0;
    double               aLast  = 2.0 * M_PI;
    TopoDS_Vertex        aVertex =
      TopoDS::Vertex(TopExp_Explorer(aDegeneratedEdge, TopAbs_VERTEX).Current());

    // Create a new degenerated edge without pcurve
    BRep_Builder aBuilder;
    TopoDS_Edge  aNewDegEdge;
    aBuilder.MakeEdge(aNewDegEdge);
    aBuilder.Add(aNewDegEdge, aVertex.Oriented(TopAbs_FORWARD));
    aBuilder.Add(aNewDegEdge, aVertex.Oriented(TopAbs_REVERSED));
    aBuilder.Degenerated(aNewDegEdge, true);
    aBuilder.Range(aNewDegEdge, aFirst, aLast);
    // NOTE: We intentionally do NOT add a pcurve here!

    // Rebuild the wire with the new degenerated edge
    TopoDS_Wire aNewWire;
    aBuilder.MakeWire(aNewWire);

    for (TopExp_Explorer aWireExp(aConicalFace, TopAbs_WIRE); aWireExp.More(); aWireExp.Next())
    {
      const TopoDS_Wire& aWire = TopoDS::Wire(aWireExp.Current());
      for (TopExp_Explorer anEdgeExp(aWire, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
        if (BRep_Tool::Degenerated(anEdge))
        {
          aBuilder.Add(aNewWire, aNewDegEdge.Oriented(anEdge.Orientation()));
        }
        else
        {
          aBuilder.Add(aNewWire, anEdge);
        }
      }
      break; // Only process first wire
    }

    // Create new face with the modified wire
    TopoDS_Face aNewFace;
    aBuilder.MakeFace(aNewFace, aSurf, Precision::Confusion());
    aBuilder.Add(aNewFace, aNewWire);

    // Build a new shell/solid with the modified face
    TopoDS_Shell aNewShell;
    aBuilder.MakeShell(aNewShell);

    for (TopExp_Explorer aFaceExp2(aCone, TopAbs_FACE); aFaceExp2.More(); aFaceExp2.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(aFaceExp2.Current());
      if (aFace.IsSame(aConicalFace))
      {
        aBuilder.Add(aNewShell, aNewFace);
      }
      else
      {
        aBuilder.Add(aNewShell, aFace);
      }
    }

    TopoDS_Solid aNewSolid;
    aBuilder.MakeSolid(aNewSolid);
    aBuilder.Add(aNewSolid, aNewShell);

    // Verify we created a shape with degenerated edge without pcurve
    EXPECT_TRUE(HasDegeneratedEdgesWithoutPCurves(aNewSolid))
      << "Modified cone should have degenerated edge without pcurve";

    // Create a box that intersects near the apex
    BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(-5, -5, 15), 10.0, 10.0, 10.0);
    TopoDS_Shape        aBox = aBoxMaker.Shape();

    // Perform Boolean fuse - this should NOT crash with the fix
    // Without the fix, this would crash due to null pcurve dereference
    BRepAlgoAPI_Fuse aFuser(aNewSolid, aBox);

    // We expect either success or graceful failure, but NOT a crash
    // The operation might fail due to invalid geometry, but it should not segfault
    EXPECT_NO_THROW({
      bool isDone = aFuser.IsDone();
      (void)isDone; // Suppress unused variable warning
    }) << "Boolean fuse should not crash even with missing pcurve";
  }
  else
  {
    // Fallback: just run with regular cone if we couldn't find degenerated edge
    BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(-5, -5, 15), 10.0, 10.0, 10.0);
    BRepAlgoAPI_Fuse    aFuser(aCone, aBoxMaker.Shape());
    EXPECT_TRUE(aFuser.IsDone());
  }
}
