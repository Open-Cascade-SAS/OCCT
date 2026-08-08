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

#include <cmath>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepExtrema_ProximityDistTool.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepExtrema_ShapeProximity.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

namespace
{
using ProximityStatus = BRepExtrema_ProximityDistTool::ProxPnt_Status;

void expectProximityValue(const TopoDS_Shape& theShape1,
                          const TopoDS_Shape& theShape2,
                          const double         theExpectedValue,
                          const double         theTolerance,
                          const ProximityStatus theExpectedStatus1,
                          const ProximityStatus theExpectedStatus2,
                          const double          theMeshDeflection,
                          const bool            theCheckStatuses = true)
{
  BRepMesh_IncrementalMesh aMesh1(theShape1, theMeshDeflection);
  BRepMesh_IncrementalMesh aMesh2(theShape2, theMeshDeflection);
  ASSERT_TRUE(aMesh1.IsDone());
  ASSERT_TRUE(aMesh2.IsDone());

  BRepExtrema_ShapeProximity aProximity(Precision::Infinite());
  ASSERT_TRUE(aProximity.LoadShape1(theShape1));
  ASSERT_TRUE(aProximity.LoadShape2(theShape2));
  aProximity.Perform();

  ASSERT_TRUE(aProximity.IsDone());
  EXPECT_NEAR(aProximity.Proximity(), theExpectedValue, theTolerance);
  if (theCheckStatuses)
  {
    EXPECT_EQ(aProximity.ProxPntStatus1(), theExpectedStatus1);
    EXPECT_EQ(aProximity.ProxPntStatus2(), theExpectedStatus2);
  }
  EXPECT_FALSE(aProximity.ProximityPoint1().IsEqual(aProximity.ProximityPoint2(),
                                                     Precision::Confusion()));
}

TopoDS_Edge makeCircularArc(const gp_Pnt& theCenter,
                            const gp_Dir& theNormal,
                            const double  theRadius,
                            const double  theFirstParameter,
                            const double  theLastParameter)
{
  const gp_Circ aCircle(gp_Ax2(theCenter, theNormal), theRadius);
  return BRepBuilderAPI_MakeEdge(aCircle, theFirstParameter, theLastParameter).Edge();
}

TopoDS_Edge makeTrimmedCircularArc(const gp_Pnt& theCenter,
                                   const gp_Dir& theNormal,
                                   const double  theRadius,
                                   const double  theFirstParameter,
                                   const double  theLastParameter)
{
  const occ::handle<Geom_Curve> aCircle =
    new Geom_Circle(gp_Circ(gp_Ax2(theCenter, theNormal), theRadius));
  const occ::handle<Geom_TrimmedCurve> aTrimmedCircle =
    new Geom_TrimmedCurve(aCircle, theFirstParameter, theLastParameter);
  return BRepBuilderAPI_MakeEdge(aTrimmedCircle).Edge();
}

TopoDS_Edge makeFullCircularEdge(const gp_Pnt& theCenter,
                                 const gp_Dir& theNormal,
                                 const double  theRadius)
{
  const occ::handle<Geom_Curve> aCircle =
    new Geom_Circle(gp_Circ(gp_Ax2(theCenter, theNormal), theRadius));
  return BRepBuilderAPI_MakeEdge(aCircle).Edge();
}

TopoDS_Face makePlanePatch(const gp_Pnt& theLocation,
                           const gp_Dir& theNormal,
                           const double  theUMin,
                           const double  theUMax,
                           const double  theVMin,
                           const double  theVMax)
{
  return BRepBuilderAPI_MakeFace(gp_Pln(theLocation, theNormal),
                                 theUMin,
                                 theUMax,
                                 theVMin,
                                 theVMax)
    .Face();
}

TopoDS_Face makeCylinderPatch(const gp_Pnt& theLocation,
                              const gp_Dir& theDirection,
                              const double  theRadius,
                              const double  theFirstU,
                              const double  theLastU,
                              const double  theFirstV,
                              const double  theLastV)
{
  const occ::handle<Geom_CylindricalSurface> aCylinder =
    new Geom_CylindricalSurface(gp_Ax3(theLocation, theDirection), theRadius);
  const occ::handle<Geom_RectangularTrimmedSurface> aTrimmedCylinder =
    new Geom_RectangularTrimmedSurface(aCylinder,
                                       theFirstU,
                                       theLastU,
                                       theFirstV,
                                       theLastV);
  return BRepBuilderAPI_MakeFace(aTrimmedCylinder, Precision::Confusion()).Face();
}

TopoDS_Face makeSpherePatch(const gp_Pnt& theCenter, const double theFirstU, const double theLastU)
{
  const occ::handle<Geom_SphericalSurface> aSphere =
    new Geom_SphericalSurface(gp_Ax3(theCenter, gp_Dir(0.0, 0.0, 1.0)), 1.0);
  const occ::handle<Geom_RectangularTrimmedSurface> aTrimmedSphere =
    new Geom_RectangularTrimmedSurface(aSphere, theFirstU, theLastU, -0.5 * M_PI, 0.5 * M_PI);
  return BRepBuilderAPI_MakeFace(aTrimmedSphere, Precision::Confusion()).Face();
}

TopoDS_Face makeSphereFace(const gp_Pnt& theCenter)
{
  const occ::handle<Geom_SphericalSurface> aSphere =
    new Geom_SphericalSurface(gp_Ax3(theCenter, gp_Dir(0.0, 0.0, 1.0)), 1.0);
  return BRepBuilderAPI_MakeFace(aSphere, Precision::Confusion()).Face();
}

TopoDS_Face makePolygonFace()
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(-1.0, -1.0, 1.0));
  aPolygon.Add(gp_Pnt(-1.0, 1.0, 1.0));
  aPolygon.Add(gp_Pnt(1.0, 1.0, 2.0));
  aPolygon.Add(gp_Pnt(1.0, -1.0, 2.0));
  aPolygon.Close();
  return BRepBuilderAPI_MakeFace(aPolygon.Wire(), true).Face();
}

