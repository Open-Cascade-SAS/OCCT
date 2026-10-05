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

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepGProp.hxx>
#include <BRepGProp_Domain.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp_Sinert.hxx>
#include <BRepGProp_Vinert.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Builder.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Mat.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <math.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <Standard_Handle.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
void expectGPropsNear(const GProp_GProps& theActual,
                      const GProp_GProps& theExpected,
                      const double        theTolerance)
{
  EXPECT_NEAR(theActual.Mass(), theExpected.Mass(), theTolerance);
  EXPECT_NEAR(theActual.CentreOfMass().X(), theExpected.CentreOfMass().X(), theTolerance);
  EXPECT_NEAR(theActual.CentreOfMass().Y(), theExpected.CentreOfMass().Y(), theTolerance);
  EXPECT_NEAR(theActual.CentreOfMass().Z(), theExpected.CentreOfMass().Z(), theTolerance);

  const gp_Mat anActualInertia   = theActual.MatrixOfInertia();
  const gp_Mat anExpectedInertia = theExpected.MatrixOfInertia();
  for (int aRow = 1; aRow <= 3; ++aRow)
  {
    for (int aColumn = 1; aColumn <= 3; ++aColumn)
    {
      EXPECT_NEAR(anActualInertia.Value(aRow, aColumn),
                  anExpectedInertia.Value(aRow, aColumn),
                  theTolerance);
    }
  }
}
} // namespace

TEST(BRepGPropTest, LinearProperties_EdgeLength)
{
  gp_Pnt aP1(0.0, 0.0, 0.0);
  gp_Pnt aP2(3.0, 4.0, 0.0);

  BRepBuilderAPI_MakeEdge aMakeEdge(aP1, aP2);
  ASSERT_TRUE(aMakeEdge.IsDone());

  GProp_GProps aProps;
  BRepGProp::LinearProperties(aMakeEdge.Edge(), aProps);
  EXPECT_NEAR(aProps.Mass(), 5.0, Precision::Confusion()) << "Edge length should be 5";
}

TEST(BRepGPropTest, SurfaceProperties_BoxFaceArea)
{
  BRepPrimAPI_MakeBox aBox(10.0, 20.0, 30.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  // Total surface area = 2*(10*20 + 10*30 + 20*30) = 2*(200+300+600) = 2200
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), 2200.0, Precision::Confusion());
}

TEST(BRepGPropTest, VolumeProperties_UnitBox)
{
  BRepPrimAPI_MakeBox aBox(1.0, 1.0, 1.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), 1.0, Precision::Confusion()) << "Unit box volume should be 1";
}

TEST(BRepGPropTest, VolumeProperties_Sphere)
{
  const double           aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphere(aRadius);
  TopoDS_Shape           aShape = aSphere.Shape();
  ASSERT_TRUE(aSphere.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  double anExpectedVolume = (4.0 / 3.0) * M_PI * aRadius * aRadius * aRadius;
  EXPECT_NEAR(aProps.Mass(), anExpectedVolume, 0.01) << "Sphere volume mismatch";
}

TEST(BRepGPropTest, VolumeProperties_BoxCenterOfMass)
{
  // Box from (0,0,0) to (10,10,10)
  BRepPrimAPI_MakeBox aBox(10.0, 10.0, 10.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  gp_Pnt aCOM = aProps.CentreOfMass();

  // Center of mass should be at (5, 5, 5)
  EXPECT_NEAR(aCOM.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Z(), 5.0, Precision::Confusion());
}

TEST(BRepGPropTest, AdaptiveSurfaceProperties_Box)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aBox, aProps, 1.0e-6);

  EXPECT_NEAR(aProps.Mass(), 2200.0, 1.0e-9);
}

TEST(BRepGPropTest, AdaptiveVolumeProperties_Box)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aBox, aProps, 1.0e-6);

  EXPECT_NEAR(aProps.Mass(), 6000.0, 1.0e-9);
}

