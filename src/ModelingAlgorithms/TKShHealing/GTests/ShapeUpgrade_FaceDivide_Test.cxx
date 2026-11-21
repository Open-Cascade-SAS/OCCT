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

#include <gtest/gtest.h>

#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_IndexedMap.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <ShapeFix.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>
#include <ShapeUpgrade_ShapeDivideClosed.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <atomic>

// Test for issue #1179: ShapeUpgrade_FaceDivide crashes with null context.
TEST(ShapeUpgrade_FaceDivideTest, Perform_WithoutContext_DoesNotCrash)
{
  for (int i = 0; i < 10; ++i)
  {
    const double       aSize = 10.0 + i * 0.5;
    const TopoDS_Shape aBox  = BRepPrimAPI_MakeBox(aSize, aSize + 5, aSize + 10).Shape();
    TopExp_Explorer    anExp(aBox, TopAbs_FACE);
    ASSERT_TRUE(anExp.More()) << "No faces found in box";

    ShapeUpgrade_FaceDivide aDivider(TopoDS::Face(anExp.Current()));
    EXPECT_NO_THROW(aDivider.Perform());
  }
}

// Test for issue #1179: ShapeUpgrade_FaceDivide is safe to call concurrently on independent faces.
TEST(ShapeUpgrade_FaceDivideTest, Perform_Parallel_DoesNotCrash)
{
  std::atomic<bool> anError{false};
  OSD_Parallel::For(0, 100, [&](int theIndex) {
    const double       aSize = 10.0 + theIndex * 0.1;
    const TopoDS_Shape aBox  = BRepPrimAPI_MakeBox(aSize, aSize + 5, aSize + 10).Shape();
    TopExp_Explorer    anExp(aBox, TopAbs_FACE);
    if (!anExp.More())
    {
      anError.store(true, std::memory_order_relaxed);
      return;
    }
    try
    {
      ShapeUpgrade_FaceDivide aDivider(TopoDS::Face(anExp.Current()));
      aDivider.Perform();
    }
    catch (...)
    {
      anError.store(true, std::memory_order_relaxed);
    }
  });

  EXPECT_FALSE(anError.load()) << "FaceDivide failed in parallel execution";
}

TEST(ShapeUpgrade_FaceDivideTest, SplitCylinderRegularityMatchesSecondDerivatives)
{
  BRepPrimAPI_MakeCylinder    aCylinder(2.0, 10.0);
  BRepBuilderAPI_NurbsConvert aConvert(aCylinder.Face());
  ASSERT_TRUE(aConvert.IsDone());
  ShapeUpgrade_ShapeDivideClosed aDivider(aConvert.Shape());
  ASSERT_TRUE(aDivider.Perform());
  const TopoDS_Shape aResult = aDivider.Result();
  ShapeFix::SameParameter(aResult, false);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaces;
  TopExp::MapShapes(aResult, TopAbs_FACE, aFaces);
  ASSERT_EQ(aFaces.Extent(), 2);
  const TopoDS_Face                                             aFace1 = TopoDS::Face(aFaces(1));
  const TopoDS_Face                                             aFace2 = TopoDS::Face(aFaces(2));
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(aFace1, TopAbs_EDGE, anEdges);
  ASSERT_EQ(anEdges.Extent(), 4);
  const occ::handle<Geom_Surface> aSurface1 = BRep_Tool::Surface(aFace1);
  const occ::handle<Geom_Surface> aSurface2 = BRep_Tool::Surface(aFace2);
  for (const int anIndex : {2, 4})
  {
    SCOPED_TRACE(anIndex);
    const TopoDS_Edge               anEdge = TopoDS::Edge(anEdges(anIndex));
    double                          aFirst1, aLast1, aFirst2, aLast2;
    const occ::handle<Geom2d_Curve> aCurve1 =
      BRep_Tool::CurveOnSurface(anEdge, aFace1, aFirst1, aLast1);
    const occ::handle<Geom2d_Curve> aCurve2 =
      BRep_Tool::CurveOnSurface(anEdge, aFace2, aFirst2, aLast2);
    ASSERT_FALSE(aCurve1.IsNull());
    ASSERT_FALSE(aCurve2.IsNull());
    const gp_Pnt2d            aUV1              = aCurve1->EvalD0((aFirst1 + aLast1) * 0.5);
    const gp_Pnt2d            aUV2              = aCurve2->EvalD0((aFirst2 + aLast2) * 0.5);
    const Geom_Surface::ResD2 aJet1             = aSurface1->EvalD2(aUV1.X(), aUV1.Y());
    const Geom_Surface::ResD2 aJet2             = aSurface2->EvalD2(aUV2.X(), aUV2.Y());
    const double              aSecondDifference = (aJet1.D2U - aJet2.D2U).Magnitude();
    const GeomAbs_Shape       aContinuity =
      BRepLib::ContinuityOfFaces(anEdge, aFace1, aFace2, Precision::Angular());
    EXPECT_EQ(BRepLib::ContinuityOfFaces(anEdge, aFace2, aFace1, Precision::Angular()),
              aContinuity);
    EXPECT_EQ(BRepLib::ContinuityOfFaces(TopoDS::Edge(anEdge.Reversed()),
                                         aFace1,
                                         aFace2,
                                         Precision::Angular()),
              aContinuity);
    EXPECT_EQ(BRepLib::ContinuityOfFaces(anEdge,
                                         TopoDS::Face(aFace1.Reversed()),
                                         TopoDS::Face(aFace2.Reversed()),
                                         Precision::Angular()),
              aContinuity);
    gp_Trsf aTransform;
    aTransform.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 2.0, 3.0)), 0.73);
    aTransform.SetTranslationPart(gp_Vec(100.0, -200.0, 300.0));
    const TopLoc_Location aLocation(aTransform);
    EXPECT_EQ(BRepLib::ContinuityOfFaces(TopoDS::Edge(anEdge.Moved(aLocation)),
                                         TopoDS::Face(aFace1.Moved(aLocation)),
                                         TopoDS::Face(aFace2.Moved(aLocation)),
                                         Precision::Angular()),
              aContinuity);
    if (anIndex == 2)
    {
      EXPECT_GT(aSecondDifference, Precision::Confusion());
      EXPECT_EQ(aContinuity, GeomAbs_G2);
    }
    else
    {
      EXPECT_LT(aSecondDifference, Precision::Confusion());
      EXPECT_EQ(aContinuity, GeomAbs_C2);
    }
  }
}