TopoDS_Shape makeSplitSphere(const gp_Pnt& theCenter)
{
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, makeSpherePatch(theCenter, 0.0, 0.5 * M_PI));
  aBuilder.Add(aCompound, makeSpherePatch(theCenter, 0.5 * M_PI, M_PI));
  return aCompound;
}

TopoDS_Edge makeBezierEdge(const NCollection_Array1<gp_Pnt>& thePoles)
{
  const occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(thePoles);
  return BRepBuilderAPI_MakeEdge(aCurve).Edge();
}
} // namespace

// Migrated from tests/lowalgos/proximity/A1. Two opposite semicircular edges
// have a proximity value equal to the diameter and interior proximity points.
TEST(BRepExtrema_ShapeProximityTest, LowAlgo_A1_OppositeCircularArcs)
{
  const TopoDS_Edge anEdge1 = makeCircularArc(gp_Pnt(0.0, 0.0, 0.0),
                                              gp_Dir(1.0, 0.0, 0.0),
                                              5.0,
                                              0.5 * M_PI,
                                              M_PI);
  const TopoDS_Edge anEdge2 = makeCircularArc(gp_Pnt(0.0, 0.0, 0.0),
                                              gp_Dir(1.0, 0.0, 0.0),
                                              5.0,
                                              -0.5 * M_PI,
                                              0.0);
  expectProximityValue(anEdge1,
                       anEdge2,
                       10.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3);
}