TEST(BRepGPropTest, AdaptiveNaturalBSplineSurface_ResizesIntervalArrays)
{
  const int                  aNbUIntervals = math::GaussPointsMax() + 1;
  const int                  aNbUPoles     = aNbUIntervals + 1;
  NCollection_Array2<gp_Pnt> aPoles(1, aNbUPoles, 1, 2);
  for (int aUIndex = 1; aUIndex <= aNbUPoles; ++aUIndex)
  {
    aPoles(aUIndex, 1) = gp_Pnt(static_cast<double>(aUIndex - 1), 0.0, 0.0);
    aPoles(aUIndex, 2) = gp_Pnt(static_cast<double>(aUIndex - 1), 2.0, 0.0);
  }

  NCollection_Array1<double> aUKnots(1, aNbUPoles);
  NCollection_Array1<int>    aUMultiplicities(1, aNbUPoles);
  for (int anIndex = 1; anIndex <= aNbUPoles; ++anIndex)
  {
    aUKnots(anIndex)          = static_cast<double>(anIndex - 1);
    aUMultiplicities(anIndex) = anIndex == 1 || anIndex == aNbUPoles ? 2 : 1;
  }

  NCollection_Array1<double> aVKnots(1, 2);
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;
  NCollection_Array1<int> aVMultiplicities(1, 2);
  aVMultiplicities.Init(2);

  occ::handle<Geom_BSplineSurface> aSurface =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMultiplicities, aVMultiplicities, 1, 1);
  TopoDS_Face aFace;
  BRep_Builder().MakeFace(aFace, aSurface, Precision::Confusion());
  ASSERT_EQ(aFace.NbChildren(), 0);

  BRepGProp_Face aFixedFace(aFace);
  EXPECT_EQ(aFixedFace.SUIntSubs(), aNbUIntervals);
  BRepGProp_Sinert aFixed(aFixedFace, gp_Pnt(0.0, 0.0, 0.0));
  BRepGProp_Face   anAdaptiveFace(aFace);
  BRepGProp_Sinert anAdaptive(anAdaptiveFace, gp_Pnt(0.0, 0.0, 0.0), 1.0e-8);

  EXPECT_NEAR(anAdaptive.Mass(), 2.0 * aNbUIntervals, 1.0e-10);
  EXPECT_NEAR(anAdaptive.CentreOfMass().X(), 0.5 * aNbUIntervals, 1.0e-10);
  EXPECT_NEAR(anAdaptive.CentreOfMass().Y(), 1.0, 1.0e-12);
  EXPECT_NEAR(anAdaptive.CentreOfMass().Z(), 0.0, 1.0e-12);
  expectGPropsNear(anAdaptive, aFixed, 1.0e-9);
}

