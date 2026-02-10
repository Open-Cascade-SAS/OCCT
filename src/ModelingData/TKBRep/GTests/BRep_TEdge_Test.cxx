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
#include <BRep_Curve3D.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_TEdge.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Line.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Polygon3D.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

TEST(BRep_TEdge_Test, DefaultConstruction_CacheIsNull)
{
  Handle(BRep_TEdge) aTEdge = new BRep_TEdge();
  EXPECT_TRUE(aTEdge->Curve3D().IsNull()) << "New TEdge should have null Curve3D cache";
  EXPECT_TRUE(aTEdge->Polygon3D().IsNull()) << "New TEdge should have null Polygon3D cache";
}

TEST(BRep_TEdge_Test, Curve3D_SetAndGet)
{
  Handle(BRep_TEdge)  aTEdge = new BRep_TEdge();
  Handle(Geom_Line)   aLine  = new Geom_Line(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  Handle(BRep_Curve3D) aCurve3D = new BRep_Curve3D(aLine, TopLoc_Location());
  aTEdge->Curve3D(aCurve3D);
  EXPECT_FALSE(aTEdge->Curve3D().IsNull()) << "Curve3D cache should be set";
  EXPECT_TRUE(aTEdge->Curve3D() == aCurve3D) << "Curve3D cache should match what was set";
}

TEST(BRep_TEdge_Test, Polygon3D_SetAndGet)
{
  Handle(BRep_TEdge) aTEdge = new BRep_TEdge();
  NCollection_Array1<gp_Pnt> aNodes(1, 2);
  aNodes.SetValue(1, gp_Pnt(0., 0., 0.));
  aNodes.SetValue(2, gp_Pnt(1., 0., 0.));
  Handle(Poly_Polygon3D) aPoly = new Poly_Polygon3D(aNodes);
  Handle(BRep_Polygon3D) aPolyRep = new BRep_Polygon3D(aPoly, TopLoc_Location());
  aTEdge->Polygon3D(aPolyRep);
  EXPECT_FALSE(aTEdge->Polygon3D().IsNull()) << "Polygon3D cache should be set";
  EXPECT_TRUE(aTEdge->Polygon3D() == aPolyRep) << "Polygon3D cache should match what was set";
}

TEST(BRep_TEdge_Test, ChangeCurves_InvalidatesCache)
{
  Handle(BRep_TEdge)   aTEdge   = new BRep_TEdge();
  Handle(Geom_Line)    aLine    = new Geom_Line(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  Handle(BRep_Curve3D) aCurve3D = new BRep_Curve3D(aLine, TopLoc_Location());
  aTEdge->Curve3D(aCurve3D);

  NCollection_Array1<gp_Pnt> aNodes(1, 2);
  aNodes.SetValue(1, gp_Pnt(0., 0., 0.));
  aNodes.SetValue(2, gp_Pnt(1., 0., 0.));
  Handle(Poly_Polygon3D) aPoly    = new Poly_Polygon3D(aNodes);
  Handle(BRep_Polygon3D) aPolyRep = new BRep_Polygon3D(aPoly, TopLoc_Location());
  aTEdge->Polygon3D(aPolyRep);

  EXPECT_FALSE(aTEdge->Curve3D().IsNull()) << "Curve3D cache should be set before ChangeCurves";
  EXPECT_FALSE(aTEdge->Polygon3D().IsNull()) << "Polygon3D cache should be set before ChangeCurves";

  // ChangeCurves() should invalidate both caches
  aTEdge->ChangeCurves();
  EXPECT_TRUE(aTEdge->Curve3D().IsNull()) << "Curve3D cache should be null after ChangeCurves";
  EXPECT_TRUE(aTEdge->Polygon3D().IsNull()) << "Polygon3D cache should be null after ChangeCurves";
}

TEST(BRep_TEdge_Test, EmptyCopy_PreservesCurve3DCache)
{
  // Build a TEdge with a Curve3D cache set explicitly
  Handle(BRep_TEdge)   aTEdge   = new BRep_TEdge();
  Handle(Geom_Line)    aLine    = new Geom_Line(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  Handle(BRep_Curve3D) aCurve3D = new BRep_Curve3D(aLine, TopLoc_Location());
  aTEdge->Curve3D(aCurve3D);
  aTEdge->ChangeCurves().Append(aCurve3D);
  // Re-set cache after ChangeCurves invalidated it
  aTEdge->Curve3D(aCurve3D);
  ASSERT_FALSE(aTEdge->Curve3D().IsNull()) << "Original should have a Curve3D cache";

  Handle(TopoDS_TShape) aCopy = aTEdge->EmptyCopy();
  const Handle(BRep_TEdge)& aCopyEdge = Handle(BRep_TEdge)::DownCast(aCopy);
  ASSERT_FALSE(aCopyEdge.IsNull()) << "EmptyCopy should return BRep_TEdge";
  EXPECT_FALSE(aCopyEdge->Curve3D().IsNull()) << "EmptyCopy should preserve Curve3D cache";
}

TEST(BRep_TEdge_Test, EmptyCopy_PreservesTolerance)
{
  Handle(BRep_TEdge) aTEdge = new BRep_TEdge();
  aTEdge->Tolerance(0.25);
  Handle(TopoDS_TShape)     aCopy     = aTEdge->EmptyCopy();
  const Handle(BRep_TEdge)& aCopyEdge = Handle(BRep_TEdge)::DownCast(aCopy);
  ASSERT_FALSE(aCopyEdge.IsNull());
  EXPECT_NEAR(aCopyEdge->Tolerance(), 0.25, 1e-15)
    << "EmptyCopy should preserve tolerance";
}

TEST(BRep_TEdge_Test, EmptyCopy_PreservesFlags)
{
  Handle(BRep_TEdge) aTEdge = new BRep_TEdge();
  aTEdge->Degenerated(true);
  aTEdge->SameParameter(false);
  aTEdge->SameRange(false);

  Handle(TopoDS_TShape)     aCopy     = aTEdge->EmptyCopy();
  const Handle(BRep_TEdge)& aCopyEdge = Handle(BRep_TEdge)::DownCast(aCopy);
  ASSERT_FALSE(aCopyEdge.IsNull());
  EXPECT_TRUE(aCopyEdge->Degenerated()) << "EmptyCopy should preserve Degenerated flag";
  EXPECT_FALSE(aCopyEdge->SameParameter()) << "EmptyCopy should preserve SameParameter flag";
  EXPECT_FALSE(aCopyEdge->SameRange()) << "EmptyCopy should preserve SameRange flag";
}

TEST(BRep_TEdge_Test, Tolerance_ClampedOnSet)
{
  Handle(BRep_TEdge) aTEdge = new BRep_TEdge();
  aTEdge->Tolerance(1e-20);
  EXPECT_GE(aTEdge->Tolerance(), Precision::Confusion())
    << "Tolerance should be clamped to Precision::Confusion()";
}

TEST(BRep_TEdge_Test, UpdateTolerance_OnlyIncreases)
{
  Handle(BRep_TEdge) aTEdge = new BRep_TEdge();
  aTEdge->Tolerance(1.0);
  const double aOrigTol = aTEdge->Tolerance();

  // Try to decrease via UpdateTolerance — should not change
  aTEdge->UpdateTolerance(0.5);
  EXPECT_NEAR(aTEdge->Tolerance(), aOrigTol, 1e-15)
    << "UpdateTolerance should not decrease tolerance";

  // Increase via UpdateTolerance — should update
  aTEdge->UpdateTolerance(2.0);
  EXPECT_NEAR(aTEdge->Tolerance(), 2.0, 1e-15)
    << "UpdateTolerance should increase tolerance";
}