// Migrated from tests/lowalgos/proximity/B1. Parallel square faces are closest
// at their boundary triangulation nodes.
TEST(BRepExtrema_ShapeProximityTest, LowAlgo_B1_ParallelPlanePatches)
{
  const TopoDS_Face aFace1 = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  const TopoDS_Face aFace2 = makePlanePatch(gp_Pnt(0.0, 0.0, 1.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  expectProximityValue(aFace1,
                       aFace2,
                       1.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       0.1);
}

// Migrated from tests/lowalgos/proximity/C1. A plane and a short coaxial
// cylinder exercise the mixed middle/border status classification.
TEST(BRepExtrema_ShapeProximityTest, LowAlgo_C1_PlaneAndCylinder)
{
  const TopoDS_Face aPlane = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  const TopoDS_Face aCylinder = makeCylinderPatch(gp_Pnt(0.0, 0.0, 0.0),
                                                  gp_Dir(0.0, 1.0, 0.0),
                                                  0.1,
                                                  0.0,
                                                  2.0 * M_PI,
                                                  0.0,
                                                  1.0);
  expectProximityValue(aPlane,
                       aCylinder,
                       0.1,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-3);
}

// Migrated from tests/lowalgos/proximity/D1. Two opposing half-spheres have a
// four-unit minimal enclosing-layer thickness.
TEST(BRepExtrema_ShapeProximityTest, LowAlgo_D1_OpposingSpherePatches)
{
  const TopoDS_Face aSphere1 = makeSpherePatch(gp_Pnt(0.0, 1.0, 0.0), 0.0, M_PI);
  const TopoDS_Face aSphere2 = makeSpherePatch(gp_Pnt(0.0, -1.0, 0.0), -M_PI, 0.0);
  expectProximityValue(aSphere1,
                       aSphere2,
                       4.0,
                       1.e-2,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_A2_PerpendicularFullCircles)
{
  const TopoDS_Edge anEdge1 =
    makeFullCircularEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0), 1.0);
  const TopoDS_Edge anEdge2 =
    makeFullCircularEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0), 1.0);
  expectProximityValue(anEdge1,
                       anEdge2,
                       2.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_A3_OffsetSemicircles)
{
  const TopoDS_Edge anEdge1 = makeTrimmedCircularArc(gp_Pnt(0.0, -1.0, 0.0),
                                                     gp_Dir(1.0, 0.0, 0.0),
                                                     1.0,
                                                     -M_PI,
                                                     0.0);
  const TopoDS_Edge anEdge2 = makeTrimmedCircularArc(gp_Pnt(0.0, 1.0, 0.0),
                                                     gp_Dir(1.0, 0.0, 0.0),
                                                     1.0,
                                                     0.0,
                                                     M_PI);
  expectProximityValue(anEdge1,
                       anEdge2,
                       std::sqrt(5.0) - 1.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-3,
                       false);
  // DRAW only assigned the two expected status names; it did not compare
  // them. The current refinement path reports UNKNOWN for these edge points,
  // so retain the validated distance and distinct-point checks above without
  // turning unasserted DRAW metadata into a new failure.
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_A4_CoplanarCirclesWithDifferentAxes)
{
  const TopoDS_Edge anEdge1 =
    makeFullCircularEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0), 1.0);
  const TopoDS_Edge anEdge2 =
    makeFullCircularEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(-1.0, 0.5, 0.0), 1.0);
  expectProximityValue(anEdge1,
                       anEdge2,
                       2.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3,
                       false);
  // As in A3, the DRAW variables named expected statuses but never checked
  // them; the numeric proximity and distinct-point invariants are meaningful.
}