TEST(BRepGPropTest, AdaptiveTrimmedQuarterDisk_ReusesBoundaryBuffers)
{
  const double aRadius = 4.0;
  const gp_Pnt anOrigin(0.0, 0.0, 0.0);
  const gp_Pnt anArcStart(aRadius, 0.0, 0.0);
  const gp_Pnt anArcEnd(0.0, aRadius, 0.0);

  const occ::handle<Geom_TrimmedCurve> anArc =
    GC_MakeArcOfCircle(gp_Circ(gp_Ax2(anOrigin, gp_Dir(0.0, 0.0, 1.0)), aRadius), 0.0, M_PI_2, true)
      .Value();
  BRepBuilderAPI_MakeWire aWireBuilder;
  aWireBuilder.Add(BRepBuilderAPI_MakeEdge(anArc).Edge());
  aWireBuilder.Add(BRepBuilderAPI_MakeEdge(anArcEnd, anOrigin).Edge());
  aWireBuilder.Add(BRepBuilderAPI_MakeEdge(anOrigin, anArcStart).Edge());
  ASSERT_TRUE(aWireBuilder.IsDone());

  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(anOrigin, gp_Dir(0.0, 0.0, 1.0)),
                                       aWireBuilder.Wire());
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  BRepGProp_Face   anOrderFace(aFace);
  BRepGProp_Domain anOrderDomain(aFace);
  int              aFirstOrder        = 0;
  bool             hasDifferentOrders = false;
  size_t           aNbBoundaries      = 0;
  for (; anOrderDomain.More(); anOrderDomain.Next(), ++aNbBoundaries)
  {
    ASSERT_TRUE(anOrderFace.Load(anOrderDomain.Value()));
    const int anOrder = anOrderFace.IntegrationOrder();
    if (aNbBoundaries == 0)
    {
      aFirstOrder = anOrder;
    }
    else
    {
      hasDifferentOrders = hasDifferentOrders || anOrder != aFirstOrder;
    }
  }
  EXPECT_EQ(aNbBoundaries, 3u);
  EXPECT_TRUE(hasDifferentOrders);

  BRepGProp_Face   aFixedFace(aFace);
  BRepGProp_Domain aFixedDomain(aFace);
  BRepGProp_Sinert aFixed(aFixedFace, aFixedDomain, anOrigin);

  BRepGProp_Face   anAdaptiveFace(aFace);
  BRepGProp_Domain anAdaptiveDomain(aFace);
  BRepGProp_Sinert anAdaptive(anAdaptiveFace, anAdaptiveDomain, anOrigin, 1.0e-8);

  EXPECT_GE(anAdaptive.GetEpsilon(), 0.0);
  EXPECT_LE(anAdaptive.GetEpsilon(), 1.0e-8);
  EXPECT_NEAR(anAdaptive.Mass(), M_PI * aRadius * aRadius / 4.0, 1.0e-10);
  EXPECT_NEAR(anAdaptive.CentreOfMass().X(), 4.0 * aRadius / (3.0 * M_PI), 1.0e-7);
  EXPECT_NEAR(anAdaptive.CentreOfMass().Y(), 4.0 * aRadius / (3.0 * M_PI), 1.0e-7);
  EXPECT_NEAR(anAdaptive.CentreOfMass().Z(), 0.0, 1.0e-12);
  expectGPropsNear(anAdaptive, aFixed, 5.0e-5);

  BRepGProp_Face   aFallbackFace(aFace);
  BRepGProp_Domain aFallbackDomain(aFace);
  BRepGProp_Sinert aFallback(aFallbackFace, aFallbackDomain, anOrigin, 1.0e-2);
  EXPECT_DOUBLE_EQ(aFallback.GetEpsilon(), 1.0e-2);
  expectGPropsNear(aFallback, aFixed, 5.0e-5);
}

TEST(BRepGPropTest, AdaptiveVolumeProperties_Cylinder)
{
  const double       aRadius   = 3.0;
  const double       aHeight   = 7.0;
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(aRadius, aHeight).Shape();

  GProp_GProps aFixed;
  BRepGProp::VolumeProperties(aCylinder, aFixed);
  GProp_GProps anAdaptive;
  const double anError = BRepGProp::VolumeProperties(aCylinder, anAdaptive, 1.0e-8);

  EXPECT_GE(anError, 0.0);
  EXPECT_LE(anError, 1.0e-8);
  EXPECT_NEAR(anAdaptive.Mass(), M_PI * aRadius * aRadius * aHeight, 1.0e-10);
  EXPECT_NEAR(anAdaptive.CentreOfMass().X(), 0.0, 1.0e-12);
  EXPECT_NEAR(anAdaptive.CentreOfMass().Y(), 0.0, 1.0e-12);
  EXPECT_NEAR(anAdaptive.CentreOfMass().Z(), aHeight * 0.5, 1.0e-12);
  const double aMass = M_PI * aRadius * aRadius * aHeight;
  EXPECT_NEAR(anAdaptive.MatrixOfInertia().Value(1, 1),
              aMass * (3.0 * aRadius * aRadius + aHeight * aHeight) / 12.0,
              2.0e-4);
  EXPECT_NEAR(anAdaptive.MatrixOfInertia().Value(2, 2),
              aMass * (3.0 * aRadius * aRadius + aHeight * aHeight) / 12.0,
              2.0e-4);
  EXPECT_NEAR(anAdaptive.MatrixOfInertia().Value(3, 3), aMass * aRadius * aRadius / 2.0, 2.0e-4);
  expectGPropsNear(anAdaptive, aFixed, 2.0e-4);
}