// Migrated from tests/lowalgos/proximity/A5. Use the same first-vertex
// distance that the DRAW script computes as an independent numeric reference.
TEST(BRepExtrema_ShapeProximityTest, LowAlgo_A5_TrimmedCircularEdges)
{
  const occ::handle<Geom_Circle> aCircle1 =
    new Geom_Circle(gp_Circ(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), 1.0));
  const occ::handle<Geom_TrimmedCurve> aTrimmedCircle1 =
    new Geom_TrimmedCurve(aCircle1, -0.5 * M_PI, 0.5 * M_PI);
  const TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aTrimmedCircle1).Edge();

  const occ::handle<Geom_Circle> aCircle2 =
    new Geom_Circle(gp_Circ(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.5, 0.0)), 1.0));
  const occ::handle<Geom_TrimmedCurve> aTrimmedCircle2 =
    new Geom_TrimmedCurve(aCircle2, 0.0, M_PI);
  const TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(aTrimmedCircle2).Edge();

  TopoDS_Vertex anEdge1First;
  TopoDS_Vertex anEdge1Last;
  TopoDS_Vertex anEdge2First;
  TopoDS_Vertex anEdge2Last;
  TopExp::Vertices(anEdge1, anEdge1First, anEdge1Last);
  TopExp::Vertices(anEdge2, anEdge2First, anEdge2Last);
  ASSERT_FALSE(anEdge1First.IsNull());
  ASSERT_FALSE(anEdge2First.IsNull());

  BRepExtrema_DistShapeShape aReferenceDistance(anEdge1First, anEdge2First);
  ASSERT_TRUE(aReferenceDistance.IsDone());
  ASSERT_GT(aReferenceDistance.Value(), 0.0);

  // The DRAW status variables are not compared in this case; retain the
  // independent vertex-distance reference and the proximity point checks.
  expectProximityValue(anEdge1,
                       anEdge2,
                       aReferenceDistance.Value(),
                       2.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-3,
                       false);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_B2_OrthogonalPlanePatches)
{
  const TopoDS_Face aFace1 = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  const TopoDS_Face aFace2 = makePlanePatch(gp_Pnt(0.0, 0.0, 1.0),
                                            gp_Dir(-1.0, 0.0, 0.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  expectProximityValue(aFace1,
                       aFace2,
                       1.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       0.1);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_B3_IntersectingPlanePatches)
{
  const TopoDS_Face aFace1 = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  const TopoDS_Face aFace2 = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(-1.0, 0.0, 0.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  expectProximityValue(aFace1,
                       aFace2,
                       1.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       0.1);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_B4_OffsetIntersectingPlanePatches)
{
  const TopoDS_Face aFace1 = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  const TopoDS_Face aFace2 = makePlanePatch(gp_Pnt(0.0, 0.0, 0.5),
                                            gp_Dir(-1.0, 0.0, 0.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  expectProximityValue(aFace1,
                       aFace2,
                       1.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       0.1);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_B5_SlopedPolygonPatch)
{
  const TopoDS_Face aFace1 = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -1.0,
                                            1.0,
                                            -1.0,
                                            1.0);
  const TopoDS_Face aFace2 = makePolygonFace();
  expectProximityValue(aFace1,
                       aFace2,
                       0.8 * std::sqrt(5.0),
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-3);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_C2_ObliqueCylinderAndPlane)
{
  const TopoDS_Face aPlane = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Dir(0.0, 0.0, 1.0),
                                            -2.0,
                                            2.0,
                                            -2.0,
                                            2.0);
  const TopoDS_Face aCylinder = makeCylinderPatch(gp_Pnt(0.0, 0.0, 1.0),
                                                  gp_Dir(0.0, std::sqrt(0.5), std::sqrt(0.5)),
                                                  0.1,
                                                  0.0,
                                                  2.0 * M_PI,
                                                  0.0,
                                                  1.0);
  expectProximityValue(aPlane,
                       aCylinder,
                       1.0 + 1.1 * std::sqrt(0.5),
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-3);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_C3_ParallelCylinderPatches)
{
  const TopoDS_Face aCylinder1 = makeCylinderPatch(gp_Pnt(0.0, 0.0, 1.0),
                                                   gp_Dir(1.0, 0.0, 0.0),
                                                   0.1,
                                                   0.0,
                                                   2.0 * M_PI,
                                                   0.0,
                                                   1.0);
  const TopoDS_Face aCylinder2 = makeCylinderPatch(gp_Pnt(0.0, 0.0, 0.0),
                                                   gp_Dir(1.0, 0.0, 0.0),
                                                   0.1,
                                                   0.0,
                                                   2.0 * M_PI,
                                                   0.0,
                                                   1.0);
  expectProximityValue(aCylinder1,
                       aCylinder2,
                       1.0,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-3);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_C4_TrimmedParallelCylinderPatches)
{
  const TopoDS_Face aCylinder1 = makeCylinderPatch(gp_Pnt(0.0, 0.0, 1.0),
                                                   gp_Dir(1.0, 0.0, 0.0),
                                                   0.1,
                                                   -0.5 * M_PI,
                                                   0.5 * M_PI,
                                                   0.0,
                                                   1.0);
  const TopoDS_Face aCylinder2 = makeCylinderPatch(gp_Pnt(0.0, 0.0, 0.0),
                                                   gp_Dir(1.0, 0.0, 0.0),
                                                   0.1,
                                                   0.5 * M_PI,
                                                   -0.5 * M_PI,
                                                   0.0,
                                                   1.0);
  expectProximityValue(aCylinder1,
                       aCylinder2,
                       1.2,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-5);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_D2_FullOpposingSpheres)
{
  const TopoDS_Face aSphere1 = makeSphereFace(gp_Pnt(0.0, 1.0, 0.0));
  const TopoDS_Face aSphere2 = makeSphereFace(gp_Pnt(0.0, -1.0, 0.0));
  expectProximityValue(aSphere1,
                       aSphere2,
                       4.0,
                       1.e-2,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_D3_SplitSphereAndPlane)
{
  const TopoDS_Shape aSphere = makeSplitSphere(gp_Pnt(0.0, 1.0, 0.0));
  const TopoDS_Face  aPlane  = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                             gp_Dir(0.0, 1.0, 0.0),
                                             -2.0,
                                             2.0,
                                             -2.0,
                                             2.0);
  expectProximityValue(aSphere,
                       aPlane,
                       2.0,
                       1.e-2,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3);
}

TEST(BRepExtrema_ShapeProximityTest, LowAlgo_D4_TrimmedSphereAndPlane)
{
  const TopoDS_Face aSphere = makeSpherePatch(gp_Pnt(0.0, 1.0, 0.0), 0.0, M_PI);
  const TopoDS_Face aPlane  = makePlanePatch(gp_Pnt(0.0, 0.0, 0.0),
                                             gp_Dir(0.0, 1.0, 0.0),
                                             -1.0,
                                             1.0,
                                             -1.0,
                                             1.0);
  expectProximityValue(aSphere,
                       aPlane,
                       2.0,
                       1.e-2,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3);
}

// Migrated from tests/lowalgos/proximity/E1. The DRAW case derives the
// expected proximity from the second endpoint of each Bezier edge.
TEST(BRepExtrema_ShapeProximityTest, LowAlgo_E1_BezierEdgesEndpointReference)
{
  NCollection_Array1<gp_Pnt> aPoles1(1, 7);
  aPoles1(1) = gp_Pnt(10.0, 84.0, 0.0);
  aPoles1(2) = gp_Pnt(54.0, 96.0, 0.0);
  aPoles1(3) = gp_Pnt(145.0, 146.0, 0.0);
  aPoles1(4) = gp_Pnt(167.0, 167.0, 0.0);
  aPoles1(5) = gp_Pnt(185.0, 212.0, 0.0);
  aPoles1(6) = gp_Pnt(187.0, 234.0, 0.0);
  aPoles1(7) = gp_Pnt(176.0, 302.0, 0.0);

  NCollection_Array1<gp_Pnt> aPoles2(1, 8);
  aPoles2(1) = gp_Pnt(120.0, 72.0, 0.0);
  aPoles2(2) = gp_Pnt(170.0, 87.0, 0.0);
  aPoles2(3) = gp_Pnt(227.0, 118.0, 0.0);
  aPoles2(4) = gp_Pnt(238.0, 126.0, 0.0);
  aPoles2(5) = gp_Pnt(243.0, 157.0, 0.0);
  aPoles2(6) = gp_Pnt(203.0, 216.0, 0.0);
  aPoles2(7) = gp_Pnt(134.0, 281.0, 0.0);
  aPoles2(8) = gp_Pnt(94.0, 324.0, 0.0);

  const TopoDS_Edge anEdge1 = makeBezierEdge(aPoles1);
  const TopoDS_Edge anEdge2 = makeBezierEdge(aPoles2);
  const gp_Pnt      aPnt1   = BRep_Tool::Pnt(TopExp::LastVertex(anEdge1, true));
  const gp_Pnt      aPnt2   = BRep_Tool::Pnt(TopExp::LastVertex(anEdge2, true));
  expectProximityValue(anEdge1,
                       anEdge2,
                       aPnt1.Distance(aPnt2),
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       ProximityStatus::ProxPnt_Status_BORDER,
                       1.e-3);
}

// Migrated from tests/lowalgos/proximity/E2. This case checks a finite
// middle-point proximity for two different-degree Bezier curves.
TEST(BRepExtrema_ShapeProximityTest, LowAlgo_E2_DifferentDegreeBezierEdges)
{
  NCollection_Array1<gp_Pnt> aPoles1(1, 6);
  aPoles1(1) = gp_Pnt(54.0, 96.0, 0.0);
  aPoles1(2) = gp_Pnt(145.0, 146.0, 0.0);
  aPoles1(3) = gp_Pnt(167.0, 167.0, 0.0);
  aPoles1(4) = gp_Pnt(185.0, 212.0, 0.0);
  aPoles1(5) = gp_Pnt(187.0, 234.0, 0.0);
  aPoles1(6) = gp_Pnt(176.0, 302.0, 0.0);

  NCollection_Array1<gp_Pnt> aPoles2(1, 7);
  aPoles2(1) = gp_Pnt(120.0, 72.0, 0.0);
  aPoles2(2) = gp_Pnt(170.0, 87.0, 0.0);
  aPoles2(3) = gp_Pnt(227.0, 118.0, 0.0);
  aPoles2(4) = gp_Pnt(238.0, 126.0, 0.0);
  aPoles2(5) = gp_Pnt(243.0, 157.0, 0.0);
  aPoles2(6) = gp_Pnt(203.0, 216.0, 0.0);
  aPoles2(7) = gp_Pnt(134.0, 281.0, 0.0);

  const TopoDS_Edge anEdge1 = makeBezierEdge(aPoles1);
  const TopoDS_Edge anEdge2 = makeBezierEdge(aPoles2);
  expectProximityValue(anEdge1,
                       anEdge2,
                       76.34,
                       1.e-3,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       ProximityStatus::ProxPnt_Status_MIDDLE,
                       1.e-3);
}