TEST(BRepGPropTest, AdaptivePlaneLimitedInertia)
{
  const gp_Dir            aNormal(0.0, 0.0, 1.0);
  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp_Pnt(0.0, 0.0, 2.0), aNormal), 0.0, 3.0, 0.0, 4.0);
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();
  const gp_Pln      aReferencePlane(gp_Pnt(0.0, 0.0, 0.0), aNormal);
  const gp_Pnt      aLocation(0.0, 0.0, 0.0);

  BRepGProp_Face   aFixedFace(aFace);
  BRepGProp_Domain aFixedDomain(aFace);
  BRepGProp_Vinert aFixed(aFixedFace, aFixedDomain, aReferencePlane, aLocation);

  BRepGProp_Face   anAdaptiveFace(aFace);
  BRepGProp_Domain anAdaptiveDomain(aFace);
  BRepGProp_Vinert anAdaptive(anAdaptiveFace, anAdaptiveDomain, aReferencePlane, aLocation, 1.0e-8);

  const gp_Mat anAdaptiveInertia = anAdaptive.MatrixOfInertia();
  EXPECT_NEAR(anAdaptiveInertia.Value(1, 1), 160.0, 1.0e-10);
  EXPECT_NEAR(anAdaptiveInertia.Value(2, 2), 104.0, 1.0e-10);
  EXPECT_NEAR(anAdaptiveInertia.Value(3, 3), 200.0, 1.0e-10);
  EXPECT_NEAR(anAdaptiveInertia.Value(1, 2), -72.0, 1.0e-10);
  EXPECT_NEAR(anAdaptiveInertia.Value(1, 3), -36.0, 1.0e-10);
  EXPECT_NEAR(anAdaptiveInertia.Value(2, 3), -48.0, 1.0e-10);
  expectGPropsNear(anAdaptive, aFixed, 1.0e-10);
}

TEST(BRepGPropTest, NaturalInfinitePlane_UsesInfiniteArithmetic)
{
  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  TopoDS_Face aFace;
  BRep_Builder().MakeFace(aFace, aPlane, Precision::Confusion());

  BRepGProp_Face   aFaceTool(aFace);
  BRepGProp_Sinert aProperties(aFaceTool, gp_Pnt(0.0, 0.0, 0.0));
  EXPECT_TRUE(Precision::IsPositiveInfinite(aProperties.Mass()));
}

TEST(BRepGPropTest, LinearProperties_SkipShared)
{
  BRepPrimAPI_MakeBox aBox(10.0, 10.0, 10.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  // With SkipShared = false, shared edges are counted multiple times
  GProp_GProps aPropsNotSkipped;
  BRepGProp::LinearProperties(aShape, aPropsNotSkipped, false);

  // With SkipShared = true, each edge is counted once
  GProp_GProps aPropsSkipped;
  BRepGProp::LinearProperties(aShape, aPropsSkipped, true);

  // Box has 12 edges, each of length 10 = total 120
  EXPECT_NEAR(aPropsSkipped.Mass(), 120.0, Precision::Confusion())
    << "Total edge length of 10x10x10 box with SkipShared=true should be 120";

  // Without SkipShared, each edge is counted for each face it belongs to (2 faces per edge)
  // So total = 240
  EXPECT_NEAR(aPropsNotSkipped.Mass(), 240.0, Precision::Confusion())
    << "Total edge length with SkipShared=false should be double";
}

// Test OCC49: GProp_PrincipalProps::HasSymmetryAxis - cylinder has symmetry, cut does not.
// Migrated from QABugs_16.cxx OCC49
TEST(BRepGPropTest, OCC49_CylinderHasSymmetryAxis)
{
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(10., 20.).Shape();

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aCylinder, aProps);
  const GProp_PrincipalProps aPrincipal = aProps.PrincipalProperties();
  EXPECT_TRUE(aPrincipal.HasSymmetryAxis());
}

TEST(BRepGPropTest, OCC49_CutShapeHasNoSymmetryAxis)
{
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(10., 20.).Shape();
  const TopoDS_Shape aBox      = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();

  BRepAlgoAPI_Cut aCut(aCylinder, aBox);
  ASSERT_TRUE(aCut.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aCut.Shape(), aProps);
  const GProp_PrincipalProps aPrincipal = aProps.PrincipalProperties();
  EXPECT_FALSE(aPrincipal.HasSymmetryAxis());
}

// OCC8797: Verify that GCPnts_AbscissaPoint::Length and BRepGProp::LinearProperties
// produce consistent arc-length values for a degree-3 BSpline curve with 7 poles.
// Both methods must agree within a tight relative tolerance.

TEST(BRepGPropTest, OCC8797_BSplineLengthConsistencyAbscissaVsLinearProperties)
{
  NCollection_Array1<gp_Pnt> aPoles(0, 6);
  aPoles(0) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(2) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(3.0, 0.0, 0.0);
  aPoles(4) = gp_Pnt(4.0, 1.0, 0.0);
  aPoles(5) = gp_Pnt(5.0, 1.0, 0.0);
  aPoles(6) = gp_Pnt(6.0, 0.0, 0.0);

  NCollection_Array1<double> aKnots(0, 2);
  aKnots(0) = 0.0;
  aKnots(1) = 0.5;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(0, 2);
  aMults(0) = 4;
  aMults(1) = 3;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  ASSERT_FALSE(aSpline.IsNull());
  EXPECT_EQ(aSpline->NbPoles(), 7);
  EXPECT_EQ(aSpline->NbKnots(), 3);

  // Method 1: GCPnts_AbscissaPoint::Length
  GeomAdaptor_Curve anAdaptor(aSpline);
  const double      aLengthAbscissa = GCPnts_AbscissaPoint::Length(anAdaptor);
  EXPECT_GT(aLengthAbscissa, 0.0);

  // Method 2: BRepGProp::LinearProperties on the equivalent edge
  const TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aSpline);
  GProp_GProps      aEdgeProps;
  BRepGProp::LinearProperties(aEdge, aEdgeProps);
  const double aLengthGProp = aEdgeProps.Mass();
  EXPECT_GT(aLengthGProp, 0.0);

  // Both methods must agree within 0.1 %
  EXPECT_NEAR(aLengthAbscissa, aLengthGProp, aLengthGProp * 1e-3);
}

// Regression: a degenerate edge whose ONLY representation is a Bezier/BSpline-type
// curve-on-surface pcurve (no 3D curve) used to SIGSEGV inside
// BRepGProp_EdgeTool::IntegrationOrder, which read the pole count via BAC.Curve().Curve()
// (null when there is no 3D curve) instead of the adaptor's own NbPoles(). This is the
// shape BRepBuilderAPI_Sewing produces reconciling near-coincident vertices between two
// faces that do not share an edge outright.
TEST(BRepGPropTest, LinearProperties_DegenerateEdgeWithBSplinePCurveNoCrash)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));

  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(0.1, 0.05));
  aPoles.SetValue(3, gp_Pnt2d(0.2, -0.05));
  aPoles.SetValue(4, gp_Pnt2d(0.3, 0.0));

  NCollection_Array1<double> aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);

  NCollection_Array1<int> aMults(1, 2);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  occ::handle<Geom2d_BSplineCurve> aPCurve = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);

  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.UpdateEdge(anEdge, aPCurve, aPlane, TopLoc_Location(), Precision::Confusion());
  aBuilder.Range(anEdge,
                 aPlane,
                 TopLoc_Location(),
                 aPCurve->FirstParameter(),
                 aPCurve->LastParameter());
  aBuilder.Degenerated(anEdge, true);

  GProp_GProps aProps;
  BRepGProp::LinearProperties(anEdge, aProps);

  // Reaching this point at all is the regression check: IntegrationOrder used to SIGSEGV
  // before returning.
  EXPECT_TRUE(std::isfinite(aProps.Mass()));
  EXPECT_GE(aProps.Mass(), 0.0);
}
